#pragma once
#include "IEntity.h"
namespace tankett {

	class bullet : public IEntity {
	public:
		bullet(sprite pSprite);
		~bullet();

		void update(keyboard kb, mouse ms, time dt);
		void render(render_system& pRenderSystem);
		void fire(float pPosX, float pPosY, vector2 pDirection, int pID);
		void onCollision(IEntity* collider);

		int id_;
		void UpdateData(vector2 pPos);
	private:
		void interpolateEntity(time dt);
		vector2 direction_;
		vector2 nextToLastReceivedPosition_;
		vector2 lastReceivedPosition_;
		time timeOfLastMessage;
		time messageDeltaTime_;
	};
}