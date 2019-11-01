#include <tank.h>

namespace alpha {

	tank::tank(sprite pSprite, sprite pTurretSprite, float pPosX, float pPosY, uint8 pID) {
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
		id_ = pID;
	}

	tank::~tank() {
	}

	void tank::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
		pRenderSystem.render(turretSprite_, turretTransform_);
	}

	void tank::update(keyboard kb, mouse ms, time dt) {
		shootingCooldown_ -= dt.as_seconds();
		
		previousPosition = transform_.position_;
		vector2 direction = targetMoveDirection(kb);
		transform_.set_rotation(targetRotation(kb));
		turretTransform_.set_rotation(targetTurretRotation(ms));

		transform_.position_.x_ += direction.x_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		transform_.position_.y_ += direction.y_ * SPEED_ * TILE_SIZE * dt.as_seconds();
		turretTransform_.position_ = transform_.position_;
		setColliderPosition();
	}

	void tank::setColliderPosition() {
		vector2 targetPosition = transform_.position_ - size_ / 2;
		collider_.set_position(targetPosition);
	}

	void tank::onCollision(IEntity* collider) {
		switch (collider->type_) {
		case WALL: {
			preventCollision();
		}
		break;
		case BULLET: {
			if (!ownsBullet(collider)) {
				isEnabled = false;
			}
		}
		break;
		case TANK: {
			isEnabled = false;
		}
		break;
		default:
			break;
		}
		
	}

	vector2 tank::targetMoveDirection(keyboard kb) {
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

	float tank::targetRotation(keyboard kb) {
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

	float tank::targetTurretRotation(mouse ms) {
		vector2 aimVector = getAimVector(ms);
		return atan2(aimVector.y_, aimVector.x_) * (180 / PI);
	}

	vector2 tank::getAimVector(mouse ms) {
		vector2 mousePosition((float)ms.x_, (float)ms.y_);
		aimVector_ = vector2(mousePosition - turretTransform_.position_).normalized();
		return aimVector_;
	}
	void tank::preventCollision() {
		transform_.position_ = previousPosition;
		turretTransform_.position_ = transform_.position_;
		collider_.set_position(previousPosition);
	}

	bool tank::ownsBullet(IEntity* pBullet) {
		for (IEntity* b : bullets_) {
			if (pBullet == b) return true;
		}
		return false;
	}

#pragma region ClientSpecificFunctions

	void tank::UpdateValues(bool pAlive,
							vector2 pPos, 
							float pAngle,
							dynamic_array<vector2> bullets){
		SetPosition(pPos);
		SetAngle(pAngle);
		SetActive(pAlive);
	}

	void tank::SetPosition(vector2 pPos) {
		transform_.set_position(pPos);
		turretTransform_.position_ = transform_.position_;
		setColliderPosition();
	}

	void tank::SetAngle(float pAngle) {
		transform_.set_rotation(pAngle);
	}

	void tank::SetActive(bool pIsActive) {
		isEnabled = pIsActive;
	}

#pragma endregion
}