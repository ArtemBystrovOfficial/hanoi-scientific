#include "../include/hanoi.hpp"

#include <boost/lockfree/queue.hpp>

#include <thread>
#include <vector>

int main() {
    	Hanoi<5, 7> hanoi;
	std::vector<std::thread> ths(std::thread::hardware_concurrency());
	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
		ths[i] = std::thread([&]() { hanoi.run(); });
	for (auto & th : ths) 
		th.join();
}