#pragma once
#include <stdint.h>
#include <list>
#include <boost/crc.hpp>

//#define PARALLEL_MODE

using hanoi_limit = uint8_t;
constexpr hanoi_limit HANOI_LIMIT_NULL = 255; //Reserver

using crc64_t = boost::crc_optimal<64, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true>;

template<hanoi_limit N>
using uuid_columns_pack_t = std::array<uint32_t, N>;
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