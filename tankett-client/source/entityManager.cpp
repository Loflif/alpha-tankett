#include "entityManager.h"

namespace tankett {

	entityManager::entityManager() {
		wallTexture_.create_from_file("assets/wallTile.png");
		tankTexture_.create_from_file("assets/tank.png");
		turretTexture_.create_from_file("assets/Turret.png");
		remotetankTexture_.create_from_file("assets/remoteTank.png");
		remoteturretTexture_.create_from_file("assets/remoteTurret.png");
		bulletTexture_.create_from_file("assets/bullet.png");

		createLevel();
		createTankBuffer();
		createBulletBuffer();
	}

	entityManager::~entityManager() {
		for(IEntity* e : entities_) {
			delete(e);
		}
	}

#pragma region Initialisation
	void entityManager::createLevel() {
		int rows = std::extent<decltype(LEVEL), 0>::value; // Get the amount of rows
		for (size_t row = 0; row < rows; row++) {
			int column = 0;
			for (TILE_TYPE type : LEVEL[row]) {
				if (type == W) {
					sprite wallSprite(wallTexture_, vector2(TILE_SIZE, TILE_SIZE));
					entities_.push_back(new tile(wallSprite, (float)column * TILE_SIZE, (float)row * TILE_SIZE));
				}
				column++;
			}
		}
	}

	void entityManager::createTankBuffer() {
		for (int i = 0; i < 4; i++) {
			sprite tankSpr(tankTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			sprite turretSpr(turretTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			sprite remotetankSpr(remotetankTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			sprite remoteturretSpr(remoteturretTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			tank* t = new tank(tankSpr, turretSpr, remotetankSpr, remoteturretSpr, SPAWN_POINTS[i].x_ * TILE_SIZE, SPAWN_POINTS[i].y_ * TILE_SIZE, (uint8)i);
			entities_.push_back(t);
			tanks_[i] = t;
		}
	}

	void entityManager::createBulletBuffer() {
		sprite spr(bulletTexture_, vector2(TILE_SIZE * BULLET_SIZE, TILE_SIZE * BULLET_SIZE));

		for (int i = 0; i < BULLET_MAX_COUNT * 4; i++) {
			bullet* b = new bullet(spr);
			bullets_[i] = b;
			entities_.push_back(b);
		}
	}
#pragma endregion

#pragma region CollisionHandling
	void entityManager::manageCollisions() {
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
	bool entityManager::checkCollision(IEntity* firstEntity, IEntity* secondEntity) {
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

	bool entityManager::isCollisionPair(IEntity* pFirstEntity, IEntity* pSecondEntity) {
		for(std::pair<ENTITY_TYPE, ENTITY_TYPE> p : collisionPairs_) {
			if ((p.first == pFirstEntity->type_ && p.second == pSecondEntity->type_) ||
				(p.first == pSecondEntity->type_ && p.second == pFirstEntity->type_)) {
				return true;
			}
		}
		return false;
	}
#pragma endregion
	
#pragma region Update
	void entityManager::update(keyboard pKeyboard, mouse pMouse, time dt) {
		shootingCooldown_ -= dt.as_seconds();
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->update(pKeyboard, pMouse, dt);
			}
		}
	}

	void entityManager::UpdateTank(server_to_client_data pData) {
		tanks_[pData.client_id]->UpdateValues(pData.alive,
										      pData.position * TILE_SIZE,
										      pData.angle);
		UpdateBullets(pData);
	}
	void entityManager::UpdateBullets(server_to_client_data pData) {
		CompareBulletLists(pData);
		for (int i = 0; i < pData.bullet_count; i++) {
			if (tanks_[pData.client_id]->hasBulletWithID(pData.bullets[i].id)) {
				tanks_[pData.client_id]->getBulletWithID(pData.bullets[i].id)->UpdateData(pData.bullets[i].position * TILE_SIZE);
			}
			else {
				createBullet(tanks_[pData.client_id]);
			}
		}
	}

	void entityManager::Reset() {
		for (IEntity* e : entities_) {
			if (e->type_ == TANK || e->type_ == BULLET) {
				e->isEnabled = false;
			}
		}
	}

	void entityManager::CompareBulletLists(server_to_client_data pData) {
		for (int i = 0; i < tanks_[pData.client_id]->bullets_.size(); i++) {
			if (!isBulletInList(pData, (uint8)tanks_[pData.client_id]->bullets_[i]->id_)) {
				tanks_[pData.client_id]->bullets_[i]->isEnabled = false;
			}
		}
	}
	bool entityManager::isBulletInList(server_to_client_data pData, uint8 pID) {
		for (int i = 0; i < pData.bullet_count; i++) {
			if (pData.bullets[i].id == pID) return true;
		}
		return false;
	}
#pragma endregion
	void entityManager::createBullet(tank* t) {
		for (bullet* b : bullets_) {
			if (!b->isEnabled) {
				vector2 tPos = t->transform_.position_;
				vector2 direction = vector2::zero();
				if (t->isLocal_) {
					direction = t->aimVector_;
					shootingCooldown_ = FIRE_RATE;
				}
				b->fire(tPos.x_, tPos.y_, direction, t->getUnusedBulletID());
				t->bullets_.push_back(b);
				break;
			}
		}
	}
	void entityManager::addEntity(IEntity& pEntity) {
		entities_.push_back(&pEntity);
	}

	tank* entityManager::getTank(int ID) {
		return tanks_[ID];
	}

	void entityManager::setLocalTank(uint8 ID) {
		tanks_[ID]->SetLocal(true);
		localTankID_ = ID;
	}

	uint8 entityManager::getLocalTankID() {
		return localTankID_;
	}

	void entityManager::render(render_system& pRenderSystem) {
		for (IEntity* e : entities_) {
			if (e->isEnabled) {
				e->render(pRenderSystem);
			}
		}
	}

	message_client_to_server* entityManager::checkInput(keyboard pKeyboard, mouse pMouse) {
		message_client_to_server* msg = new message_client_to_server;
		bool shoot = false;
		bool right = false;
		bool left = false;
		bool down = false;
		bool up = false;
		if (localTankID_ < 255) {
			vector2 mousePosition((float)pMouse.x_, (float)pMouse.y_);
			vector2 aimVector = vector2(mousePosition - tanks_[localTankID_]->transform_.position_).normalized();
			msg->turret_angle = atan2(aimVector.y_, aimVector.x_) * (180 / PI);
		}
		if (pMouse.is_pressed(MOUSE_BUTTON_LEFT)) {
			shoot = true;
		}
		if (pKeyboard.is_down(KEYCODE_D)) {
			right = true;
		}
		if (pKeyboard.is_down(KEYCODE_A)) {
			left = true;
		}
		if (pKeyboard.is_down(KEYCODE_S)) {
			down = true;
		}
		if (pKeyboard.is_down(KEYCODE_W)) {
			up = true;
		}
		msg->set_input(shoot, right, left, down, up);
		msg->type_ = NETWORK_MESSAGE_CLIENT_TO_SERVER;
		return msg;
	}


}
