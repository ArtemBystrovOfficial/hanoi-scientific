#include "../include/hanoi.hpp"

#include <boost/lockfree/queue.hpp>

#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <bitset>

int main() {
    Hanoi<4, 14> hanoi;
	auto start = std::chrono::steady_clock::now();
#ifdef PARALLEL_MODE
	std::vector<std::thread> ths(std::thread::hardware_concurrency());
	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
		ths[i] = std::thread([&]() { hanoi.run(); });
	for (auto& th : ths)
		th.join();
#else
	hanoi.run();
#endif
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time execution: " << std::chrono::duration <double, std::milli>(end - start).count() / 1000 << "s";
}