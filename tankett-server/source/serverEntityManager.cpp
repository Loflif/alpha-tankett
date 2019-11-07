#include "serverEntityManager.h"

namespace tankett {

	serverEntityManager::serverEntityManager() {
		createLevel();
		createBulletBuffer();
		createTankBuffer();
	}

	serverEntityManager::~serverEntityManager() {
	}

	void serverEntityManager::parseClientMessage(message_client_to_server message, uint8 clientID, const time& pDeltaRecieveTime) {
		if (message.get_input(message_client_to_server::SHOOT)) {
			fireBullet(tanks_[clientID]);
		}
		vector2 targetDirection = targetMoveDirection(message);
		if (targetDirection.x_ != 0
			&& targetDirection.y_ != 0) {
			int i = 0;
		}
		float speed = 4 * pDeltaRecieveTime.as_seconds();
		tanks_[clientID]->SetPosition(tanks_[clientID]->transform_.position_ + targetDirection * speed);
		tanks_[clientID]->turretRotation_ = message.turret_angle;
	}

	void serverEntityManager::addEntity(IServerEntity& pEntity) {
		entities_.push_back(&pEntity);
	}

	serverTank* serverEntityManager::getTank(int ID) {
		return tanks_[ID];
	}

	void serverEntityManager::spawnTank(int id) {
		tanks_[id]->isEnabled = true;
		tanks_[id]->SetPosition(SPAWN_POINTS[id]);
	}

	void serverEntityManager::update(time dt) {
		for(IServerEntity* e : entities_) {
			if(e->isEnabled) {
				e->update(dt);
			}
		}
		respawnTanks(dt);
	}

	void serverEntityManager::respawnTanks(time dt) {
		for (serverTank* t : tanks_) {
			if (!t->isAlive) {
				t->respawnTime -= dt;
				if (t->respawnTime.as_milliseconds() < 0) {
					t->SetPosition(SPAWN_POINTS[t->id_]);
					t->isEnabled = true;
					t->isAlive = true;
					t->respawnTime = time(RESPAWN_MILLISECONDS);
				}
			}
		}
	}

	void serverEntityManager::resetTank(int ID) {
		tanks_[ID]->isEnabled = false;
		tanks_[ID]->SetPosition(SPAWN_POINTS[ID]);
		tanks_[ID]->isAlive = true;
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

	vector2 serverEntityManager::targetMoveDirection(message_client_to_server message) {
		if (message.get_input(message_client_to_server::UP) && message.get_input(message_client_to_server::RIGHT)) return { 0.7071f ,-0.7071f };  //Normalised Diagonal Vector
		if (message.get_input(message_client_to_server::UP) && message.get_input(message_client_to_server::LEFT)) return { -0.7071f ,-0.7071f };
		if (message.get_input(message_client_to_server::DOWN) && message.get_input(message_client_to_server::LEFT)) return { -0.7071f ,0.7071f };
		if (message.get_input(message_client_to_server::DOWN) && message.get_input(message_client_to_server::RIGHT)) return { 0.7071f ,0.7071f };
		if (message.get_input(message_client_to_server::RIGHT))  return { 1.0f ,0 };
		if (message.get_input(message_client_to_server::LEFT))  return { -1.0f,0 };
		if (message.get_input(message_client_to_server::UP))	return { 0,-1.0f };
		if (message.get_input(message_client_to_server::DOWN))	return { 0, 1.0f };
		return { 0,0 };
	}

	void serverEntityManager::createTankBuffer() {
		for (int i = 0; i < 4; i++) {
			serverTank* t = new serverTank(SPAWN_POINTS[i], (uint8)i);
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
						if (entities_[i]->type_ == TANK || entities_[j]->type_ == TANK) {
							int x = 0;
						}
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
		if (t->shootingCooldown_ > 0
			|| !t->isEnabled)
			return;
		for(serverBullet* b : bullets_) {
			if(!b->isEnabled) {
				vector2 tPos = t->transform_.position_;
				b->fire(tPos, t->getAimVector(), t->getUnusedBulletID(), t->id_);
				t->bullets_.push_back(b);
				t->shootingCooldown_ = FIRE_RATE;
				break;
			}
		}
	}
#pragma endregion
}
