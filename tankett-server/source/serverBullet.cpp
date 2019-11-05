#pragma once
#include "serverBullet.h"

namespace tankett {
	serverBullet::serverBullet() {
		transform_.set_position({0, 0});
		type_ = BULLET;
		size_ = { tankett::BULLET_SIZE, tankett::BULLET_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(transform_.position_ - size_ / 2);
	}

	serverBullet::~serverBullet() {
	}

	void serverBullet::onCollision(IServerEntity* collider) {
	}

	void serverBullet::SetPosition(vector2 pNewPosition) {
		transform_.set_position(pNewPosition);
		collider_.set_position(transform_.position_ - size_ / 2);
	}
}

