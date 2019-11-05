#include <tank.h>

namespace tankett {
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
		timeOfLastMessage = time::now();
		isEnabled = false;
	}

	tank::~tank() {
	}

	void tank::render(render_system& pRenderSystem) {
		pRenderSystem.render(sprite_, transform_);
		pRenderSystem.render(turretSprite_, turretTransform_);
	}

	void tank::update(keyboard kb, mouse ms, time dt) {
		previousPosition = transform_.position_;
		if (isLocal_)UpdatePosition(kb, dt);
		else interpolateEntity(dt);

		updateAimVector(ms);
		//transform_.set_rotation(targetRotation(kb));
		SetTurret(ms);
		setColliderPosition();
		updateBulletList();
	}



	int tank::getUnusedBulletID() {
		for (uint8 i = 0; i < 10; i++) {
			if (!hasBulletWithID(i)) return i;
		}
		return -1;
	}

	bool tank::hasBulletWithID(uint8 pID) {
		for (bullet* b : bullets_) {
			if (b->id_ == pID) return true;
		}
		return false;
	}

	void tank::SetTurret(mouse ms) {
		turretTransform_.set_rotation(targetTurretRotation(ms));
		turretTransform_.position_ = transform_.position_;
	}

	void tank::interpolateEntity(time dt) {
		SetPosition(vector2::Lerp(transform_.position_, lastReceivedPosition_, dt.as_seconds() / messageDeltaTime_.as_seconds()));
	}

	void tank::setColliderPosition() {
		vector2 targetPosition = transform_.position_ - size_ / 2;
		collider_.set_position(targetPosition);
	}

	void tank::updateBulletList() {
		dynamic_array<bullet*> newBullets;
		for (int i = 0; i < bullets_.size(); i++) {
			if (bullets_[i]->isEnabled) {
				newBullets.push_back(bullets_[i]);
			}
		}
		bullets_ = newBullets;
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

	void tank::UpdatePosition(keyboard kb, time dt) {
		vector2 newOffset = targetMoveDirection(kb) * (SPEED_ * TILE_SIZE *dt.as_seconds());
		predictedPositionOffsets_.push_back(newOffset);

		vector2 destination = lastReceivedPosition_;
		for (vector2 offset : predictedPositionOffsets_) {
			destination += offset;
		}

		SetPosition(vector2::Lerp(transform_.position_, destination, 0.9f));
	}

	void tank::SetLocal(bool pIsLocal) {
		isLocal_ = pIsLocal;
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
		vector2 aimVector = aimVector_;
		turretRotation = atan2(aimVector.y_, aimVector.x_) * (180 / PI);
		return turretRotation;
	}

	void tank::updateAimVector(mouse ms) {
		vector2 mousePosition((float)ms.x_, (float)ms.y_);
		aimVector_ = vector2(mousePosition - turretTransform_.position_).normalized();
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
		SetAngle(pAngle);
		SetActive(pAlive);
		

		//Input Prediction:
		if (isLocal_) {
			vector2 lastPrediction = vector2::zero();
			if (predictedPositionOffsets_.size() > 0) {
				lastPrediction = predictedPositionOffsets_[predictedPositionOffsets_.size() - 1];
			}
			lastReceivedPosition_ = pPos + lastPrediction;
			//SetPosition(vector2::Lerp(transform_.position_, lastReceivedPosition_, 0.9f));
			predictedPositionOffsets_.clear();
		}
		//Entity Interpolation:
		else {
			nextToLastReceivedPosition_ = lastReceivedPosition_;
			lastReceivedPosition_ = pPos;
			SetPosition(nextToLastReceivedPosition_);
			messageDeltaTime_ = time::now() - timeOfLastMessage;
			timeOfLastMessage = time::now();
		}
	}

	void tank::SetPosition(vector2 pPos) {
		transform_.set_position(pPos);
		turretTransform_.position_ = transform_.position_;
		setColliderPosition();
	}

	void tank::PredictPosition(vector2 pReceivedPos) {

	}

	void tank::SetAngle(float pAngle) {
		transform_.set_rotation(pAngle);
	}

	void tank::SetActive(bool pIsActive) {
		isEnabled = pIsActive;
	}

#pragma endregion
}
