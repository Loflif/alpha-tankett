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
		serverTank& getTank(int ID);

		void update(time dt);
		
		void manageCollisions();
		void UpdateTank(uint8 pTankID);
	private:
		void createLevel();
		void createTankBuffer();
		void createBulletBuffer();

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
