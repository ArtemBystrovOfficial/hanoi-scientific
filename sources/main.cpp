#include "../include/hanoi.hpp"
#include <boost/lockfree/queue.hpp>

int main() {
	auto b = hanoi::singleRun<5, 15>();
}
