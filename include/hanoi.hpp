#pragma once
#include "task_queue.hpp"
#include "optimization.hpp"
#include <atomic>


template<hanoi_limit N, hanoi_limit M>
class Hanoi {
public:
	//initial
	Hanoi() : is_result_promised( false ) {};

	void run() {
		while (true) {
			Frame<N,M> frame = RecursiveQueue<N,M>::Instance().pop();
			frame.dumpData();
			frame_moves moves(make_basic_moves<N>());
			OptimizationPacket<N,M>::Instance().iterate(&moves, frame);
			if (frame.dumpEnd() || is_result_promised.load()) {
				is_result_promised.store(true);
				return;
			}
			frame.acceptMoves(&moves);
			while (!frame.isEndIterate())
				RecursiveQueue<N,M>::Instance().push(frame.generateNext());
		}
	}
private:
	std::atomic<bool> is_result_promised;
};