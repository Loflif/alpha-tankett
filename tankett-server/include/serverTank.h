#pragma once
#include "IServerEntity.h"
#include "serverBullet.h"

namespace tankett {
	class serverTank : public IServerEntity {
	public:
		serverTank(vector2 pSpawnPos, uint8 ID);
		~serverTank();
		void update(time dt);

		void onCollision(IServerEntity* collider) override;
		void SetPreviousPosition();
		void SetPosition(vector2 pNewPosition);

		vector2 getAimVector();

		float turretRotation_ = 0.0f;
		float shootingCooldown_ = 0.0f;
		int id_;
		dynamic_array<serverBullet*> bullets_;
		int getUnusedBulletID();
		bool isAlive;
		time respawnTime;
	private:
		bool hasBulletWithID(uint8 pID);
		void updateBulletList();
		bool ownsBullet(IServerEntity* pBullet);
		vector2 previousPosition_;
	};
}
