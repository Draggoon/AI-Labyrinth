#pragma once


namespace Labyrinth {
	/**
	* Directions
	*
	*	The 4 possible directions
	*/
	typedef enum Directions_t {
		none,
		up,
		down,
		left,
		right
	} Directions;

	typedef enum Cell_t {
		empty,
		wall
	} Cell;

	/**
	* Position
	*
	*	An X;Y coordinate
	*/
	class Position {
	public:
		Position(const int& x = 0, const int& y = 0) : x(x), y(y) {};
		bool operator==(const Position& p) { return (p.x == x) && (p.y == y); };

		int x;
		int y;
	};
}
