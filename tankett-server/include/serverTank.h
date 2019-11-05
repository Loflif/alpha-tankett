#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverTank : public IServerEntity {
	public:
		serverTank(vector2 pSpawnPos);
		~serverTank();

		void onCollision(IServerEntity* collider) override;
		void SetPosition(vector2 pNewPosition);
		void Move(vector2 direction);

		dynamic_array<vector2> bulletPositions_;

		const float SPEED_ = 4.0f;
		float turretRotation_ = 0.0f;
	private:
		transform transform_;
		vector2 previousPosition_;
	};
}
