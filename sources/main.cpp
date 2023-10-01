#include "../include/hanoi.hpp"

#include <boost/lockfree/queue.hpp>

#include <thread>
#include <vector>
#include <iostream>

int main() {
    Hanoi<3, 4> hanoi;
	std::vector<std::thread> ths(std::thread::hardware_concurrency());
	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
		ths[i] = std::thread([&]() { hanoi.run(); });
	for (auto & th : ths) 
		th.join();

	//Frame<4, 9> frame;
	//frame.drawData();
	//int x=0, y=1;
	//while (true) {
	//	std::cin >> x >> y;
	//	frame = frame.generateNext(x,y);
	//	frame.dumpData();
	//}
}