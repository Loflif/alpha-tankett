// tankett_client.cc

#include "tankett_client.h"

namespace alpha {
	application* application::create(int& width, int& height, string& title) {
		title = "tankett-wars-client";
		return new tankett::client_app;
	}
} // !alpha

namespace tankett {

#pragma region Intialisation

	client_app::client_app()
		: client_key_(0)
		, server_key_(0)
		, state_(INIT)
		, entityManager_(new entityManager()){

	}



	bool client_app::enter() {
		current_ = time::now();

		debugText_.set_text("IP: ...");
		debugTextTransform_.set_position({ 2, 2 });

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

		createLevel();
		createBulletBuffer();

		return true;
	}

	void client_app::initializeUI() {
		SetUIElement(timer, "00:00", 2, vector2(40 * TILE_SIZE, 32.2f * TILE_SIZE));
	}

	void client_app::SetUIElement(UIElement& element, const char* pText, int32 pSize, vector2 pPos, uint32 pColor) {
		element.text_.set_text(pText);
		element.text_.set_scale(pSize);
		element.transform_.set_position(pPos);
		element.text_.set_color(pColor);

	}

	void client_app::createTile(vector2 p_pos) {
		sprite spr(wallTexture_, vector2(TILE_SIZE, TILE_SIZE));
		entities_.push_back(new tile(spr, p_pos.x_, p_pos.y_));
	}

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

#pragma region Termination

	void client_app::exit() {
		network_shut();

		delete(entityManager_);
	}
#pragma endregion

#pragma region Update

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

	void client_app::checkInput() {
		message_client_to_server* msg = new message_client_to_server;
		bool shoot = false;
		bool right = false;
		bool left = false;
		bool down = false;
		bool up = false;
		if (mouse_.is_pressed(MOUSE_BUTTON_LEFT)) {
			shoot = true;
			//fireBullet(playerTank_);
		}
		if (keyboard_.is_down(KEYCODE_D)) {
			right = true;
		}
		if (keyboard_.is_down(KEYCODE_A)) {
			left = true;
		}
		if (keyboard_.is_down(KEYCODE_S)) {
			down = true;
		}
		if (keyboard_.is_down(KEYCODE_W)) {
			up = true;
		}
		msg->set_input(shoot, right, left, down, up);

		vector2 mousePosition((float)mouse_.x_, (float)mouse_.y_);
		vector2 aimVector = vector2(mousePosition - playerTank_->transform_.position_).normalized();

		msg->turret_angle = atan2(aimVector.y_, aimVector.x_) * (180 / PI);
		msg->type_ = NETWORK_MESSAGE_CLIENT_TO_SERVER;
		messages_.push_back(msg);
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
#pragma endregion

#pragma region Receive

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
					debugText_.set_text("CONNECTED");
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
					debugText_.set_text("CONNECTION DENIED: %d", connection_denied.reason_);
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
		SetTimer(pMessage.round_time);
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

	void client_app::SetTimer(float pTime) {
		int seconds, minutes;
		minutes = (int)pTime / 60;
		seconds = (int)pTime - minutes * 60;

		string timerText;
		if (minutes < 10) timerText += "0";
		timerText += (std::to_string(minutes) + ":");
		if (seconds < 10) timerText += "0";
		timerText += std::to_string(seconds);
		timer.text_.set_text(timerText.c_str());
	}
#pragma endregion

	void client_app::update(time dt) {
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->update(keyboard_, mouse_, dt);
			}
		}
	}

#pragma region CollisionHandling

	void client_app::manageCollisions() {
		for (int i = 0; i < entities_.size(); i++) {
			for (int j = 0; j < entities_.size(); j++) {
				if (isCollisionPair(entities_[i], entities_[j])) {
					if ((j != i) && (checkCollision(entities_[i], entities_[j]))) {
						if (entities_[i]->isEnabled && entities_[j]->isEnabled) {
							entities_[i]->onCollision(entities_[j]);
							entities_[j]->onCollision(entities_[i]);
						}
					}
				}
			}
		}
	}

	bool client_app::checkCollision(IEntity* firstEntity, IEntity* secondEntity) {
		rectangle firstRectangle = firstEntity->collider_;
		rectangle secondRectangle = secondEntity->collider_;

		float firstTop, firstBottom, firstLeft, firstRight, secondTop, secondBottom, secondLeft, secondRight;

		firstTop = firstRectangle.y_;
		firstBottom = firstRectangle.y_ + firstRectangle.height_;
		firstLeft = firstRectangle.x_;
		firstRight = firstRectangle.x_ + firstRectangle.width_;

		secondTop = secondRectangle.y_;
		secondBottom = secondRectangle.y_ + secondRectangle.height_;
		secondLeft = secondRectangle.x_;
		secondRight = secondRectangle.x_ + secondRectangle.width_;


		if (firstTop > secondBottom || firstBottom < secondTop || firstLeft > secondRight || firstRight < secondLeft) {
			return false;
		}
		else {
			return true;
		}
	}

	bool client_app::isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity) {
		for (unsigned int i = 0; i < collisionPairs_.size(); i++) {
			if ((collisionPairs_[i].first == pFirstEntity->type_ && collisionPairs_[i].second == pSecondEntity->type_) ||
				(collisionPairs_[i].first == pSecondEntity->type_ && collisionPairs_[i].second == pFirstEntity->type_)) {
				return true;
			}
		}
		return false;
	}

#pragma endregion

	void client_app::fireBullet(tank* t) {
		if (t->shootingCooldown_ > 0)
			return;
		for (bullet* b : bullets_) {
			if (!b->isEnabled) {
				vector2 tPos = t->transform_.position_;
				b->fire(tPos.x_, tPos.y_, t->aimVector_);
				t->bullets_.push_back(b);
				t->shootingCooldown_ = t->FIRE_RATE_;
				break;
			}
		}
	}

	void client_app::render() {
		render_system_.clear(0xff0e1528); //Background Color
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->render(render_system_);
			}
		}
		render_system_.render(text_, transform_);
	}
} // !tankett