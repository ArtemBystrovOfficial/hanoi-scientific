#pragma once

#include "structs.hpp"

template<hanoi_limit N, hanoi_limit M>
class OptimizationUnit {
public:
	virtual void optimize(frame_moves<N>* moves, const Frame<N,M>& frame) = 0;
};

template<hanoi_limit N, hanoi_limit M>
class A : public OptimizationUnit<N,M> {
public:
	void optimize(frame_moves<N>* moves, const Frame<N,M>& frame) override {
	}
};

template<hanoi_limit N, hanoi_limit M>
class B : public OptimizationUnit<N,M> {
public:
	void optimize(frame_moves<N>* moves, const Frame<N,M>& frame) override {
	}
};