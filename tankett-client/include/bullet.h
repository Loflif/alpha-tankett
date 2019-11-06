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
		transform transform_;
		void SetPosition(vector2 pPos);
	private:		
		vector2 size_ = { 0,0 };
		sprite sprite_;		
		vector2 direction_;
		const float SPEED_ = 8.0f;
		vector2 previousPosition;
	};
}