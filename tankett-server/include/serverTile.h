#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverTile : public IServerEntity {
	public:
		serverTile(vector2 pSpawnPos);
		~serverTile();

		void onCollision(IServerEntity* collider) override;
	};
}