#include <tile.h>
namespace tankett {

	tile::tile(sprite pSprite, float pPosX, float pPosY) {
		sprite_ = pSprite;
		transform_.position_.x_ = pPosX;
		transform_.position_.y_ = pPosY;
		collider_ = rectangle(pPosX, pPosY, pSprite.size_.x_, pSprite.size_.y_);
		type_ = WALL;
		isEnabled = true;
	}

	tile::~tile() {
	}

	void tile::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
	}

}