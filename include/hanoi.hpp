#pragma once
#include "task_queue.hpp"
#include "optimization.hpp"
#include <atomic>


template<hanoi_limit N, hanoi_limit M>
class Hanoi {
public:
	//initial
	Hanoi() {};

	void run() {
		while (true) {
			Frame<N,M> frame = RecursiveQueue<N,M>::Instance().pop();
			//frame.dumpData();
			frame_moves moves(make_basic_moves<N>());
			OptimizationPacket<N,M>::Instance().iterate(&moves, frame);
			if (frame.dumpEnd() 
#ifdef PARALLEL_MODE
				|| is_result_promised.load()
#endif	
				) {
				break;
			}
			frame.acceptMoves(&moves);
			while (!frame.isEndIterate())
				RecursiveQueue<N,M>::Instance().push(frame.generateNext());
		}
#ifdef PARALLEL_MODE
		if(!is_result_promised.exchange(true))
#endif
			OptimizationPacket<N, M>::Instance().dumpScoreOptimization();
	}
private:
#ifdef PARALLEL_MODE
	std::atomic<bool> is_result_promised { false };
#endif
};