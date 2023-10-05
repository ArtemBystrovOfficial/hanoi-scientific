#pragma once
#include "task_queue.hpp"
#include "optimization.hpp"
#include <atomic>
#include <thread>
#include <fstream>

using namespace std::chrono_literals;

namespace hanoi {

template<hanoi_limit N, hanoi_limit M>
class Hanoi {
public:
	//initial
	Hanoi() {};
	uint32_t run() {
#ifndef PARALLEL_MODE
		std::cout << std::string(30, '-') << std::endl;
		std::cout << "N: " << int(N) << " M: " << int(M) << std::endl;
		std::cout << std::string(30, '-') << std::endl;
#endif	
		uint32_t out;
		while (true) {
			Frame<N, M> frame = m_recursive_queue.pop();
			//frame.dumpData();
			frame_moves moves(make_basic_moves<N>());
			m_optimization_packet.iterate(&moves, frame);
			if (frame.dumpEnd()
#ifdef PARALLEL_MODE
				|| is_result_promised.load()
#endif	
				) {
				out = frame.getDepth();
				break;
			}
			frame.acceptMoves(&moves);
			while (!frame.isEndIterate())
				m_recursive_queue.push(frame.generateNext());
		}
#ifdef PARALLEL_MODE
		if (!is_result_promised.exchange(true))
#endif
		m_optimization_packet.dumpScoreOptimization();
		return out * 2 - 1;
	}
private:
	OptimizationPacket<N, M> m_optimization_packet;
	RecursiveQueue<N, M> m_recursive_queue;
#ifdef PARALLEL_MODE
	std::atomic<bool> is_result_promised{ false };
#endif
};

template<hanoi_limit N, hanoi_limit M>
uint32_t singleRun() {
	hanoi::Hanoi<N, M> hanoi;
	auto start = std::chrono::steady_clock::now();
#ifdef PARALLEL_MODE

	std::vector<std::thread> ths(std::thread::hardware_concurrency());
	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
		ths[i] = std::thread([&]() { hanoi.run(); });
	for (auto& th : ths)
		th.join();
#else
	return hanoi.run();
#endif
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time execution: " <<
		std::chrono::duration <double, std::milli>(end - start).count() / 1000 << "s" << std::endl;

}

}