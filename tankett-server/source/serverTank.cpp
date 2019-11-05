#include "serverTank.h"

namespace tankett {
	serverTank::serverTank(vector2 pSpawnPos, uint8 ID) {
		position_ = pSpawnPos;
		type_ = TANK;
		size_ = { tankett::TANK_SIZE, tankett::TANK_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(position_ - size_ / 2);
		id_ = ID;
	}


	serverTank::~serverTank() {
	}

	void serverTank::Move(vector2 direction) {
		previousPosition_ = transform_.position_;
	}

	vector2 serverTank::getAimVector() {
		float theta = turretRotation_ * PI / 180;
		return vector2(cos(theta), sin(theta));
	}

	void serverTank::onCollision(IServerEntity* collider) {
		
	}

	void serverTank::SetPosition(vector2 pNewPosition) {
		position_ = pNewPosition;
		collider_.set_position(position_ - size_ / 2);
	}

	void serverTank::update(time dt) {
		shootingCooldown_ -= dt.as_seconds();
	}
}