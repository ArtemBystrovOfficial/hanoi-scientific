#define BOOST_TEST_MODULE My Test 
#include <boost/test/included/unit_test.hpp> 
#include <boost/lockfree/queue.hpp>
#include <hanoi.hpp>

BOOST_AUTO_TEST_CASE(TEST_4_1) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 1>()), 1);
}

BOOST_AUTO_TEST_CASE(TEST_4_2) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 2>()), 3);
}

BOOST_AUTO_TEST_CASE(TEST_4_3) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 3>()), 5);
}

BOOST_AUTO_TEST_CASE(TEST_4_4) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 4>()), 9);
}

BOOST_AUTO_TEST_CASE(TEST_4_5) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 5>()), 13);
}

BOOST_AUTO_TEST_CASE(TEST_4_6) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 6>()), 17);
}

BOOST_AUTO_TEST_CASE(TEST_4_7) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 7>()), 25);
}

BOOST_AUTO_TEST_CASE(TEST_4_8) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 8>()), 33);
}

BOOST_AUTO_TEST_CASE(TEST_4_9) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 9>()), 41);
}

BOOST_AUTO_TEST_CASE(TEST_4_11) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 11>()), 65);
}

BOOST_AUTO_TEST_CASE(TEST_4_12) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 12>()), 81);
}

BOOST_AUTO_TEST_CASE(TEST_4_13) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 13>()), 97);
}

BOOST_AUTO_TEST_CASE(TEST_4_14) {
	BOOST_CHECK_EQUAL((hanoi::singleRun<4, 14>()), 113);
}
