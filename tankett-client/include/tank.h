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
		vector2 aimVector_;
		transform transform_;

		const float FIRE_RATE_ = 1.0f;
		float shootingCooldown_ = 0.0f;
		
		dynamic_array<bullet*> bullets_;

		//Client-specific:
		void SetPosition(vector2 pPos);

	private:
		vector2 targetMoveDirection(keyboard pKeyboard);
		float targetRotation(keyboard pKeyboard);
		float targetTurretRotation(mouse pMouse);
		void setColliderPosition();
		void preventCollision();
		bool ownsBullet(IEntity* pBullet);
		vector2 getAimVector(mouse pMouse);

		const float SPEED_ = 4.0f;
		vector2 size_ = { 0,0 };
		sprite sprite_;
		sprite turretSprite_;
		transform turretTransform_;
		vector2 previousPosition;
	};

}
