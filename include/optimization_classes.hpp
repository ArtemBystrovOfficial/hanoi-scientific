#pragma once

#include "structs.hpp"
#include <functional>
#include <unordered_map>
#include <map>
#include <shared_mutex>

namespace hanoi {

template<hanoi_limit N, hanoi_limit M>
class OptimizationUnit {
public:
	virtual void optimize(frame_moves* moves, const Frame<N, M>& frame) = 0;
	virtual std::string name() const = 0;
	uint64_t all_count_of_remove = 0;
};

inline static uint64_t eraseVisitor(frame_moves* moves, const std::function<bool(const std::pair<hanoi_limit, hanoi_limit>&)>& func) {
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
class BasicShiftingRules : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
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
template<hanoi_limit N, hanoi_limit M, bool parallel>
class AntiLoopDP : public OptimizationUnit<N, M> {
public:

	AntiLoopDP() {

	}

	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		if constexpr (parallel)
			mut.lock();
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			Frame frame_new(frame, from, to);
			auto hash = frame_new.getHashColumns();
			auto it = m_history.find(hash);
			if (it == m_history.end()) {
				m_history[hash] = frame.getDepth();
				return true;
			}
			else
				return it->second > frame.getDepth();
			});
		if constexpr (parallel)
			mut.unlock();
	}
	std::string name() const override {
		return "AntiLoopDP";
	}
private:
	std::mutex mut;
	std::map<uuid_columns_pack_t<N>, hanoi_limit> m_history;
};

// BAD GENERATIONS  
#include <iostream>
template<hanoi_limit N, hanoi_limit M, bool parallel>
class BadGenerations : public OptimizationUnit<N, M> {
public:
	BadGenerations() {
		m_minimal_move.reserve(sizeof(hanoi_limit));
	}

	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		bool is_kill = false;

		auto [depth_from_max_circle, max_circle] = frame.getMaxCircle();
		if constexpr (parallel)
			mut.lock();
		auto it = m_minimal_move.find(depth_from_max_circle);
		if (it == m_minimal_move.end()) {
			m_minimal_move[depth_from_max_circle] = max_circle;
		}
		else {
			if (it->second > max_circle)
				is_kill = true;
			else if (it->second < max_circle) {
				m_minimal_move[depth_from_max_circle] = max_circle;
			}
		}
		if constexpr (parallel)
			mut.unlock();
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {;
		return !is_kill;
			});
	}
	std::string name() const override {
		return "BadGenerations";
	}
private:
	//DEPTH - MAX CIRCLE
	std::unordered_map<hanoi_limit, hanoi_limit> m_minimal_move;
	std::mutex mut;
};

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
				|| (frame.getColumnSize(to) == M - 1 && to == 0));
			});
	}
	std::string name() const override {
		return "EmptyMove";
	}
};

// THE ADVANTAGE OF NON-FIRST COLUMNS
//	   ---        ---	
//     2 |        2 |
//	ok 3 1    bad 3 v 1
////////////////////////
template<hanoi_limit N, hanoi_limit M>
class AdvantageColumns : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			if (to == 0) {
				for (hanoi_limit i = 1; i < N; ++i) {
					if (!frame.getColumnSize(i))
						return false;
				}
			}
			return true;
			});
	}
	std::string name() const override {
		return "AdvantageColumns";
	}
};

// SIMETRIC MOVES   
//	   ----|        --|	
//     1   |        1 |
//	ok 2   V    bad 2 V 
////////////////////////
template<hanoi_limit N, hanoi_limit M>
class SimetricMoves : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		bool stop_simectric_to_others = false;
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			if (from == 0 && !frame.getColumnSize(to)) {
				if (stop_simectric_to_others)
					return false;
				stop_simectric_to_others = true;
			}
			return true;
			});
	}
	std::string name() const override {
		return "SimetricMoves";
	}
};

// LAST STEP NONE MOVEBLE  
//	   ___            ___	
//     1 |  ->      1 | |
//	ok 2 V		bad 2 1 V
///////////////////////////
template<hanoi_limit N, hanoi_limit M>
class LastStepNoneMoveble : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		bool stop_simectric_to_others = false;
		all_count_of_remove += eraseVisitor(moves, [&](const std::pair<hanoi_limit, hanoi_limit>& move) -> bool {
			const auto& [from, to] = move;
			return from != frame.getLastMoveTo();
			});
	}
	std::string name() const override {
		return "LastStepNoneMoveble";
	}
};

// IF ANY EMPTY COLUMN, MOVE  
//	   ___            	
//     1 |     
//	ok 2 V 0		
////////////////
template<hanoi_limit N, hanoi_limit M>
class EmptyAnyColumnMove : public OptimizationUnit<N, M> {
public:
	void optimize(frame_moves* moves, const Frame<N, M>& frame) override {
		for (auto& [from, to] : moves->moves) {
			if (!frame.getColumnSize(to) && from == 0) {
				all_count_of_remove += moves->moves.size() - 1;
				std::pair<hanoi_limit, hanoi_limit> out_pair = { from,to };
				moves->moves.clear();
				moves->moves.push_back(out_pair);
				break;
			}
		}
	}
	std::string name() const override {
		return "EmptyAnyColumnMove";
	}
};

}