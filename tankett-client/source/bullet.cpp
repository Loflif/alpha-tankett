#include <bullet.h>
namespace alpha {

	bullet::bullet(sprite pSprite, float pPosX, float pPosY, vector2 pDirection) {
		sprite_ = pSprite;
		size_ = pSprite.size_;
		transform_.set_origin(vector2(size_ / 2));
		transform_.position_.x_ = pPosX;
		transform_.position_.y_ = pPosY;
		collider_ = rectangle(pPosX, pPosY, pSprite.size_.x_, pSprite.size_.y_);
		direction_ = pDirection;
		type_ = BULLET;
	}

	bullet::~bullet() {

	}

	void bullet::update(keyboard kb, mouse ms, time dt) {
		transform_.position_.x_ += direction_.x_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		transform_.position_.y_ += direction_.y_ * SPEED_ * TILE_SIZE * dt.as_seconds();
	}

	void bullet::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
	}

	void bullet::onCollision(IEntity* collider) {
		switch (collider->type_) {
		case WALL: {
			isEnabled = false;
		}
				   break;
				   /*case TANK: {
					   isEnabled = false;
				   }
							  break;
				   case BULLET: {
					   isEnabled = false;
				   }
							  break;*/
		default:
			break;
		}
	}

}