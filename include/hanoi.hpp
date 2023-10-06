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
	Hanoi() {
		initIndex();
	};
	uint32_t run() {
#ifndef PARALLEL_MODE
		std::cout << std::string(30, '-') << std::endl;
		std::cout << "N: " << int(N) << " M: " << int(M) << std::endl;
		std::cout << std::string(30, '-') << std::endl;
#endif	
		uint32_t out;
		while (true) {
			Frame<N, M> frame = m_recursive_queue.pop();

			if constexpr (N==4) //BETA ONLY FOR 4 
				if (isMiddleOptimization(frame))
					continue;
			//frame.dumpData();
			frame_moves moves  = m_is_beta_confirm ? make_basic_moves<N>(m_is_beta_column)
												   : make_basic_moves<N>();
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
	bool isMiddleOptimization(Frame<N, M> &frame) {
		if (m_is_beta_confirm)
			return false;

		for (int i = 1; i < N; i++) 
			if (frame.getColumnSize(i) == m_beta_index) {
				m_is_beta_confirm = true;
				m_is_beta_column = i;
				m_recursive_queue.clear();
				m_recursive_queue.push(std::move(frame));
				break;
			}
		return m_is_beta_confirm;
	}

	void initIndex() {
		int Left = 1;
		int Right = 0;
		for (int i = 1; i < M - 1; i++)
			if (std::pow(2, Right) == Left)
				++Right;
			else
				++Left;
		m_beta_index = Left;
	}

	int32_t m_beta_index = -1;
	bool m_is_beta_confirm = false;
	hanoi_limit m_is_beta_column = 0;

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
	auto out = hanoi.run();
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time execution: " <<
		std::chrono::duration <double, std::milli>(end - start).count() / 1000 << "s" << std::endl;
	return out;
#endif
}

}