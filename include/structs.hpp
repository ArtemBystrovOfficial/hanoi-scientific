#pragma once
#include <stdint.h>
#include <array>

using hanoi_limit = uint8_t;
constexpr hanoi_limit HANOI_LIMIT_NULL = 255; //Reserver

struct move {
    hanoi_limit from, to;
};

template<hanoi_limit N>
struct frame_moves {
	 std::array<move, N> moves;
};
