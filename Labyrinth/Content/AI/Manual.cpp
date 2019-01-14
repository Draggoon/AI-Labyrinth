#include "pch.h"
#include "Manual.h"

Labyrinth::Directions Labyrinth::Manual::nextMove(Position current, std::vector<Cell> surroundings) {
	Directions ret = m_next;
	m_next = none;
	return ret;
}

void Labyrinth::Manual::moveDirection(Directions dir) {
	m_next = dir;
}
