#pragma once
#include "IServerEntity.h"

namespace tankett {
	class serverEntityManager {
	public:
		serverEntityManager();
		~serverEntityManager();

		private:
		dynamic_array<IServerEntity*> entities_;
		
	};
}