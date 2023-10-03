#include "../include/hanoi.hpp"
#include <boost/lockfree/queue.hpp>



int main() {
	hanoi::singleRun<4, 3>();
	hanoi::singleRun<4, 5>();
	hanoi::singleRun<4, 8>();
	hanoi::singleRun<4, 12>();
	hanoi::singleRun<4, 14>();

	hanoi::singleRun<5, 3>();
	hanoi::singleRun<5, 5>();
	hanoi::singleRun<5, 8>();
	hanoi::singleRun<5, 12>();

	hanoi::singleRun<6, 5>();
	hanoi::singleRun<6, 7>();
	hanoi::singleRun<6, 10>();
}