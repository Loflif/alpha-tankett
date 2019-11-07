#include "serverTank.h"
#include "gameManager.h"

namespace tankett {
	serverTank::serverTank(vector2 pSpawnPos, uint8 ID) {
		transform_.set_origin(vector2(size_ / 2));
		type_ = TANK;		
		size_ = { TANK_SIZE, TANK_SIZE };		
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		SetPosition(pSpawnPos);
		id_ = ID;
		isAlive = true;
		respawnTime = time(RESPAWN_MILLISECONDS);
	}


	serverTank::~serverTank() {
	}


	void serverTank::update(time dt) {
		shootingCooldown_ -= dt.as_seconds();

		updateBulletList();
	}


	vector2 serverTank::getAimVector() {
		float theta = turretRotation_ * PI / 180;
		return vector2(cos(theta), sin(theta));
	}

	void serverTank::updateBulletList() {
		dynamic_array<serverBullet*> newBullets;
		for (int i = 0; i < bullets_.size(); i++) {
			if (bullets_[i]->isEnabled) {
				newBullets.push_back(bullets_[i]);
			}
		}
		bullets_ = newBullets;
	}

	bool serverTank::ownsBullet(IServerEntity* pBullet) {
		for (IServerEntity* b : bullets_) {
			if (pBullet == b) return true;
		}
		return false;
	}

	int serverTank::getUnusedBulletID() {
		for (uint8 i = 0; i < 10; i++) {
			if (!hasBulletWithID(i)) return i;
		}
		return -1;
	}

	bool serverTank::hasBulletWithID(uint8 pID) {
		for (serverBullet* b : bullets_) {
			if (b->id_ == pID) return true;
		}
		return false;
	}

	void serverTank::onCollision(IServerEntity* collider) {
		if (collider->type_ == WALL) {
			SetPosition(previousPosition_);
		}
		if (collider->type_ == BULLET) {
			if(!ownsBullet(collider)) {
				gameManager::AddScore((uint8)collider->owner_);
				collider->isEnabled = false;
				isEnabled = false;
				isAlive = false;
			}
		}
		if(collider->type_ == TANK) {
			isEnabled = false;
			isAlive = false;
			gameManager::AddScore((uint8)id_);
		}
	}

	void serverTank::SetPreviousPosition() {
		previousPosition_ = transform_.position_;
	}

	void serverTank::SetPosition(vector2 pNewPosition) {
		transform_.position_ = pNewPosition;
		vector2 targetPosition = transform_.position_ - size_/2;
		collider_.set_position(targetPosition);
	}
}
