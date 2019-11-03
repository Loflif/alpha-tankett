#pragma once
#include "IEntity.h"
namespace tankett {
	class tile : public IEntity {
	public:
		tile(sprite pSprite, float pPosx, float pPosY);
		~tile();

		void render(render_system& pRenderSystem);
		void update(keyboard kb, mouse ms, time dt) {};
		void onCollision(IEntity* collider) {};
	private:
		sprite sprite_;
		transform transform_;
	};
}