// tankett_server.cc

#include "tankett_server.h"
#include <dos.h>

namespace tankett {
	server::server()
		: running_(false) {

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
	}

	void server::update(const time& dt) {

	}

	void server::receive(const time& dt) {
		ip_address remote;
		byte_stream stream(1024 * 4, dst_);
		byte_stream_reader reader(stream);
		if (!socket_.recv_from(remote, stream)) {
			return;
		}
		while (!reader.eos()) {
			const uint8 type = reader.peek();

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
					debugf("[Warning] Tried to serialize challenge response but it didnt work for some reason.");
					break;
				}
				processPayload(remote, msg, dt);
			}
									break;
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
				xorinator.encrypt(sizeof(uint64), (uint8*)&encryptedKeys);
				if (encryptedKeys == msg.combined_key_) {
					if (client.state_ != CONNECTED) {
						client.state_ = CONNECTED;
						debugf("[Info] Client connected: %s", client.address_.as_string());
						client.id_ = connectedClientCount();
						SpawnTank();
					}
				}
				else {
					client.state_ = DENIED;
					debugf("[Info] Client denied: %s", client.address_.as_string());
				}
			}
		}
	}

	void server::processPayload(ip_address remote, protocol_payload& msg, const time& dt) {
		for (client& client : clients_) {
			if (client.address_ == remote) {
				if (msg.is_newer(client.latest_received_sequence_)) {
					byte_stream stream(msg.length_, msg.payload_);
					byte_stream_reader reader(stream);

					client.xorinator_.decrypt(msg.length_, msg.payload_);

					network_message_type type = (network_message_type)reader.peek();

					switch (type) {
					case tankett::NETWORK_MESSAGE_PING: {
					}
														break;
					case tankett::NETWORK_MESSAGE_CLIENT_TO_SERVER: {
						message_client_to_server message;
						if (!message.serialize(reader)) {
							auto error = network_error::get_error();
						}
						else {
							parseClientMessage(message, client, dt);
						}
					}
																	break;
					case tankett::NETWORK_MESSAGE_COUNT: {

					}
														 break;
					default:
						break;
					}
					
				}
			}
		}
	}
#pragma region ParseInput
	void server::parseClientMessage(message_client_to_server message, client& pClient, const time& dt) {
		vector2 targetDirection = targetMoveDirection(message);
		float speed = 4 * dt.as_seconds();
		clientData[pClient.id_-1].position += targetDirection * speed;
		
	}

	vector2 server::targetMoveDirection(message_client_to_server message) {
		if (message.get_input(message_client_to_server::UP) && message.get_input(message_client_to_server::RIGHT)) return { 0.7071f ,-0.7071f };  //Normalised Diagonal Vector
		if (message.get_input(message_client_to_server::UP) && message.get_input(message_client_to_server::LEFT)) return { -0.7071f ,-0.7071f };
		if (message.get_input(message_client_to_server::DOWN) && message.get_input(message_client_to_server::LEFT)) return { -0.7071f ,0.7071f };
		if (message.get_input(message_client_to_server::DOWN) && message.get_input(message_client_to_server::RIGHT)) return { 0.7071f ,0.7071f };
		if (message.get_input(message_client_to_server::RIGHT))  return { 1.0f ,0 };
		if (message.get_input(message_client_to_server::LEFT))  return { -1.0f,0 };
		if (message.get_input(message_client_to_server::UP))	return { 0,-1.0f };
		if (message.get_input(message_client_to_server::DOWN))	return { 0, 1.0f };
		return { 0,0 };
	}
#pragma endregion

	void server::processDisconnect(ip_address remote, protocol_payload& msg) { //TODO: TEST!
		int iterator = -1;
		for (int i = 0; i < clients_.size(); i++) {
			if (clients_[i].address_ == remote) {
				iterator = i;
			}
		}
		if (iterator >= 0) {
			clients_.erase(clients_.begin() + iterator);
		}
	}
#pragma region Send
	void server::send(const time& dt) {
		send_accumulator_ += dt;
		if (send_accumulator_ > time(33)) {
			send_accumulator_ -= time(33);

			for (client& client : clients_) {
				switch (client.state_) {
				case CHALLENGE:
					challengeClient(client);
					break;
				case CONNECTED:
					queueMessage(client);
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
		msg->client_count = (uint8)clients_.size();
		for (int i = 0; i < 4; i++) {
			msg->client_data[i] = clientData[i];
		}
		msg->receiver_id = pClient.id_;
		msg->type_ = NETWORK_MESSAGE_SERVER_TO_CLIENT;
		pClient.messages_.push_back(msg);
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
		for (client client : clients_) {
			if (client.state_ == CONNECTED) count++;
		}
		return count;
	}

#pragma endregion
	void server::SpawnTank() {
		for (int i = 0; i < 4; i++) {
			if (!clientData[i].connected) {
				clientData[i].connected = true;
				clientData[i].position = spawnPoints[i];
				return;
			}
		}
	}
} // !tankett

