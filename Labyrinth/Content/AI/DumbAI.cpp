#include "pch.h"
#include "DumbAI.h"


Labyrinth::DumbAI::DumbAI () {

}


Labyrinth::Directions Labyrinth::DumbAI::nextMove(Position current, std::vector<Cell> surroundings) {
	bool _up(false), _down(false), _left(false), _right(false);
	std::vector<Directions> v;
	for (int i(0); i < surroundings.size(); ++i) {
		if (surroundings[i] != wall) {
			switch (i) {
			case 0:
				v.push_back(up);
				break;
			case 1:
				v.push_back(down);
				break;
			case 2:
				v.push_back(left);
				break;
			case 3:
				v.push_back(right);
				break;
			default:;
			}
		}
	}
	if (v.size() == 1) {
			return v.back();
	}
	else if (v.size() > 1) {
		std::random_device r;
		std::default_random_engine engine(r());
		std::uniform_int_distribution<int> uniform_dist(0, (int)v.size()-1);
		int dir = uniform_dist(engine);
		return v[dir];
	}
	else
		return none;
}
