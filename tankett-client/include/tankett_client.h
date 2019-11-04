// tankett_client.h

#ifndef TANKETT_CLIENT_H_INCLUDED
#define TANKETT_CLIENT_H_INCLUDED

#include <alpha.h>
#include <tankett_shared.h>
#include "tile.h"
#include "tank.h"
#include "entityManager.h"

using namespace alpha;

namespace tankett {
	enum CLIENT_STATE {
		INIT,
		CONNECTION_REQUEST,
		CHALLENGE_RESPONSE,
		CONNECTED,
		DISCONNECTED
	};

	struct client_app : application {
		client_app();

		virtual bool enter() final;
		virtual void exit() final;
		virtual bool tick() final;
		void send(time dt);
		bool pack_payload(protocol_payload& pPayload);
		bool send_payload(protocol_payload& pPayload);

		void receive();
		void manageCollisions();
		bool checkCollision(IEntity* firstEntity, IEntity* secondEntity);
		void fireBullet(tank* t);
		bool isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity);
		void parsePayload(protocol_payload pPayload);
		void parseServerMessage(message_server_to_client pMessage);

		time current_;
		text text_;
		transform transform_;

		udp_socket socket_;
		ip_address server_ip_;
		ip_address send_all_ip_;

		uint64 client_key_;
		uint64 server_key_;

		CLIENT_STATE state_;

		time send_accumulator;

		uint32 send_sequence_ = 0;

		crypt::xorinator xorinator_;
		entityManager* entityManager_;
		
		dynamic_array<network_message_header*> messages_;
		message_client_to_server currentMessage_;
	};
} // !tankett

#endif // !TANKETT_CLIENT_H_INCLUDED