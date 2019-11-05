#include "serverTank.h"

namespace tankett {
	serverTank::serverTank(vector2 pSpawnPos) {
		position_ = pSpawnPos;
		type_ = TANK;
		size_ = { tankett::TANK_SIZE, tankett::TANK_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(position_ - size_ / 2);
	}


	serverTank::~serverTank() {
	}

	void serverTank::Move(vector2 direction) {
		previousPosition_ = transform_.position_;
	}

	void serverTank::onCollision(IServerEntity* collider) {

	}

	void serverTank::SetPosition(vector2 pNewPosition) {
		position_ = pNewPosition;
		collider_.set_position(position_ - size_ / 2);
	}
}



