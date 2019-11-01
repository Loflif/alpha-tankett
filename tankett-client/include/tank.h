#pragma once
#include <alpha.h>
#include "IEntity.h"
#include "bullet.h"

namespace alpha {

	class tank : public IEntity {
	public:
		tank(sprite pSprite, sprite pTurretSprite, float pPosX, float pPosY, uint8 pID);
		~tank();

		void render(render_system& pRenderSystem);
		void update(keyboard pKeyboard, mouse pMouse, time pDeltaTime);
		void onCollision(IEntity* pCollider);
		vector2 aimVector_;
		transform transform_;

		const float FIRE_RATE_ = 1.0f;
		float shootingCooldown_ = 0.0f;
		float turretRotation = 0.0f;

		dynamic_array<bullet*> bullets_;

		//Client-specific (keep these when refactoring later):
		void UpdateValues(bool pAlive,
						  vector2 pPos,
						  float pAngle,
						  dynamic_array<vector2> bullets);
		void SetPosition(vector2 pPos);
		void SetAngle(float pAngle);
		void SetActive(bool pIsActive);
		//Also keep Constructor/Destructor, Render and Update, and only set aimvector when local

		dynamic_array<vector2> bulletPositions_;

		uint8 id_;

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
