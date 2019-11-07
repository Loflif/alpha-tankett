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
		initializeUI();

		return true;
	}

	void client_app::initializeUI() {
		SetUIElement(timer_, "00:00", 2, vector2(21.2f * TILE_SIZE, 0.25f * TILE_SIZE));
		SetUIElement(coolDown_, "", 1, vector2(4 * TILE_SIZE, 4.5f * TILE_SIZE));
		SetUIElement(p1Eliminations, "P1: 0", 2, vector2(1 * TILE_SIZE, 1 * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p2Eliminations, "P2: 0", 2, vector2(40 * TILE_SIZE, 1 * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p3Eliminations, "P3: 0", 2, vector2(1 * TILE_SIZE, 31.4f * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p4Eliminations, "P4: 0", 2, vector2(40 * TILE_SIZE, 31.4f * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p1Ping, "0123456789", 1, vector2(1 * TILE_SIZE, 1.8f * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p2Ping, "0123456789", 1, vector2(40 * TILE_SIZE, 1.8f * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p3Ping, "0123456789", 1, vector2(1 * TILE_SIZE, 30.6f * TILE_SIZE), 0xFF4F5E9C);
		SetUIElement(p4Ping, "0123456789", 1, vector2(40 * TILE_SIZE, 30.6f * TILE_SIZE), 0xFF4F5E9C);
	}

	void client_app::SetUIElement(UIElement& element, const char* pText, int32 pSize, vector2 pPos, uint32 pColor) {
		element.text_.set_text(pText);
		element.text_.set_scale(pSize);
		element.transform_.set_position(pPos);
		element.text_.set_color(pColor);

	}

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
			SetCoolDownDisplay();
		}

		SetEliminationUI(0, p1Eliminations);
		SetEliminationUI(1, p2Eliminations);
		SetEliminationUI(2, p3Eliminations);
		SetEliminationUI(3, p4Eliminations);
		SetPingUI(0, p1Ping);
		SetPingUI(1, p2Ping);
		SetPingUI(2, p3Ping);
		SetPingUI(3, p4Ping);

		

		render();

		return true;
	}

	void client_app::SetEliminationUI(int pID, UIElement &ui) {
		string eliminationText;
		if (remoteClientData_[pID].connected_) {
			eliminationText = "P" + std::to_string(pID+1) + ": " + std::to_string(remoteClientData_[pID].eliminations_);			
		}
		else {
			eliminationText = "";
		}
		ui.text_.set_text(eliminationText.c_str());
	}

	void client_app::SetPingUI(int pID, UIElement& ui) {
		string pingText;
		if (remoteClientData_[pID].connected_) {
			pingText = std::to_string(remoteClientData_[pID].ping_);
		}
		else {
			pingText = "";
		}
		ui.text_.set_text(pingText.c_str());
	}

	void client_app::SetCoolDownDisplay() {
		string text;
		int coolDown = (int)(entityManager_->shootingCooldown_ *10);
		if (coolDown < 0) text = "";
		else text = std::to_string(coolDown);
		vector2 targetPosition = entityManager_->getTank(entityManager_->getLocalTankID())->transform_.position_;
		targetPosition += vector2(0 * TILE_SIZE, -1.7f * TILE_SIZE);
		coolDown_.transform_.set_position(targetPosition);
		coolDown_.text_.set_text(text.c_str());
	}

	void client_app::render() {
		render_system_.clear(0xff0e1528); //Background Color
		entityManager_->render(render_system_);
		renderUI(timer_);
		renderUI(coolDown_);
		renderUI(p1Eliminations);
		renderUI(p2Eliminations);
		renderUI(p3Eliminations);
		renderUI(p4Eliminations);
		renderUI(p1Ping);
		renderUI(p2Ping);
		renderUI(p3Ping);
		renderUI(p4Ping);
	}

	void client_app::renderUI(UIElement pUI) {
		render_system_.render(pUI.text_, pUI.transform_);
	}
#pragma endregion

#pragma region send

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
		uint8 buffer[1024] = {};
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
				if (entityManager_->getLocalTankID() == 255)
					entityManager_->setLocalTank(pMessage.receiver_id);
			}
			remoteClientData_[i].eliminations_ = pMessage.client_data[i].eliminations;
			remoteClientData_[i].ping_ = pMessage.client_data[i].ping;
			remoteClientData_[i].connected_ = pMessage.client_data[i].connected;
			entityManager_->UpdateTank(pMessage.client_data[i]);
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
		timer_.text_.set_text(timerText.c_str());
	}
#pragma endregion
} // !tankett