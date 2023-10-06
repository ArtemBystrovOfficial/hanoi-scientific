#pragma once
#include "optimization_classes.hpp"
#include <memory>
#include <vector>

namespace hanoi {

template<hanoi_limit N, hanoi_limit M, class Unit>
std::unique_ptr<OptimizationUnit<N, M>>static inline make_unit() {
	return std::unique_ptr<OptimizationUnit<N, M>>(dynamic_cast<OptimizationUnit<N, M>*>(new Unit));
}

template<hanoi_limit N, hanoi_limit M, bool parallel>
class OptimizationPacket {
public:
	void iterate(frame_moves* moves, const Frame<N,M>& frame) {
		++count_all_frames;
		for (auto& m_unit : m_units) 
			m_unit->optimize(moves, frame);
	}
	void dumpScoreOptimization() {
		std::cout << "Efficiency coficent: \n";
		uint64_t count_skipped_moves = 0;
		for (auto& m_unit : m_units) {
			count_skipped_moves += m_unit->all_count_of_remove;
			std::cout << "\t" << m_unit->name() << "\t" << m_unit->all_count_of_remove << std::endl;
		}
		std::cout << "Total frames: " << count_all_frames << std::endl;
	}
	OptimizationPacket() : count_all_frames(0) {
		//m_units.push_back(make_unit <N, M, EmptyAnyColumnMove<N, M>>());
		m_units.push_back(make_unit <N, M, AdvantageColumns<N, M>>());
		m_units.push_back(make_unit <N, M, LastStepNoneMoveble<N, M>>());
		m_units.push_back(make_unit <N, M, BasicShiftingRules<N, M>>());
		m_units.push_back(make_unit <N, M, AntiLoopDP<N, M, parallel>>());
		m_units.push_back(make_unit <N, M, EmptyMove<N, M>>());
		m_units.push_back(make_unit <N, M, BadGenerations<N, M, parallel>>());
		m_units.push_back(make_unit <N, M, SimetricMoves<N, M>>());
	}
private:
	std::vector <std::unique_ptr<OptimizationUnit<N,M>>> m_units;
	uint64_t count_all_frames;
};

}
