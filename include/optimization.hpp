#pragma once
#include "optimization_classes.hpp"
#include <memory>
#include <vector>
//SINGLETON
template<hanoi_limit N, hanoi_limit M>
class OptimizationPacket {
public:
	static OptimizationPacket<N,M>& Instance() {
		static OptimizationPacket<N,M> packet;
		return packet;
	}
	void iterate(frame_moves* moves, const Frame<N,M>& frame) {
		for (auto& m_unit : m_units) 
			m_unit->optimize(moves, frame);
	}
private:
	OptimizationPacket() {
		m_units.push_back(std::unique_ptr<OptimizationUnit<N,M>>(dynamic_cast<OptimizationUnit<N,M>*>(new A<N,M>)));
		m_units.push_back(std::unique_ptr<OptimizationUnit<N,M>>(dynamic_cast<OptimizationUnit<N,M>*>(new B<N,M>)));
	}

	std::vector <std::unique_ptr<OptimizationUnit<N,M>>> m_units;
};