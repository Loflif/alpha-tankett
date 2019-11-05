#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverTank : public IServerEntity {
	public:
		serverTank(vector2 pSpawnPos, uint8 ID);
		~serverTank();
		void update(time dt);

		void onCollision(IServerEntity* collider) override;
		void SetPosition(vector2 pNewPosition);
		void Move(vector2 direction);

		vector2 getAimVector();
		
		dynamic_array<vector2> bulletPositions_;

		const float SPEED_ = 4.0f;
		float turretRotation_ = 0.0f;
		float shootingCooldown_ = 0.0f;
		uint8 id_;
		transform transform_;
	private:
		vector2 previousPosition_;
	};
}
