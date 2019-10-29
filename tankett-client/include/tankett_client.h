// tankett_client.h

#ifndef TANKETT_CLIENT_H_INCLUDED
#define TANKETT_CLIENT_H_INCLUDED

#include <alpha.h>
#include <tankett_shared.h>
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

	enum TILE_TYPE {
		W,
		E
	};

	struct client_app : application {
		client_app();

		dynamic_array<std::pair<ENTITY_TYPE, ENTITY_TYPE>> collisionPairs_;

		virtual bool enter() final;
		virtual void exit() final;
		virtual bool tick() final;
		void checkInput();
		void send(time dt);
		void receive();
		void createTile(vector2 pos, TILE_TYPE type);
		void createLevel();
		tank* createTank(vector2 p_pos);
		void createBulletBuffer();
		void manageCollisions();
		bool checkCollision(IEntity* firstEntity, IEntity* secondEntity);
		void fireBullet(tank* t);
		bool isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity);

		void update(time dt);
		void render();

		time current_;
		text text_;
		transform transform_;

		texture wallTexture_;
		texture bulletTexture_;
		texture tankTexture_;
		texture turretTexture_;
		
		udp_socket sock_;
		ip_address server_ip_;
		ip_address send_all_ip_;

		uint64 client_key_;
		uint64 server_key_;

		CLIENT_STATE state_;

		time send_accumulator;

		dynamic_array<IEntity*> entities_;
		dynamic_array<bullet*>bullets_;
		tank* playerTank_;
		
		TILE_TYPE level[33][44] {
			{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}, //1
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //2
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //3
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //4
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //5
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //6
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //7
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //8
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,W,W,W,W,W,W,E,E,E,W,W,W,W,W,W,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //9
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //10
			{W,E,E,E,E,E,W,W,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,W,W,E,E,E,E,E,W}, //11
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //12
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //13
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //14
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //15
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //16
			{W,W,W,W,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,W,W,W,W}, //17
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //18
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //19
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //20
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //21
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //22
			{W,E,E,E,E,E,W,W,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,W,W,E,E,E,E,E,W}, //23
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //24
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,W,W,W,W,W,W,E,E,E,W,W,W,W,W,W,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //25
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //26
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //27
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //28
			{W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //29
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //30
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //31
			{W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //32
			{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}, //33
		};
	};
} // !tankett

#endif // !TANKETT_CLIENT_H_INCLUDED
