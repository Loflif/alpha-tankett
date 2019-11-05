#include "entityManager.h"

namespace tankett {

	entityManager::entityManager() {
		wallTexture_.create_from_file("assets/wallTile.png");
		tankTexture_.create_from_file("assets/tank.png");
		turretTexture_.create_from_file("assets/turret.png");
		bulletTexture_.create_from_file("assets/bullet.png");

		createLevel();
		createBulletBuffer();
		createTankBuffer();
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
			sprite tankSprite(tankTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			sprite turretSpr(turretTexture_, vector2(TILE_SIZE * TANK_SIZE, TILE_SIZE * TANK_SIZE));
			tank* t = new tank(tankSprite, turretSpr, SPAWN_POINTS[i].x_, SPAWN_POINTS[i].y_, (uint8)i);
			entities_.push_back(t);
			tanks_[i] = t;
		}
	}

	void entityManager::createBulletBuffer() {
		sprite spr(bulletTexture_, vector2(TILE_SIZE * BULLET_SIZE, TILE_SIZE * BULLET_SIZE));

		for (int i = 0; i < BULLET_MAX_COUNT; i++) {
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

	
	
	void entityManager::UpdateLocalTank(server_to_client_data pData) {
		dynamic_array<vector2> bulletPos;
		for (bullet_data b : pData.bullets) {
			bulletPos.push_back(b.position * TILE_SIZE);
		}

		tanks_[localTankID_]->UpdateValues(pData.alive,
								  pData.position * TILE_SIZE,
								  pData.angle,
								  bulletPos);
	}

	void entityManager::UpdateRemoteTank(server_to_client_data pData) {
		dynamic_array<vector2> bulletPos;
		for (bullet_data b : pData.bullets) {
			bulletPos.push_back(b.position * TILE_SIZE);
		}

		tanks_[pData.client_id]->UpdateValues(pData.alive,
										pData.position * TILE_SIZE,
										pData.angle,
										bulletPos);
	}
#pragma endregion
	void entityManager::fireBullet(tank* t) {
		if (shootingCooldown_ > 0)
			return;
		for (bullet* b : bullets_) {
			if (!b->isEnabled) {
				vector2 tPos = t->transform_.position_;
				b->fire(tPos.x_, tPos.y_, t->aimVector_);
				t->bullets_.push_back(b);
				entities_.push_back(b);
				shootingCooldown_ = FIRE_RATE;
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
		pRenderSystem.clear(0xff0e1528); //Background Color
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
		if (pMouse.is_pressed(MOUSE_BUTTON_LEFT)) {
			shoot = true;
			fireBullet(tanks_[localTankID_]);
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

		if (localTankID_ < 255) {
			vector2 mousePosition((float)pMouse.x_, (float)pMouse.y_);
			vector2 aimVector = vector2(mousePosition - tanks_[localTankID_]->transform_.position_).normalized();

			msg->turret_angle = atan2(aimVector.y_, aimVector.x_) * (180 / PI);
			msg->type_ = NETWORK_MESSAGE_CLIENT_TO_SERVER;
		}
		return msg;
	}


}
