#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverTank : public IServerEntity {
	public:
		serverTank(vector2 pSpawnPos, uint8 pID);
		~serverTank();

		void onCollision(IServerEntity* collider) override;
		void SetPosition(vector2 pNewPosition);
		
		dynamic_array<vector2> bulletPositions_;

		const float SPEED_ = 4.0f;
		float turretRotation_ = 0.0f;
		uint8 ID_;
	private:
		vector2 previousPosition_;
	};
}