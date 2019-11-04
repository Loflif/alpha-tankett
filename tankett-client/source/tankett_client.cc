// tankett_client.cc

#include "tankett_client.h"

namespace alpha {
	application* application::create(int& width, int& height, string& title) {
		title = "tankett-wars-client";
		return new tankett::client_app;
	}
} // !alpha

namespace tankett {
	client_app::client_app()
		: client_key_(0)
		, server_key_(0)
		, state_(INIT)
		, entityManager_(new entityManager()){

	}

	bool client_app::enter() {
		current_ = time::now();

		text_.set_text("IP: ...");
		transform_.set_position({ 2, 2 });

		network_init();

		state_ = CONNECTION_REQUEST;

		if (!socket_.open()) {
			auto error = network_error::get_error();
			return false;
		}

		client_key_ = crypt::generator::random_key();

		dynamic_array<ip_address> addressess;
		if (!ip_address::local_addresses(addressess)) {
			return false;
		}
		ip_address client(addressess[0].host_ & 0xffffff00, PROTOCOL_PORT);
		client.host_ = client.host_ + 255;
		send_all_ip_ = client;
		send_all_ip_.set_port(PROTOCOL_PORT);

		return true;
	}

#pragma region Intialisation

	//void client_app::createTile(vector2 p_pos) {
	//	sprite spr(wallTexture_, vector2(TILE_SIZE, TILE_SIZE));
	//	entities_.push_back(new tile(spr, p_pos.x_, p_pos.y_));
	//}

	//void client_app::createLevel() {
	//	int rows = std::extent<decltype(LEVEL), 0>::value; // Get the amount of rows
	//	for (size_t row = 0; row < rows; row++) {
	//		int column = 0;
	//		for (TILE_TYPE type : LEVEL[row]) {
	//			if (type == W)
	//				createTile(vector2((float)column * TILE_SIZE, (float)row * TILE_SIZE));
	//			column++;
	//		}
	//	}
	//}

	/*tank* client_app::createTank(vector2 p_pos, uint8 pID, bool pIsLocal) {
		sprite spr(tankTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
		sprite turretSpr(turretTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
		tank* t = new tank(spr, turretSpr, p_pos.x_, p_pos.y_, pID);
		entities_.push_back(t);
		return t;
	}

	void client_app::createBulletBuffer() {

		sprite spr(bulletTexture_, vector2(TILE_SIZE * BULLET_SIZE, TILE_SIZE * BULLET_SIZE));

		for (int i = 0; i < BULLET_MAX; i++) {
			bullet* b = new bullet(spr);
			bullets_.push_back(b);
			entities_.push_back(b);
		}
	}*/

#pragma endregion

	void client_app::exit() {
		network_shut();

		delete(entityManager_);
	}

	bool client_app::tick() {
		const time now = time::now();
		time dt = now - current_;
		current_ = now;

		if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
			return false;
		}

		if (state_ != DISCONNECTED) {
			send(dt);
			receive();
		}
		if (state_ == CONNECTED) {
			entityManager_->update(keyboard_, mouse_, dt);
			messages_.push_back(entityManager_->checkInput(keyboard_, mouse_));
			entityManager_->manageCollisions();
		}
		entityManager_->render(render_system_);

		return true;
	}

	void client_app::send(time dt) {
		send_accumulator += dt;
		if (send_accumulator > time(33)) {
			send_accumulator -= time(33);

			uint8 buffer[2048];

			byte_stream stream(sizeof(buffer), buffer);
			byte_stream_writer writer(stream);

			switch (state_) {
			case CONNECTION_REQUEST:
			{
				protocol_connection_request connection_request(client_key_);

				if (connection_request.serialize(writer)) {
					if (!socket_.send_to(send_all_ip_, stream)) {
						auto error = network_error::get_error();
					}
				}
				break;
			}
			case CHALLENGE_RESPONSE:
			{
				crypt::xorinator xorinator(client_key_, server_key_);
				xorinator_ = xorinator;
				uint64 encryptedKeys = 0;
				xorinator.encrypt(sizeof(uint64), (uint8*)&encryptedKeys);
				protocol_challenge_response challenge_response(encryptedKeys);
				if (challenge_response.serialize(writer)) {
					if (!socket_.send_to(server_ip_, stream)) {
						auto error = network_error::get_error();
					}
				}
				break;
			}
			case CONNECTED:
			{
				protocol_payload payload(send_sequence_);

				pack_payload(payload);
				//send_payload(payload);

				send_sequence_++;
			}
			}
		}
	}
	bool client_app::pack_payload(protocol_payload& pPayload) {
		/*byte_stream stream(sizeof(pPayload.payload_), pPayload.payload_);
		byte_stream_writer writer(stream);*/

		// note: calculate the number of messages we can pack into the payload
		int messages_evaluated = 0;
		byte_stream stream(sizeof(pPayload.payload_), pPayload.payload_);
		byte_stream_evaluator evaluator(stream);
		for (auto iter = messages_.begin(); iter != messages_.end(); ++iter) {
			if (!(*iter)->write(evaluator)) {
				break;
			}

			messages_evaluated++;
		}

		// note: actual packing of messages into the payload
		int messages_packed = 0;
		byte_stream_writer writer(stream);
		for (auto iter = messages_.begin(); iter != messages_.end(); ++iter) {
			if (!(*iter)->write(writer)) {
				break;
			}

			messages_packed++;
			if (messages_evaluated == messages_packed) {
				break;
			}
		}
		pPayload.length_ = (uint16)stream.length();
		xorinator_.encrypt(pPayload.length_, pPayload.payload_);

		if (send_payload(pPayload)) {
			// note: if sending succeeds we can:
			//       - delete messages sent
			//       - remove them from client message queue
			for (int remove_message_index = 0;
				 remove_message_index < messages_packed;
				 remove_message_index++) {
				delete messages_.at(remove_message_index);
			}

			messages_.erase(messages_.begin(), messages_.begin() + messages_packed);
		}
		return true;
	}

	bool client_app::send_payload(protocol_payload& pPayload) {
		uint8 buffer[2048] = {};
		byte_stream stream(sizeof(buffer), buffer);
		byte_stream_writer writer(stream);

		if (!pPayload.serialize(writer)) {
			return false;
		}

		if (!socket_.send_to(server_ip_, stream)) {
			auto err = network_error::get_error();

			return false;
		}

		return true;
	}


	void client_app::receive() {
		uint8 buffer[2048];

		byte_stream stream(sizeof(buffer), buffer);
		byte_stream_reader reader(stream);

		packet_type type;

		ip_address remote;

		while (!reader.eos()) {
			if (!socket_.recv_from(remote, stream)) {
				auto error = network_error::get_error();
				return;
			}
			type = (packet_type)reader.peek();
			switch (type) {
			case PACKET_TYPE_CONNECTION_CHALLENGE: {
				protocol_connection_challenge connection_challenge;
				if (!connection_challenge.serialize(reader)) {
					auto error = network_error::get_error();
				}
				else {
					server_ip_ = remote;
					server_key_ = connection_challenge.server_key_;
					state_ = CHALLENGE_RESPONSE;
				}
				break;
			}
			case PACKET_TYPE_PAYLOAD: {
				protocol_payload payload;
				if (!payload.serialize(reader)) {
					auto error = network_error::get_error();
				}
				else {
					text_.set_text("CONNECTED");
					state_ = CONNECTED;
					parsePayload(payload);
				}
				break;
			}

			case PACKET_TYPE_CONNECTION_DENIED: {
				protocol_connection_denied connection_denied;
				if (!connection_denied.serialize(reader)) {
					auto error = network_error::get_error();
				}
				else {
					text_.set_text("CONNECTION DENIED: %d", connection_denied.reason_);
					state_ = DISCONNECTED;
				}
				break;
			}
			default:
				return;
				break;
			}
		}
	}


#pragma region ParseData

	void client_app::parsePayload(protocol_payload pPayload) {
		byte_stream stream(pPayload.length_, pPayload.payload_);
		byte_stream_reader reader(stream);

		xorinator_.decrypt(pPayload.length_, pPayload.payload_);

		network_message_type type = (network_message_type)reader.peek();

		switch (type) {
		case NETWORK_MESSAGE_PING: {
		}
		break;
		case NETWORK_MESSAGE_SERVER_TO_CLIENT: {
			message_server_to_client message;
			if (!message.serialize(reader)) {
				auto error = network_error::get_error();
			}
			else {
				parseServerMessage(message);
			}
		}
		break;
		case NETWORK_MESSAGE_COUNT: {

		}
		break;
		default:
			break;
		}
	}

	void client_app::parseServerMessage(message_server_to_client pMessage) {
		for (int i = 0; i < pMessage.client_count; i++) {
			if (i == pMessage.receiver_id) {
				if (entityManager_->getLocalTank() == 255)
					entityManager_->setLocalTank(pMessage.receiver_id);
				entityManager_->UpdateLocalTank(pMessage.client_data[i]);
			}
			else {
				entityManager_->UpdateRemoteTank(pMessage.client_data[i]);
			}
		}
	}

#pragma endregion
} // !tankett