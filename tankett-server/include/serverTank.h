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
		void SetPosition(vector2 pNewPosition);

		vector2 getAimVector();

		const float SPEED_ = 4.0f;
		float turretRotation_ = 0.0f;
		float shootingCooldown_ = 0.0f;
		int id_;
		transform transform_;
		dynamic_array<serverBullet*> bullets_;
		int getUnusedBulletID();
	private:
		bool hasBulletWithID(uint8 pID);
		void updateBulletList();
		vector2 previousPosition_;
	};
}
