#pragma once
#include <alpha.h>
#include "tankett_shared.h"

namespace tankett {
	class IEntity {
	public:
		virtual ~IEntity() {};
		virtual void render(render_system& pRenderSystem) = 0;
		virtual void update(keyboard kb, mouse ms, time dt) = 0;
		virtual void onCollision(IEntity* collider) = 0;
		
		rectangle collider_ = {0,0,0,0};
		ENTITY_TYPE type_ = ENTITY_TYPE::TANK;
		bool isEnabled = true;
	};
}
