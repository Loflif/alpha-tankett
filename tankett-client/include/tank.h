#pragma once
#include <alpha.h>
#include "IEntity.h"
#include "bullet.h"

namespace alpha {

	class tank : public IEntity {
	public:
		tank(sprite pSprite, sprite pTurretSprite, float pPosX, float pPosY);
		~tank();

		void render(render_system& pRenderSystem);
		void update(keyboard kb, mouse ms, time dt);
		void onCollision(IEntity* collider);
	
	private:
		vector2 getTargetMoveDirection(keyboard kb);
		float getTargetRotation(keyboard kb);
		float getTargetTurretRotation(mouse m);
		void setColliderPosition();
		vector2 getAimVector(mouse ms);
		void preventCollision();

		const float SPEED_ = 4.0f;
		vector2 size_ = { 0,0 };
		sprite sprite_;
		sprite turretSprite_;
		transform transform_;
		transform turretTransform_;
		vector2 previousPosition;
	};

}
