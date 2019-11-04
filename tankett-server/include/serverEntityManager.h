#pragma once
#include "IServerEntity.h"
#include "serverTank.h"
#include "serverTile.h"
#include "serverBullet.h"

namespace tankett {
	class serverEntityManager {
	public:
		serverEntityManager();
		~serverEntityManager();

		
	private:
		dynamic_array<IServerEntity*> entities_;
		serverTank* createTank();
		void createLevel();
		serverTile* createTile(vector2 pPosition);
		serverBullet* createServerBullet();

		serverTank* tanks_[4];
		serverBullet* bullets_[40];
		
	};
}
