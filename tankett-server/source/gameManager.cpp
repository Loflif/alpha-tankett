#include "gameManager.h"

namespace tankett {

	uint8 gameManager::score_[4] = { 0,0,0,0 };
	
	gameManager::gameManager() {
		
	}

	gameManager::~gameManager() {
	}

	void gameManager::AddScore(uint8 pTankID) {
		score_[pTankID]++;
	}

	void gameManager::ResetScore() {
		for (uint8 s : score_) {
			s = 0;
		}
	}
	

}
