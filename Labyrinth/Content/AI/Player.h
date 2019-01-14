#pragma once

#include <vector>
#include "../utils.h"

namespace Labyrinth {
	class Player {
	public:
		virtual Directions nextMove(Position current, std::vector<Cell> surroundings) = 0;
	};
}
