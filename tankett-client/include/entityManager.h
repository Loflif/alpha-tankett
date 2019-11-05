#pragma once
#include "IEntity.h"
#include "bullet.h"
#include "tank.h"
#include "tile.h"

namespace tankett {
	class entityManager {
	public:
		entityManager();
		~entityManager();

		void addEntity(IEntity& pEntity);
		tank& getTank(int ID);
		void setLocalTank(uint8 ID);
		uint8 getLocalTank();
		void update(keyboard pKeyboard, mouse pMouse, time dt);
		void render(render_system& pRenderSystem);
		message_client_to_server* checkInput(keyboard pKeyboard, mouse pMouse);
		
		void manageCollisions();
		void UpdateLocalTank(server_to_client_data pData);
		void UpdateRemoteTank(server_to_client_data pData);
	private:
		void createLevel();
		void createTankBuffer();
		void createBulletBuffer();

		bool checkCollision(IEntity* firstEntity, IEntity* secondEntity);
		bool isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity);

		void fireBullet(tank* t);

		uint8 localTankID_ = 255;
		tank* tanks_[4];
		bullet* bullets_[BULLET_MAX_COUNT];
		dynamic_array<IEntity*> entities_;
		const std::pair<ENTITY_TYPE, ENTITY_TYPE> collisionPairs_[COLLISION_PAIR_COUNT] {
			std::make_pair(TANK, WALL),
			std::make_pair(TANK, BULLET),
			std::make_pair(TANK, TANK),
			std::make_pair(WALL, BULLET)
		};
		texture wallTexture_;
		texture bulletTexture_;
		texture tankTexture_;
		texture turretTexture_;
	};
}