#include "../include/hanoi.hpp"
#include <boost/lockfree/queue.hpp>

int main() {
	hanoi::singleRun<4, 5>();
}