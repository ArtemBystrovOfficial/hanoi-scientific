#pragma once

#include "structs.hpp"
#include <functional>

template<hanoi_limit N, hanoi_limit M>
class OptimizationUnit {
public:
	virtual void optimize(frame_moves* moves, const Frame<N,M>& frame) = 0;
};

inline static void eraseVisitor(frame_moves* moves, const std::function<bool(const std::pair<hanoi_limit,hanoi_limit> &)> & func) {
	auto i = moves->moves.begin();
	auto end = moves->moves.end();
	while (i != end) {
		if (func(*i))
			++i; // alternatively, i = items.erase(i);
		else
			moves->moves.erase(i++);
	}
}

//      ___	     ___
//    4 | |    4 V |
//    5 2 V    5 2 |
// NO 6 3 1 OK 6 3 1
/////////////////////
template<hanoi_limit N, hanoi_limit M>
class BasicShiftingRules : public OptimizationUnit<N,M> {
public:
	void optimize(frame_moves* moves, const Frame<N,M>& frame) override { 
		eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			if (!frame.getColumnSize(from)) // empty column; 
				return false;
			if (!frame.getColumnSize(to)) // empty column; 
				return true;
			return frame.getLastCircle(from) < frame.getLastCircle(to);
		});
	}
};

template<hanoi_limit N, hanoi_limit M>
class B : public OptimizationUnit<N,M> {
public:
	void optimize(frame_moves* moves, const Frame<N,M>& frame) override {
	}
};