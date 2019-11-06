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
		interpolateEntity(dt);
		collider_.set_position(transform_.position_);
	}

	void bullet::interpolateEntity(time dt) {
		transform_.set_position(vector2::Lerp(transform_.position_, lastReceivedPosition_, dt.as_seconds() / messageDeltaTime_.as_seconds()));
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
		nextToLastReceivedPosition_ = transform_.position_;
		lastReceivedPosition_ = transform_.position_;
	}

	void bullet::onCollision(IEntity* collider) {
	}

	void bullet::UpdateData(vector2 pPos) {
		nextToLastReceivedPosition_ = lastReceivedPosition_;
		lastReceivedPosition_ = pPos;
		transform_.set_position(nextToLastReceivedPosition_);
		messageDeltaTime_ = time::now() - timeOfLastMessage;
		timeOfLastMessage = time::now();
	}
}