#pragma once
#include "IServerEntity.h"
#include "serverTank.h"
#include "serverTile.h"
#include "serverBullet.h"

namespace tankett {
	class serverEntityManager {
	public:
		serverEntityManager();
		~serverEntityManager();

		void addEntity(IServerEntity& pEntity);
		serverTank* getTank(int ID);
		void spawnTank(int id);

		void update(time dt);
		void respawnTanks(time dt);
		void parseClientMessage(message_client_to_server message,uint8 clientID, const time& pDeltaRecieveTime);
		
		void manageCollisions();
	private:
		void createLevel();
		void createTankBuffer();
		void createBulletBuffer();
		vector2 targetMoveDirection(message_client_to_server message);

		bool checkCollision(IServerEntity* firstEntity, IServerEntity* secondEntity);
		bool isCollisionPair(IServerEntity* pFirstEntity, IServerEntity* pSecondEntity);

		void fireBullet(serverTank* t);
		
		serverTank* tanks_[4];
		serverBullet* bullets_[BULLET_MAX_COUNT * 4];
		
		dynamic_array<IServerEntity*> entities_;

		const std::pair<ENTITY_TYPE, ENTITY_TYPE> collisionPairs_[COLLISION_PAIR_COUNT]{
			std::make_pair(TANK, WALL),
			std::make_pair(TANK, BULLET),
			std::make_pair(TANK, TANK),
			std::make_pair(WALL, BULLET)
		};
	};
}
