#pragma once
#include <stdint.h>
#include <list>

using hanoi_limit = uint8_t;
constexpr hanoi_limit HANOI_LIMIT_NULL = 255; //Reserver

struct frame_moves {
	using frame_moves_t = std::list <std::pair<hanoi_limit, hanoi_limit>>;
	
	void initBegin() {
		current_elem = moves.begin();
	}

	std::pair<hanoi_limit, hanoi_limit> nextElem() {
		return *(current_elem++);
	}

	bool isEnd() {
		return current_elem == moves.end();
	}

	frame_moves_t moves;
	frame_moves_t::iterator current_elem;
};

template <hanoi_limit N>
frame_moves make_basic_moves() {
	frame_moves moves; //rvo
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (i != j)
				moves.moves.push_back({ i,j });
	return moves;
}

//Доделать итерацию по мувам и сделать первые верные ходы хотябы куда можно