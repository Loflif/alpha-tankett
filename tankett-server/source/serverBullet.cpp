#pragma once
#include "serverBullet.h"

namespace tankett {
	serverBullet::serverBullet() {
		position_ = {0, 0};
		type_ = BULLET;
		size_ = { BULLET_SIZE, BULLET_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(position_ - size_ / 2);
		isEnabled = false;
	}

	serverBullet::~serverBullet() {
	}

	

	void serverBullet::update(time dt) {
		vector2 newPosition = { position_.x_ += direction_.x_ * SPEED_ * dt.as_seconds(),position_.y_ += direction_.y_ * SPEED_ * dt.as_seconds() };
		SetPosition(newPosition);
		
	}

	void serverBullet::SetPosition(vector2 pNewPosition) {
		position_ = pNewPosition;
		collider_.set_position(position_ - size_ / 2);
	}

	void serverBullet::fire(vector2 pSpawnPos, vector2 pDirection) {
		SetPosition(pSpawnPos);
		direction_ = pDirection;
		isEnabled = true;
	}
	
	void serverBullet::onCollision(IServerEntity* collider) {
		if (collider->type_ == WALL) {
			isEnabled = false;
		}
	}
}

