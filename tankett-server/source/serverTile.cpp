#pragma once
#include "serverTile.h"

namespace tankett {
	serverTile::serverTile(vector2 pSpawnPos) {
		position_ = pSpawnPos;
		type_ = WALL;
		size_ = { 1.0f, 1.0f };
		collider_ = rectangle(0, 0, size_.x_, size_.y_);
		collider_.set_position(position_ - size_ / 2);
	}

	serverTile::~serverTile() {
	}

	void serverTile::onCollision(IServerEntity* collider) {
	}
}

