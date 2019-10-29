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
		void update(keyboard pKeyboard, mouse pMouse, time pDeltaTime);
		void onCollision(IEntity* pCollider);
		vector2 getAimVector(mouse pMouse);
	

		dynamic_array<bullet*> bullets_;
	private:
		vector2 targetMoveDirection(keyboard kb);
		float targetRotation(keyboard kb);
		float targetTurretRotation(mouse m);
		void setColliderPosition();
		void preventCollision();
		bool ownsBullet(IEntity* pBullet);

		const float SPEED_ = 4.0f;
		vector2 size_ = { 0,0 };
		sprite sprite_;
		sprite turretSprite_;
		transform transform_;
		transform turretTransform_;
		vector2 previousPosition;
	};

}
