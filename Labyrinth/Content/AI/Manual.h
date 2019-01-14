#pragma once

#include "Player.h"


namespace Labyrinth {
	class Manual : public Player {

	public:
		// Inherited via AI
		virtual Directions nextMove(Position current, std::vector<Cell> surroundings) override;
		void moveDirection(Directions dir);

	protected:
		Directions m_next;
	};
}
