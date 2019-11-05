#include "serverEntityManager.h"

namespace tankett {

	serverEntityManager::serverEntityManager() {
		createLevel();
		createBulletBuffer();
		createTankBuffer();
	}

	serverEntityManager::~serverEntityManager() {
	}

	void serverEntityManager::addEntity(IServerEntity& pEntity) {
		entities_.push_back(&pEntity);
	}

	serverTank& serverEntityManager::getTank(int ID) {
		return *tanks_[ID];
	}

	void serverEntityManager::update(time dt) {
		for(IServerEntity* e : entities_) {
			if(e->isEnabled) {
				e->update(dt);
			}
		}
	}

#pragma region Initialisation
	void serverEntityManager::createLevel() {
		int rows = std::extent<decltype(LEVEL), 0>::value; // Get the amount of rows
		for (size_t row = 0; row < rows; row++) {
			int column = 0;
			for (TILE_TYPE type : LEVEL[row]) {
				if (type == W) {
					entities_.push_back(new serverTile(vector2((float)column, (float)row)));
				}
				column++;
			}
		}
	}

	void serverEntityManager::createTankBuffer() {
		for (int i = 0; i < 4; i++) {
			serverTank* t = new serverTank(spawnPoints[i], (uint8)i);
			entities_.push_back(t);
			tanks_[i] = t;
		}
	}

	void serverEntityManager::createBulletBuffer() {
		for (int i = 0; i < BULLET_MAX_COUNT; i++) {
			serverBullet* b = new serverBullet();
			bullets_[i] = b;
			entities_.push_back(b);
		}
	}
#pragma endregion

#pragma region CollisionHandling
	void serverEntityManager::manageCollisions() {
		for (int i = 0; i < entities_.size(); i++) {
			for (int j = 0; j < entities_.size(); j++) {
				if (isCollisionPair(entities_[i], entities_[j])) {
					if ((j != i) && (checkCollision(entities_[i], entities_[j]))) {
						if (entities_[i]->isEnabled && entities_[j]->isEnabled) {
							entities_[i]->onCollision(entities_[j]);
							entities_[j]->onCollision(entities_[i]);
						}
					}
				}
			}
		}
	}

	bool serverEntityManager::checkCollision(IServerEntity* firstEntity, IServerEntity* secondEntity) {
		const rectangle firstRectangle = firstEntity->collider_;
		const rectangle secondRectangle = secondEntity->collider_;


		const float firstTop = firstRectangle.y_;
		const float firstBottom = firstRectangle.y_ + firstRectangle.height_;
		const float firstLeft = firstRectangle.x_;
		const float firstRight = firstRectangle.x_ + firstRectangle.width_;

		const float secondTop = secondRectangle.y_;
		const float secondBottom = secondRectangle.y_ + secondRectangle.height_;
		const float secondLeft = secondRectangle.x_;
		const float secondRight = secondRectangle.x_ + secondRectangle.width_;


		if (firstTop > secondBottom || firstBottom < secondTop || firstLeft > secondRight || firstRight < secondLeft) {
			return false;
		}
		return true;

	}

	bool serverEntityManager::isCollisionPair(IServerEntity* pFirstEntity, IServerEntity* pSecondEntity) {
		for (std::pair<ENTITY_TYPE, ENTITY_TYPE> p : collisionPairs_) {
			if ((p.first == pFirstEntity->type_ && p.second == pSecondEntity->type_) ||
				(p.first == pSecondEntity->type_ && p.second == pFirstEntity->type_)) {
				return true;
			}
		}
		return false;
	}

	void serverEntityManager::fireBullet(serverTank* t) {
		if (t->shootingCooldown_ > 0)
			return;
		for(serverBullet* b : bullets_) {
			if(!b->isEnabled) {
				vector2 tPos = t->transform_.position_;
				b->fire(tPos, t->getAimVector());
				t->bulletPositions_.push_back(b->position_);
				entities_.push_back(b);
				t->shootingCooldown_ = FIRE_RATE;
				break;
			}
		}
	}
#pragma endregion
}
