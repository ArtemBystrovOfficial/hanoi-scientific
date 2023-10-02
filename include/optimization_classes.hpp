#pragma once

#include "structs.hpp"
#include <functional>
#include <unordered_map>
#include <map>

#ifdef PARALLEL_MODE
#include <shared_mutex>
#endif

template<hanoi_limit N, hanoi_limit M>
class OptimizationUnit {
public:
	virtual void optimize(frame_moves* moves, const Frame<N,M>& frame) = 0;
	virtual std::string name() const = 0;
	uint64_t all_count_of_remove = 0;
};

inline static uint64_t eraseVisitor(frame_moves* moves, const std::function<bool(const std::pair<hanoi_limit,hanoi_limit> &)> & func) {
	auto i = moves->moves.begin();
	auto end = moves->moves.end();
	uint64_t sum_of_rem = 0;
	while (i != end) {
		if (func(*i))
			++i; // alternatively, i = items.erase(i);
		else {
			sum_of_rem++;
			moves->moves.erase(i++);
		}
	}
	return sum_of_rem;
}

//the main optimizations on which the logic of moving rings and eliminating loops is based

// BASIC RULE
//      ___	     ___
//    4 | |    4 V |
//    5 2 V    5 2 |
// NO 6 3 1 OK 6 3 1
/////////////////////
template<hanoi_limit N, hanoi_limit M>
class BasicShiftingRules : public OptimizationUnit<N,M> {
public:
	void optimize(frame_moves* moves, const Frame<N,M>& frame) override { 
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			if (!frame.getColumnSize(from)) // empty column; 
				return false;
			if (!frame.getColumnSize(to)) // empty column; 
				return true;
			return frame.getLastCircle(from) < frame.getLastCircle(to);
		});
	}
	std::string name() const override {
		return "BasicShiftingRules";
	}
};

//other optimizations 

// LOOPS
//        ___	  ___
//      4 | V   4 V |
//      5 1     5  
// BAD  6 3     6 3 1
/////////////////////

using hash_count_first_level = uint32_t;
using hash_count_second_level = uint64_t;
template<hanoi_limit N, hanoi_limit M>
class AntiLoopDP : public OptimizationUnit<N, M> {
public:

	AntiLoopDP() {
	}

	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
				const auto& [from, to] = move;
				Frame frame_new(frame, from, to);
				auto hash = frame_new.getHashColumns();
#ifdef PARALLEL_MODE
				mut.lock_shared();
#endif
				auto it = m_history.find(hash);
#ifdef PARALLEL_MODE
				mut.unlock_shared();
#endif
				if (it == m_history.end()) {
#ifdef PARALLEL_MODE
					mut.lock();
#endif
					m_history[hash] = frame.getDepth();
#ifdef PARALLEL_MODE
					mut.unlock();
#endif
					return true;
				}
				else 
					return it->second > frame.getDepth();
			});
	}
	std::string name() const override {
		return "AntiLoopDP";
	}
private:
#ifdef PARALLEL_MODE
	static std::shared_mutex mut;
#endif
	static std::map<uuid_columns_pack_t<N>, hanoi_limit> m_history;
};
template<hanoi_limit N, hanoi_limit M>
std::map<uuid_columns_pack_t<N>, hanoi_limit> AntiLoopDP<N, M>::m_history = {};

#ifdef PARALLEL_MODE
template<hanoi_limit N, hanoi_limit M>
std::shared_mutex AntiLoopDP<N, M>::mut = {};
#endif

// EMPTY MOVE
//      ___		   V___
//    4 | |		   1  |
//    5 | V 1	   2  |
// NO 6 3   2 AND  3  |(to begin)
/////////////////////
template<hanoi_limit N, hanoi_limit M>
class EmptyMove : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
			all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			return !(frame.getColumnSize(from) == 1 && frame.getColumnSize(to) == 0 && from != 0
				|| ( frame.getColumnSize(to) == M - 1 && to == 0)); 
		});
	}
	std::string name() const override {
		return "EmptyMove";
	}
};
