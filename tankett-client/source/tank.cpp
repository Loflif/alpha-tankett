#include <tank.h>
#include <algorithm>

namespace tankett {
	tank::tank(sprite pSprite, sprite pTurretSprite, sprite pRemoteSprite, sprite pRemoteTurretSprite, float pPosX, float pPosY, uint8 pID) {
		sprite_ = pSprite;
		remoteSprite_ = pRemoteSprite;
		remoteTurretSprite_ = pRemoteTurretSprite;
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
		isEnabled = false;
	}

	tank::~tank() {
		for(InputData &i : sentInputData_)
		{
			delete(i.message_);
		}
	}

	void tank::render(render_system& pRenderSystem) {
		if (isLocal_) {
			pRenderSystem.render(sprite_, transform_);
			pRenderSystem.render(turretSprite_, turretTransform_);
		}
		else {
			pRenderSystem.render(remoteSprite_, transform_);
			pRenderSystem.render(remoteTurretSprite_, turretTransform_);
		}

	}

	void tank::update(keyboard kb, mouse ms, time dt) {
		previousPosition = transform_.position_;
		if (isLocal_) {
			transform_.set_rotation(targetRotation(kb));
			SetTurretAngle(lerp(turretTransform_.rotation_, targetTurretRotation(), 0.9f));
		}
		else {
			interpolateEntity();
		}



		updateAimVector(ms);
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

	bullet* tank::getBulletWithID(uint8 pID) {
		for (bullet* b : bullets_) {
			if (b->id_ == pID) return b;
		}
		return nullptr;
	}

	void tank::setColliderPosition() {
		vector2 targetPosition = transform_.position_ - size_ / 2;
		collider_.set_position(targetPosition);
	}

	void tank::updateBulletList() {
		dynamic_array<bullet*> newBullets;
		for (int i = 0; i < bullets_.size(); i++) {
			if (bullets_[i]->isEnabled) {
				if (bullets_[i]->id_ != 255) newBullets.push_back(bullets_[i]);
				else bullets_[i]->isEnabled = false;
			}
		}
		bullets_ = newBullets;
	}

	bool tank::compareTimeStamps(RemoteTankData data1, RemoteTankData data2) {
		return (data1.timestamp_ < data2.timestamp_);
	}

	void tank::onCollision(IEntity* collider) {
		if (collider->type_ == WALL) preventCollision();
	}

	void tank::SetLocal(bool pIsLocal) {
		isLocal_ = pIsLocal;
	}

	void tank::AddSentMessage(message_client_to_server* pMessage, time dt) {
		if (!isEnabled)
			return;
		SetPosition(transform_.position_ + targetMoveDirection(pMessage) * (TILE_SIZE * TANK_SPEED * dt.as_seconds()));
		message_client_to_server* sentMessage = new message_client_to_server;
		sentMessage->input_field = pMessage->input_field;
		sentMessage->turret_angle = pMessage->turret_angle;
		sentMessage->input_number = pMessage->input_number;

		InputData data;
		data.message_ = sentMessage;
		data.dt_ = dt;
		sentInputData_.push_back(data);
	}

	void tank::interpolateEntity() {

		//go back in time
		float past = time::now().as_milliseconds() - REMOTE_TANK_UPDATE_DELAY;

		//get target and source of lerp
		bool hasData = false;
		RemoteTankData lerpSource;
		RemoteTankData lerpTarget;
		for (int i = 0; i < entityHistory_.size(); i++) {
			if (entityHistory_[i].timestamp_ > past) {
				if (i < 1) {
					return;
				}
				lerpTarget = entityHistory_[i];
				lerpSource = entityHistory_[i - 1];
				hasData = true;
				break;
			}
		}
		if (!hasData) return;
		//get lerp amount
		float timestampDeltaTime = lerpTarget.timestamp_ - lerpSource.timestamp_;
		float timeSinceMessage = past - lerpSource.timestamp_;
		float lerpAmount = timeSinceMessage / timestampDeltaTime;

		//lerp
		vector2 targetPosition = vector2::Lerp(lerpSource.position_, lerpTarget.position_, lerpAmount);
		SetPosition(targetPosition);

		float targetRotation = lerp(lerpSource.angle_, lerpTarget.angle_, lerpAmount);
		SetTurretAngle(targetRotation);

		//delete what's older than 1000ms
		int it = -1;
		for (int i = 0; i < entityHistory_.size(); i++) {
			if (entityHistory_[i].timestamp_ < time::now().as_milliseconds() - REMOTE_TANK_HISTORY_SPAN) {
				it = i;
			}
		}
		if (it >= 0) {
			entityHistory_.erase(entityHistory_.begin(), entityHistory_.begin() + it);
		}

		//Set Tank Rotation
		if (lerpSource.position_ != lerpTarget.position_) {
			vector2 direction = lerpSource.position_ - lerpTarget.position_;
			float angle = atan2(direction.y_, direction.x_) * (180 / PI);
			transform_.set_rotation(angle);
		}
	}

	vector2 tank::targetMoveDirection(message_client_to_server* pMessage) {
		if (pMessage->get_input(message_client_to_server::UP) && pMessage->get_input(message_client_to_server::RIGHT)) return { 0.7071f ,-0.7071f };  //Normalised Diagonal Vector
		if (pMessage->get_input(message_client_to_server::UP) && pMessage->get_input(message_client_to_server::LEFT)) return { -0.7071f ,-0.7071f };
		if (pMessage->get_input(message_client_to_server::DOWN) && pMessage->get_input(message_client_to_server::LEFT)) return { -0.7071f ,0.7071f };
		if (pMessage->get_input(message_client_to_server::DOWN) && pMessage->get_input(message_client_to_server::RIGHT)) return { 0.7071f ,0.7071f };
		if (pMessage->get_input(message_client_to_server::RIGHT))  return { 1.0f ,0 };
		if (pMessage->get_input(message_client_to_server::LEFT))  return { -1.0f,0 };
		if (pMessage->get_input(message_client_to_server::UP))	return { 0,-1.0f };
		if (pMessage->get_input(message_client_to_server::DOWN))	return { 0, 1.0f };
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

	float tank::targetTurretRotation() {
		vector2 aimVector = aimVector_;
		turretRotation = atan2(aimVector.y_, aimVector.x_) * (180 / PI);
		return turretRotation;
	}

	void tank::updateAimVector(mouse ms) {
		vector2 mousePosition((float)ms.x_, (float)ms.y_);
		aimVector_ = vector2(mousePosition - turretTransform_.position_).normalized();
	}

	void tank::preventCollision() {
		SetPosition(previousPosition);		
	}

	bool tank::ownsBullet(IEntity* pBullet) {
		for (IEntity* b : bullets_) {
			if (pBullet == b) return true;
		}
		return false;
	}

#pragma region ClientSpecificFunctions

	void tank::UpdateRemoteTank(bool pAlive,
							vector2 pPos, 
							float pAngle,
							float pTimeStamp) {
		if (isLocal_)
			return;

		SetActive(pAlive);

		RemoteTankData newData;
		newData.timestamp_ = pTimeStamp;
		newData.position_ = pPos;
		newData.angle_ = pAngle;
		entityHistory_.push_back(newData);
		std::sort(entityHistory_.begin(), entityHistory_.end(), compareTimeStamps);
	}

	void tank::UpdateLocalTank(bool pAlive, 
							   vector2 pPos,  
							   uint32 pInputNumber) {
		if (!isLocal_)
			return;
		SetActive(pAlive);
		previousPosition = transform_.position_;
		
		int it = 0;
		bool hasMessage = false;
		for(int i = 0; i < sentInputData_.size(); i++) {
			if(pInputNumber == sentInputData_[i].message_->input_number) {
				it = i;
				hasMessage = true;
				break;
			}
		}
		if (!hasMessage) {
			return;
		}
		vector2 targetPos = pPos;
		for (int i = it; i < sentInputData_.size(); i++) {
			targetPos += targetMoveDirection(sentInputData_[i].message_) * (TILE_SIZE * TANK_SPEED * sentInputData_[i].dt_.as_seconds());
		}
		sentInputData_.erase(sentInputData_.begin(), sentInputData_.begin() + it);
		SetPosition(targetPos);
	}

	void tank::SetPosition(vector2 pPos) {
		transform_.set_position(pPos);
		turretTransform_.position_ = transform_.position_;
		setColliderPosition();
	}

	void tank::SetTurretAngle(float pAngle) {
		turretTransform_.set_rotation(pAngle);
	}

	void tank::SetActive(bool pIsActive) {
		isEnabled = pIsActive;
	}

#pragma endregion
}
