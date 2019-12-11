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

		float turretRotation = 0.0f;

		dynamic_array<bullet*> bullets_;

		void UpdateRemoteTank(bool pAlive,
			vector2 pPos,
			float pAngle,
			float pTimeStamp);
		void UpdateLocalTank(bool pAlive,
							 vector2 pPos,
							 uint32 pInputNumber);
		
		void SetPosition(vector2 pPos);
		void SetTurretAngle(float pAngle);
		void SetActive(bool pIsActive);
		void UpdatePosition(keyboard kb, time dt);
		void SetLocal(bool pIsLocal);
		void AddSentMessage(message_client_to_server *pMessage, time dt);

		uint8 id_;

		int getUnusedBulletID();
		bool hasBulletWithID(uint8 pID);
		bullet* getBulletWithID(uint8 pID);
		bool isLocal_ = false;
	private:
		void interpolateEntity();
		vector2 targetMoveDirection(message_client_to_server* pMessage);
		float targetRotation(keyboard kb);
		float targetTurretRotation();
		void setColliderPosition();
		void preventCollision();
		bool ownsBullet(IEntity* pBullet);
		void updateAimVector(mouse ms);
		void updateBulletList();

		sprite turretSprite_;
		sprite remoteSprite_;
		sprite remoteTurretSprite_;
		transform turretTransform_;
		vector2 previousPosition;

		//For Prediciton:
		struct InputData {
			message_client_to_server* message_;
			time dt_;
		};

		struct RemoteTankData {
			vector2 position_;
			float angle_;
			float timestamp_;
		};

		dynamic_array<RemoteTankData> entityHistory_;

		dynamic_array<InputData> sentInputData_;

		static bool compareTimeStamps(RemoteTankData data1, RemoteTankData data2);
	};

}
