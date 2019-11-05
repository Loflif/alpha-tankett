#include "serverTank.h"

namespace tankett {
	serverTank::serverTank(vector2 pSpawnPos, uint8 ID) {
		transform_.position_ = pSpawnPos;
		type_ = TANK;
		size_ = { tankett::TANK_SIZE, tankett::TANK_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(transform_.position_ - size_ / 2);
		id_ = ID;
	}


	serverTank::~serverTank() {
	}

	void serverTank::update(time dt) {
		//Set Collider Pos
		vector2 targetPosition = transform_.position_ - size_ / 2;
		collider_.set_position(targetPosition);
	}


	vector2 serverTank::getAimVector() {
		float theta = turretRotation_ * PI / 180;
		return vector2(cos(theta), sin(theta));
	}

	void serverTank::onCollision(IServerEntity* collider) {
		if (collider->type_ == WALL) {
			transform_.position_ = previousPosition_;
		}
	}

	void serverTank::SetPosition(vector2 pNewPosition) {
		previousPosition_ = transform_.position_;
		transform_.position_ = pNewPosition;
		collider_.set_position(transform_.position_ - size_ / 2);
	}
}

	void serverTank::update(time dt) {
		shootingCooldown_ -= dt.as_seconds();
	}
}