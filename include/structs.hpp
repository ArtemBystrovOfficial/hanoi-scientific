#pragma once
#include <stdint.h>
#include <list>

using hanoi_limit = uint8_t;
constexpr hanoi_limit HANOI_LIMIT_NULL = 255; //Reserver

using frame_moves = std::list<std::pair<hanoi_limit, hanoi_limit>>;

template <hanoi_limit N>
frame_moves make_basic_moves() {
	frame_moves moves; //rvo
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (i != j)
				moves.push_back({ i,j });
	return moves;
}

//Доделать итерацию по мувам и сделать первые верные ходы хотябы куда можно