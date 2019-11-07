#pragma once
#include "IEntity.h"
#include "bullet.h"

namespace tankett {

	class tank : public IEntity {
	public:
		tank(sprite pSprite, sprite pTurretSprite, sprite pRemoteSprite, sprite pRemoteTurretSprite, float pPosX, float pPosY, uint8 pID);
		~tank();

		void render(render_system& pRenderSystem);
		void update(keyboard pKeyboard, mouse pMouse, time pDeltaTime);
		void onCollision(IEntity* pCollider);
		vector2 aimVector_;
		transform transform_;

		float turretRotation = 0.0f;

		dynamic_array<bullet*> bullets_;

		//Client-specific (keep these when refactoring later):
		void UpdateValues(bool pAlive,
						  vector2 pPos,
						  float pAngle);
		void SetPosition(vector2 pPos);
		void SetAngle(float pAngle);
		void SetActive(bool pIsActive);
		void UpdatePosition(keyboard kb, time dt);
		void SetLocal(bool pIsLocal);
		//Also keep Constructor/Destructor, Render and Update, and only set aimvector when local

		uint8 id_;

		int getUnusedBulletID();
		bool hasBulletWithID(uint8 pID);
		bullet* getBulletWithID(uint8 pID);
		bool isLocal_ = false;
	private:
		void SetTurret(mouse ms);
		void interpolateEntity(time dt);
		vector2 targetMoveDirection(keyboard pKeyboard);
		float targetRotation(keyboard pKeyboard);
		float targetTurretRotation(mouse pMouse);
		void setColliderPosition();
		void preventCollision();
		bool ownsBullet(IEntity* pBullet);
		void updateAimVector(mouse pMouse);
		void updateBulletList();

		const float SPEED_ = 4.0f;
		vector2 size_ = { 0,0 };
		sprite sprite_;
		sprite turretSprite_;
		sprite remoteSprite_;
		sprite remoteTurretSprite_;
		transform turretTransform_;
		vector2 previousPosition;
		

		//For Prediciton:
		void PredictPosition(vector2 pReceivedPos);
		dynamic_array<vector2> predictedPositionOffsets_;
		float lastPredictedAngle;
		vector2 lastReceivedPosition_;
		float lastReceivedAngle_;
		//For interpolation:
		vector2 nextToLastReceivedPosition_;
		float nextToLastReceivedAngle_;
		time timeOfLastMessage;
		time messageDeltaTime_;
	};

}
