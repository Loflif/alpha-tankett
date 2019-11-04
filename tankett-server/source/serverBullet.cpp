#pragma once
#include "serverBullet.h"

namespace tankett {
	serverBullet::serverBullet() {
		position_ = {0, 0};
		type_ = BULLET;
		size_ = { tankett::BULLET_SIZE, tankett::BULLET_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(position_ - size_ / 2);
	}

	serverBullet::~serverBullet() {
	}

	void serverBullet::onCollision(IServerEntity* collider) {
	}

	void serverBullet::SetPosition(vector2 pNewPosition) {
		position_ = pNewPosition;
		collider_.set_position(position_ - size_ / 2);
	}
}

