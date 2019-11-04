// tankett_client.h

#ifndef TANKETT_CLIENT_H_INCLUDED
#define TANKETT_CLIENT_H_INCLUDED

#include <alpha.h>
#include <tankett_shared.h>
#include <iostream>
#include <string>
#include "tile.h"
#include "tank.h"

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

		dynamic_array<std::pair<ENTITY_TYPE, ENTITY_TYPE>> collisionPairs_;

		virtual bool enter() final;
		virtual void exit() final;
		virtual bool tick() final;
		void checkInput();
		void send(time dt);
		bool pack_payload(protocol_payload& pPayload);
		bool send_payload(protocol_payload& pPayload);

		void receive();
		void initializeUI();
		void SetUIElement(UIElement &element, const char* pText, int32 pSize, vector2 pPos, uint32 color = 0xffffffff);
		void createTile(vector2 pos);
		void createLevel();
		tank* createTank(vector2 p_pos, uint8 pID, bool pIsLocal);
		void createBulletBuffer();
		void manageCollisions();
		bool checkCollision(IEntity* firstEntity, IEntity* secondEntity);
		void fireBullet(tank* t);
		bool isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity);
		void parsePayload(protocol_payload pPayload);
		void parseServerMessage(message_server_to_client pMessage);
		void SetTimer(float pTime);
		void UpdateLocalTank(server_to_client_data pData);
		void UpdateRemoteTanks(server_to_client_data pData);
		void UpdateRemoteTank(server_to_client_data pData, uint8 pID);

		void update(time dt);
		void render();
		void renderUI(UIElement pUI);

		time current_;
		text debugText_;
		transform debugTextTransform_;

		texture wallTexture_;
		texture bulletTexture_;
		texture tankTexture_;
		texture turretTexture_;

		udp_socket socket_;
		ip_address server_ip_;
		ip_address send_all_ip_;

		uint64 client_key_;
		uint64 server_key_;

		CLIENT_STATE state_;

		time send_accumulator;

		uint32 send_sequence_ = 0;

		dynamic_array<IEntity*> entities_;
		const int BULLET_MAX = 10;
		dynamic_array<bullet*>bullets_;
		tank* playerTank_ = nullptr;
		dynamic_array<tank*>remoteTanks_;

		crypt::xorinator xorinator_;

		dynamic_array<network_message_header*> messages_;
		message_client_to_server currentMessage_;

		UIElement timer;

	};
} // !tankett

#endif // !TANKETT_CLIENT_H_INCLUDED