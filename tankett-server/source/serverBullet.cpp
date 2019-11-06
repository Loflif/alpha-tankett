#pragma once
#include "serverBullet.h"

namespace tankett {
	serverBullet::serverBullet() {
		transform_.set_position({0, 0});
		type_ = BULLET;
		size_ = { BULLET_SIZE, BULLET_SIZE };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(transform_.position_ - size_ / 2);
		isEnabled = false;
		id_ = 255;
	}

	serverBullet::~serverBullet() {
	}

	

	void serverBullet::update(time dt) {
		vector2 newPosition = { 
			transform_.position_.x_ + direction_.x_ * SPEED_ * dt.as_seconds(),
			transform_.position_.y_ + direction_.y_ * SPEED_ * dt.as_seconds() 
		};
		SetPosition(newPosition);
		
	}

	void serverBullet::SetPosition(vector2 pNewPosition) {
		transform_.set_position(pNewPosition);
		collider_.set_position(transform_.position_ - size_ / 2);
	}

	void serverBullet::fire(vector2 pSpawnPos, vector2 pDirection, int pID, int pTankID) {
		SetPosition(pSpawnPos);
		direction_ = pDirection.normalized();
		isEnabled = true;
		id_ = pID;
		owner_= pTankID;
	}
	
	void serverBullet::onCollision(IServerEntity* collider) {
		if (collider->type_ == WALL) {
			isEnabled = false;
			id_ = 255;
		}
		
	}
}

