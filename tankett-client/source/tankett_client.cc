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
		, state_(INIT) {

		wallTexture_.create_from_file("assets/wallTile.png");
		tankTexture_.create_from_file("assets/tank.png");
		turretTexture_.create_from_file("assets/turret.png");
	}

	bool client_app::enter() {
		current_ = time::now();

		text_.set_text("IP: ...");
		transform_.set_position({ 2, 2 });

		network_init();

		state_ = CONNECTION_REQUEST;

		if (!sock_.open()) {
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
		createTank(vector2(4 * TILE_SIZE, 4 * TILE_SIZE));

		return true;
	}

	void client_app::exit() {
		network_shut();
		for (IEntity* e : entities_) {
			//delete(e);
		}
	}

	bool client_app::tick() {
		// note: deltatime
		const time now = time::now();
		time dt = now - current_;
		current_ = now;



		// note: quit
		if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
			return false;
		}

		// note: connect
		if (keyboard_.is_pressed(KEYCODE_C)) {
		}

		// note: disconnect
		if (keyboard_.is_down(KEYCODE_D)) {
		}

		// note: rendering
		render_system_.clear(0xff0e1528); //Background Color
		render_system_.render(text_, transform_);

		if (state_ != DISCONNECTED) {
			send(dt);
			receive();
		}

		update(dt);
		manageCollisions();
		render();

		return true;
	}
	void client_app::send(time dt) {
		send_accumulator += dt;
		if (send_accumulator > time(100)) {
			send_accumulator -= time(100);

			uint8 buffer[2048];

			byte_stream stream(sizeof(buffer), buffer);

			byte_stream_writer writer(stream);

			switch (state_) {
			case CONNECTION_REQUEST:
			{
				protocol_connection_request connection_request(client_key_);

				if (connection_request.serialize(writer)) {
					if (!sock_.send_to(send_all_ip_, stream)) {
						auto error = network_error::get_error();
					}
				}
				break;
			}
			case CHALLENGE_RESPONSE:
			{
				crypt::xorinator xorinator(client_key_, server_key_);
				uint64 encryptedKeys = 0;
				xorinator.encrypt(sizeof(uint64), (uint8*)& encryptedKeys);
				protocol_challenge_response challenge_response(encryptedKeys);
				if (challenge_response.serialize(writer)) {
					if (!sock_.send_to(server_ip_, stream)) {
						auto error = network_error::get_error();
					}
				}
				break;
			}
			}
		}
	}
	void client_app::receive() {
		uint8 buffer[2048];

		byte_stream stream(sizeof(buffer), buffer);
		byte_stream_reader reader(stream);

		packet_type type;

		ip_address remote;

		if (!sock_.recv_from(remote, stream)) {
			auto error = network_error::get_error();
		}
		else {
			type = (packet_type)reader.peek();
			switch (type) {
			case PACKET_TYPE_CONNECTION_CHALLENGE:
			{
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
			case PACKET_TYPE_PAYLOAD:
			{
				protocol_payload payload;
				if (!payload.serialize(reader)) {
					auto error = network_error::get_error();
				}
				else {
					text_.set_text("CONNECTED");
					state_ = CONNECTED;
				}
				break;
			}

			case PACKET_TYPE_CONNECTION_DENIED:
			{
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
			}
		}
	}

	void client_app::createTile(vector2 p_pos, TILE_TYPE p_type) {
		switch (p_type) {
		case tankett::W: {
			sprite spr(wallTexture_, vector2(TILE_SIZE, TILE_SIZE));
			entities_.push_back(new tile(spr, p_pos.x_, p_pos.y_));
		}
						 break;
		case tankett::E: {
			/* sprite spr(0xffffff);
			 entities_.push_back(new tile(spr, p_pos.x_, p_pos.y_));*/
		}
						 break;
		default: {
			//Invalid type
		}
				 break;
		}
	}

	void client_app::createLevel() {
		int rows = std::extent<decltype(level), 0>::value; // Get the amount of rows
		for (size_t row = 0; row < rows; row++) {
			int column = 0;
			for (TILE_TYPE type : level[row]) {
				createTile(vector2((float)column * TILE_SIZE, (float)row * TILE_SIZE), type);
				column++;
			}
		}
	}

	void client_app::createTank(vector2 p_pos) {
		sprite spr(tankTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
		sprite turretSpr(turretTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
		entities_.push_back(new tank(spr, turretSpr, p_pos.x_, p_pos.y_));
	}

	void client_app::update(time dt) {
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->update(keyboard_, mouse_, dt);
			}
		}
	}

	void client_app::manageCollisions() {
		for (int i = 0; i < entities_.size(); i++) {
			for (int j = 0; j < entities_.size(); j++) {
				if ((j != i) && (checkCollision(entities_[i], entities_[j]))) {
					entities_[i]->onCollision(entities_[j]);
					entities_[j]->onCollision(entities_[i]);
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

	void client_app::render() {
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->render(render_system_);
			}
		}
	}



} // !tankett
