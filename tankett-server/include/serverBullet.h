#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverBullet : public IServerEntity {
	public:
		serverBullet();
		~serverBullet();
		void update(time dt) {};

		void onCollision(IServerEntity* collider) override;
		void SetPosition(vector2 pNewPosition);
		
		const float SPEED_ = 8.0f;
	private:
	};

}
