#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverBullet : public IServerEntity {
	public:
		serverBullet();
		~serverBullet();

		void onCollision(IServerEntity* collider) override;
		void update(time dt) override;
		void SetPosition(vector2 pNewPosition);

		void fire(vector2 pSpawnPos, vector2 pDirection);
	private:
		vector2 direction_;
		const float SPEED_ = 8.0f;
	};

}
