#pragma once
#include <alpha.h>
#include "tankett_shared.h"

namespace tankett {
	class IServerEntity {
	public:
		virtual ~IServerEntity() {};
		virtual void update(time dt) = 0;
		virtual void onCollision(IServerEntity* collider) = 0;

		rectangle collider_ = { 0,0,0,0 };
		ENTITY_TYPE type_;
		transform transform_;
		vector2 size_ = { 0.0f, 0.0f };
		bool isEnabled = false;
	};
}