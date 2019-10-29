#pragma once
#include <alpha.h>
#include "IEntity.h"
namespace alpha {

	class bullet : public IEntity {
	public:
		bullet(sprite pSprite);
		~bullet();

		void update(keyboard kb, mouse ms, time dt);
		void render(render_system& pRenderSystem);
		void fire(float pPosX, float pPosY, vector2 pDirection);
		void onCollision(IEntity* collider);

	private:
		vector2 size_ = { 0,0 };
		sprite sprite_;
		transform transform_;
		vector2 direction_;
		const float SPEED_ = 8.0f;
	};
}