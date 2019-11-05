#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverBullet : public IServerEntity {
	public:
		serverBullet();
		~serverBullet();
		void update(time dt);

		void onCollision(IServerEntity* collider) override;
		void SetPosition(vector2 pNewPosition);

		void fire(vector2 pSpawnPos, vector2 pDirection, uint8 pID);
	private:
		vector2 direction_;
		uint8 id_ = 255;
		const float SPEED_ = 8.0f;
	};

}
