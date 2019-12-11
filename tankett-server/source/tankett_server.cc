// tankett_server.cc

#include "tankett_server.h"
#include <dos.h>
#include "gameManager.h"

namespace tankett {
	server::server()
		: running_(false)
		, entityManager_(new serverEntityManager()) {

		key = crypt::generator::random_key();
	}

	bool server::init() {
		if (!network_init()) {
			debugf("[err] could not initialize network!");
			return false;
		}

		dynamic_array<ip_address> addressess;
		if (!ip_address::local_addresses(addressess)) {
			debugf("[err] could not retreive local ip addressess!");
			return false;
		}

		debugf("[Info] found %d ip addressess", (int)addressess.size());

		for (int index = 0; index < (int)addressess.size(); index++) {
			debugf("[Info]   #%d - %s", index, addressess[index].as_string());
		}

		local_ = addressess[0];
		local_.set_port(PROTOCOL_PORT);

		socket_.open(local_);

		running_ = true;

		return running_;
	}

	void server::shut() {
		delete(entityManager_);
		network_shut();
	}

	void server::run() {
		debugf("[Info] server running at %s", local_.as_string());
		const time start = time::now();
		time current = time::now();
		while (running_) {
			time now = time::now();
			time dt = now - current;
			current = now;

			receive(dt);
			update(dt);
			send(dt);
		}
		int x = 0;
	}
#pragma region Update
	void server::update(const time& dt) {
		if (connectedClientCount() < 2) {
			state_ = WAITING_FOR_PLAYER;
			currentRoundTime_ = ROUND_TIME;
		}
		switch (state_) {
		case ROUND_RUNNING: {
			currentRoundTime_ -= dt.as_seconds();
			entityManager_->update(dt);
			entityManager_->manageCollisions();
			if (currentRoundTime_ < 0) {
				EndRound();
			}
		}
		break;
		case WAITING_FOR_PLAYER: {
			if (connectedClientCount() >= 2) {
				StartRound();
			}
		}
								 break;
		case ROUND_END: {
			currentRoundTime_ -= dt.as_seconds();
			if (currentRoundTime_ < 0) {
				StartRound();
			}			
		}
						break;
		default:
			break;
		}
		DisconnectAfterTimeout();
		updateClientData();
	}

	void server::updateClientData() {
		for (int i = 0; i < 4; i++) {
			serverTank* t = entityManager_->getTank(i);
			clientData[i].alive = t->isEnabled;
			clientData[i].angle = t->transform_.rotation_;
			clientData[i].position = t->transform_.position_;
			dynamic_array<bullet_data> bulletdata;
			for (serverBullet* b : t->bullets_) {
				if (b->isEnabled) {
					bullet_data bd;
					bd.position = b->transform_.position_;
					bd.id = (uint8)b->id_;
					bulletdata.push_back(bd);
				}
			}
			clientData[i].bullet_count = (uint8)bulletdata.size();
			for (int j = 0; j < bulletdata.size(); j++) {
				clientData[i].bullets[j] = bulletdata[j];
			}
		}
	}
	void server::StartRound() {
		state_ = ROUND_RUNNING;
		gameManager::ResetScore();
		currentRoundTime_ = ROUND_TIME;
		for (int i = 0; i < connectedClientCount(); i++) {
			entityManager_->getTank(clients_[i].id_)->isEnabled = true;
			entityManager_->getTank(clients_[i].id_)->isAlive = true;
			entityManager_->getTank(clients_[i].id_)->SetPosition(SPAWN_POINTS[clients_[i].id_]);
		}
	}
	void server::EndRound() {
		state_ = ROUND_END;
		currentRoundTime_ = ROUNDENDTIME;
		entityManager_->disableAllTanks();
	}
#pragma endregion

#pragma region Receive
	void server::receive(const time& dt) {
		ip_address remote;
		byte_stream stream(1024 * 4, dst_);
		byte_stream_reader reader(stream);
		while (!reader.eos()) {
			if (!socket_.recv_from(remote, stream)) {
				return;
			}
			const uint8 type = reader.peek();
			if (type != PACKET_TYPE_PAYLOAD) {
				int i = 0;
			}

			switch (type) {
			case PACKET_TYPE_CONNECTION_REQUEST: {
				protocol_connection_request msg;
				if (!msg.serialize(reader)) {
					debugf("[Warning] Tried to serialize connection request but it didnt work for some reason.");
					break;
				}
				processConnectionRequest(remote, msg);
			}
												 break;
			case PACKET_TYPE_CHALLENGE_RESPONSE: {
				protocol_challenge_response msg;
				if (!msg.serialize(reader)) {
					debugf("[Warning] Tried to serialize challenge response but it didnt work for some reason.");
					break;
				}
				processChallengeResponse(remote, msg);
			}
												 break;
			case PACKET_TYPE_PAYLOAD: {
				protocol_payload msg;
				if (!msg.serialize(reader)) {
					debugf("[Warning] Tried to serialize payload but it didnt work for some reason.");
					break;
				}
				processPayload(remote, msg);
			}
									  break;
			case PACKET_TYPE_DISCONNECT: {
				protocol_disconnect msg;
				if (!msg.serialize(reader)) {
					debugf("[Warning] Tried to serialize disconnect message but it didnt work for some reason.");
					break;
				}
				processDisconnect(remote);
			}
			default:
				return;
				break;
			}
		}
	}

	bool server::hasAddress(ip_address addr) {
		for (client client : clients_) {
			if (client.address_ == addr) {
				return true;
			}
		}
		return false;
	}

	void server::processConnectionRequest(ip_address remote, protocol_connection_request& msg) {
		if (!hasAddress(remote)) {
			client client;
			client.address_ = remote;
			client.state_ = CHALLENGE;
			client.client_key_ = msg.client_key_;			
			client.id_ = GetUnusedClientID();
			client.latest_receive_time_ = time::now();
			clientData[client.id_].client_id = client.id_;
			clients_.push_back(client);
			debugf("[Info] New client added: %s", remote.as_string());
		}
	}

	void server::processChallengeResponse(ip_address remote, protocol_challenge_response& msg) {
		for (client& client : clients_) {
			if (client.address_ == remote) {
				crypt::xorinator xorinator(client.client_key_, key);
				uint64 encryptedKeys = 0;
				client.xorinator_ = xorinator;
				xorinator.encrypt(sizeof(uint64), (uint8*)& encryptedKeys);
				if (encryptedKeys == msg.combined_key_) {
					if (client.state_ != CONNECTED) {
						client.state_ = CONNECTED;
						debugf("[Info] Client connected: %s", client.address_.as_string());
						SpawnTank(client.id_);
					}
				}
				else {
					client.state_ = DENIED;
					debugf("[Info] Client denied: %s", client.address_.as_string());
				}
			}
		}
	}

	void server::processPayload(ip_address remote, protocol_payload& msg) {
		for (client& client : clients_) {
			if (client.address_ == remote) {
				if (msg.is_newer(client.latest_received_sequence_)) {
					byte_stream stream(msg.length_, msg.payload_);
					byte_stream_reader reader(stream);

					client.latest_received_sequence_ = msg.sequence_;

					client.xorinator_.decrypt(msg.length_, msg.payload_);

					entityManager_->getTank(client.id_)->SetPreviousPosition();

					bool invalidMessageReceived = false;
					while (!reader.eos() && !invalidMessageReceived) {
						network_message_type type = (network_message_type)reader.peek();

						switch (type) {
						case NETWORK_MESSAGE_PING: {
							network_message_ping message;
							if (!message.serialize(reader)) {
								auto error = network_error::get_error();
							}
							else {
								parsePingMessage(message, client.id_);
							}
						}
												   break;
						case NETWORK_MESSAGE_CLIENT_TO_SERVER: {
							message_client_to_server message;
							if (!message.serialize(reader)) {
								auto error = network_error::get_error();
							}
							else {
								time deltaReceiveTime = time::now() - client.latest_receive_time_;
								client.inputNumber_ = message.input_number;
								if (state_ == ROUND_RUNNING) entityManager_->parseClientMessage(message, client.id_, deltaReceiveTime);
							}
						}
															   break;
						case NETWORK_MESSAGE_COUNT: {

						}
													break;
						default:
							invalidMessageReceived = true;
							break;
						}
					}
					client.latest_receive_time_ = time::now();
				}
			}
		}
	}


	void server::processDisconnect(ip_address remote) {
		int iterator = -1;
		for (int i = 0; i < clients_.size(); i++) {
			if (clients_[i].address_ == remote) {
				iterator = i;
				entityManager_->resetTank(i);
			}
		}
		if (iterator >= 0) {
			clients_.erase(clients_.begin() + iterator);
			clientData[iterator].connected = false;
			clientData[iterator].client_id = 255;
		}
	}
	void server::parsePingMessage(network_message_ping message, uint8 clientID) {
		int it = 0;
		bool hasPing = false;
		for (int i = 0; i < clients_.size(); i++) {
			if (clients_[i].id_ == clientID) {
				for (std::pair<uint8, time> pair : clients_[i].pings_) {
					if (pair.first == message.sequence_) {
						clients_[i].ping_ = time::now() - pair.second;
						hasPing = true;
						break;
					}
					it++;
				}
				if (clients_[i].pings_.size() > it) {
					if (hasPing) clients_[i].pings_.erase(clients_[i].pings_.begin() + it);
				}
			}
		}


	}
#pragma endregion

#pragma region Send
	void server::send(const time& dt) {
		send_accumulator_ += dt;
		if (send_accumulator_ > time(100)) {
			send_accumulator_ -= time(100);

			for (client& client : clients_) {
				switch (client.state_) {
				case CHALLENGE:
					challengeClient(client);
					break;
				case CONNECTED:
					queueMessage(client);
					queuePing(client);


					break;
				default:
					break;
				}
			}

			process_client_queues();

		}
	}



	void server::process_client_queues() {
		// note: use the same server sequence number for all sends
		const uint32 current_sequence_number = server_sequence_++;
		protocol_payload packet(current_sequence_number);

		// note: iterate over all clients and pack messages into payload, then send
		for (auto& client : clients_) {
			if (client.state_ == CONNECTED) {
				auto& messages = client.messages_;

				// note: calculate the number of messages we can pack into the payload
				int messages_evaluated = 0;
				byte_stream stream(sizeof(packet.payload_), packet.payload_);
				byte_stream_evaluator evaluator(stream);
				for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
					if (!(*iter)->write(evaluator)) {
						break;
					}

					messages_evaluated++;
				}

				// note: actual packing of messages into the payload
				int messages_packed = 0;
				byte_stream_writer writer(stream);
				for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
					if (!(*iter)->write(writer)) {
						break;
					}

					messages_packed++;
					if (messages_evaluated == messages_packed) {
						break;
					}
				}

				// note: did the packing succeed?
				if (messages_evaluated != messages_packed) {
					debugf("[err] %s - sequence: %u - messages_evaluated != messages_packed",
						current_sequence_number,
						client.address_.as_string());
					continue;
				}

				// note: finalize packet by setting the payload length
				packet.length_ = (uint16)stream.length();

				// note: ... then we encrypt it!
				client.xorinator_.encrypt(packet.length_, packet.payload_);

				// note: send payload to client
				if (send_payload(client.address_, packet)) {
					// note: if sending succeeds we can:
					//       - delete messages sent
					//       - remove them from client message queue
					for (int remove_message_index = 0;
						remove_message_index < messages_packed;
						remove_message_index++) {
						delete messages.at(remove_message_index);
					}

					messages.erase(messages.begin(), messages.begin() + messages_packed);
				}
			}
		}
	}

	bool server::send_payload(const ip_address& remote, protocol_payload& packet) {
		uint8 buffer[2048] = {};
		byte_stream stream(sizeof(buffer), buffer);
		byte_stream_writer writer(stream);

		if (!packet.serialize(writer)) {
			return false;
		}

		if (!socket_.send_to(remote, stream)) {
			auto err = network_error::get_error();
			if (err.is_critical()) {
				debugf("[err] %s - send error (%d) %s",
					remote.as_string(),
					err.code_,
					err.as_string());
			}

			return false;
		}

		return true;
	}

	void server::queueMessage(client& pClient) {
		message_server_to_client* msg = new message_server_to_client;
		msg->client_count = connectedClientCount();
		int it = 0;
		for (int i = 0; i < 4; i++) {
			clientData[i].eliminations = gameManager::score_[i];
			clientData[i].angle = entityManager_->getTank(i)->turretRotation_;
			clientData[i].alive = entityManager_->getTank(i)->isAlive;
			if (clientData[i].connected) {
				msg->client_data[it] = clientData[i];
				it++;
			}
			
		}
		for (int i = 0; i < clients_.size(); i++) {
			clientData[i].ping = (uint32)clients_[i].ping_.as_milliseconds();
		}
		msg->game_state = state_;
		msg->round_time = currentRoundTime_;
		msg->receiver_id = pClient.id_;
		msg->type_ = NETWORK_MESSAGE_SERVER_TO_CLIENT;
		msg->input_number = pClient.inputNumber_;
		msg->timestamp = time::now().as_milliseconds();
		pClient.messages_.push_back(msg);
	}

	void server::queuePing(client& pClient) {
		network_message_ping* msg = new network_message_ping;
		msg->sequence_ = pClient.pingSequence_;
		pClient.messages_.push_back(msg);
		bool hasSequence = false;
		for (std::pair<uint8,time> pair : pClient.pings_) {
			if (pair.first == pClient.pingSequence_) {
				pair.second = time::now();
				hasSequence = true;
			}
		}
		if (!hasSequence) pClient.pings_.push_back(std::make_pair(pClient.pingSequence_, time::now()));
		pClient.pingSequence_++;
	}

	uint8 server::GetUnusedClientID() {
		for (uint8 i = 0; i < 4; i++) {
			if (clientData[i].connected == false) return i;
		}
		return 255;
	}

	void server::DisconnectAfterTimeout() {
		int it = 0;
		bool erase = false;
		for (int i = 0; i < clients_.size(); i++) {
			if (time::now().as_seconds() - clients_[i].latest_receive_time_.as_seconds() > TIME_OUT_THRESHOLD_SECONDS) {
				it = i;
				erase = true;
			}
		}
		if (erase) {
			clients_.erase(clients_.begin() + it);
			clientData[it].connected = false;
			clientData[it].client_id = 255;
		}
	}

	void server::challengeClient(client& client) {
		protocol_connection_challenge challenge(key);

		byte_stream stream(1024 * 4, dst_);
		byte_stream_writer writer(stream);

		challenge.serialize(writer);
		if (!socket_.send_to(client.address_, stream)) {
			debugf("[Warning] Tried to send but something went wrong");
		}
		//debugf("[Info] Client challenged: %s", client.address_.as_string());

	}

	uint8 server::connectedClientCount() {
		uint8 count = 0;
		for (int i = 0; i < 4; i++) {
			if (clientData[i].connected) count++;
		}
		return count;
	}

#pragma endregion

	void server::SpawnTank(int id) {
		if (!clientData[id].connected) {
			clientData[id].connected = true;
			clientData[id].position = SPAWN_POINTS[id];
			entityManager_->spawnTank(id);
			return;
		}
	}
} // !tankett

