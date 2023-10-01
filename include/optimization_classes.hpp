#pragma once

#include "structs.hpp"
#include <functional>
#include <unordered_map>

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

//the main optimizations on which the logic of moving rings and eliminating loops is based

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


//        ___	  ___
//      4 | V   4 V |
//      5 1     5  
// BAD  6 3     6 3 1
/////////////////////

using hash_count = uint32_t;
template<hanoi_limit N, hanoi_limit M>
class AntiLoopDP : public OptimizationUnit<N,M> {
public:
	AntiLoopDP() {
		m_history.reserve(sizeof(uint32_t));
	}

	void optimize(frame_moves* moves, const Frame<N,M>& frame) override {
		eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			auto hash = frame.getHashWithMove(from, to);
			auto it = m_history.find(hash);
			if (it == m_history.end())
				m_history[hash] = frame.getDepth();
			else
				return it->second > frame.getDepth();
			return true;
		});
	}
private:
	static std::unordered_map<hash_count, hanoi_limit> m_history;
};
template<hanoi_limit N, hanoi_limit M>
std::unordered_map<hash_count, hanoi_limit> AntiLoopDP<N,M>::m_history = {};

//other optimizations