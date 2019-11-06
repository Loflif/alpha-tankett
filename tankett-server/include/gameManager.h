#pragma once
#include "alpha.h"

using namespace alpha;

namespace tankett {
	class gameManager {
	public:
		static uint8 score_[4];
		gameManager();
		~gameManager();

		static void AddScore(uint8 pTankID);
		static void ResetScore();
	};
}