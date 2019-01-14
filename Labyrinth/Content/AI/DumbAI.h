#pragma once

#include "Player.h"

#include <vector>
#include <random>

namespace Labyrinth {
	class DumbAI : public Player {
	public:
		DumbAI();

	public:
		// Inherited via AI
		virtual Directions nextMove(Position current, std::vector<Cell> surroundings) override;

		std::vector< std::vector<int> > memory;
	};
}
