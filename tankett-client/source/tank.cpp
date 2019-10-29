#include <tank.h>

namespace alpha {

	tank::tank(sprite pSprite, sprite pTurretSprite, float pPosX, float pPosY) {
		sprite_ = pSprite;
		size_ = pSprite.size_;
		turretSprite_ = pTurretSprite;
		transform_.set_origin(vector2(size_ / 2));
		transform_.position_.x_ = pPosX;
		transform_.position_.y_ = pPosY;
		turretTransform_ = transform_;
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		setColliderPosition();
		type_ = TANK;
	}

	tank::~tank() {
	}

	void tank::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
		pRenderSystem.render(turretSprite_, turretTransform_);

		if (bullet_) {
			bullet_->render(pRenderSystem);
		}
	}

	void tank::update(keyboard kb, mouse ms, time dt) {
		previousPosition = transform_.position_;
		vector2 direction = getTargetMoveDirection(kb);
		transform_.set_rotation(getTargetRotation(kb));
		turretTransform_.set_rotation(getTargetTurretRotation(ms));

		transform_.position_.x_ += direction.x_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		transform_.position_.y_ += direction.y_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		turretTransform_.position_ = transform_.position_;

		if (ms.is_pressed(MOUSE_BUTTON_LEFT)) {
			bullet_ = new bullet(sprite_, transform_.position_.x_, transform_.position_.y_, getAimVector(ms));
		}

		setColliderPosition();
		if (bullet_) {
			bullet_->update(kb, ms, dt);
		}
	}

	void tank::setColliderPosition() {
		vector2 targetPosition = transform_.position_ - size_ / 2;
		collider_.set_position(targetPosition);
	}

	void tank::onCollision(IEntity* collider) {
		transform_.position_ = previousPosition;
		turretTransform_.position_ = transform_.position_;
		collider_.set_position(previousPosition);
	}

	vector2 tank::getTargetMoveDirection(keyboard kb) {
		if (kb.is_down(KEYCODE_W) && kb.is_down(KEYCODE_D)) return { 0.7071f ,-0.7071f };  //Normalised Diagonal Vector
		if (kb.is_down(KEYCODE_W) && kb.is_down(KEYCODE_A)) return { -0.7071f ,-0.7071f };
		if (kb.is_down(KEYCODE_S) && kb.is_down(KEYCODE_A)) return { -0.7071f ,0.7071f };
		if (kb.is_down(KEYCODE_S) && kb.is_down(KEYCODE_D)) return { 0.7071f ,0.7071f };
		if (kb.is_down(KEYCODE_D))  return { 1.0f ,0 };
		if (kb.is_down(KEYCODE_A))  return { -1.0f,0 };
		if (kb.is_down(KEYCODE_W))	return { 0,-1.0f };
		if (kb.is_down(KEYCODE_S))	return { 0, 1.0f };
		return { 0,0 };
	}

	float tank::getTargetRotation(keyboard kb) {
		if (kb.is_down(KEYCODE_W) && kb.is_down(KEYCODE_D)) return 315;
		if (kb.is_down(KEYCODE_W) && kb.is_down(KEYCODE_A)) return 225;
		if (kb.is_down(KEYCODE_S) && kb.is_down(KEYCODE_A)) return 135;
		if (kb.is_down(KEYCODE_S) && kb.is_down(KEYCODE_D)) return 45;
		if (kb.is_down(KEYCODE_D))	return 0;
		if (kb.is_down(KEYCODE_A))  return 180;
		if (kb.is_down(KEYCODE_W))	return 270;
		if (kb.is_down(KEYCODE_S))	return 90;
		return transform_.rotation_;
	}

	float tank::getTargetTurretRotation(mouse ms) {
		vector2 aimVector = getAimVector(ms);
		return atan2(aimVector.y_, aimVector.x_) * (180 / PI);
	}

	vector2 tank::getAimVector(mouse ms) {
		vector2 mousePosition((float)ms.x_, (float)ms.y_);
		return vector2(mousePosition - turretTransform_.position_).normalized();
	}
}