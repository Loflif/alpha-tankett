#include <bullet.h>
namespace tankett {
	bullet::bullet(sprite pSprite) {
		sprite_ = pSprite;
		size_ = pSprite.size_;
		transform_.set_origin(vector2(size_ / 2));
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		isEnabled = false;
		type_ = BULLET;
		id_ = 255;
	}

	bullet::~bullet() {

	}

	void bullet::update(keyboard kb, mouse ms, time dt) {
		/*transform_.position_.x_ += direction_.x_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		transform_.position_.y_ += direction_.y_ * SPEED_ * TILE_SIZE * dt.as_seconds();*/
		collider_.set_position(transform_.position_);
	}

	void bullet::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
	}

	void bullet::fire(float pPosX, float pPosY, vector2 pDirection, int pID) {
		transform_.position_.x_ = pPosX;
		transform_.position_.y_ = pPosY;
		collider_.set_position(transform_.position_);
		direction_ = pDirection.normalized();
		isEnabled = true;
		id_ = pID;
	}

	void bullet::onCollision(IEntity* collider) {
		if (collider->type_ == WALL) {
			isEnabled = false;
			id_ = 255;
		}
	}

	void bullet::SetPosition(vector2 pPos) {
		previousPosition = transform_.position_;
		transform_.position_ = pPos;
		collider_.set_position(pPos);

	}
}