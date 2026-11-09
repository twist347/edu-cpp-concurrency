#include <future>
#include <mutex>

#include <doctest/doctest.h>

#include <edu/lock.hpp>

#include "mutex_probe.hpp"

TEST_CASE("lock locks two mutexes") {
	std::mutex first;
	std::mutex second;

	edu::lock(first, second);

	CHECK_FALSE(try_lock_from_another_thread(first));
	CHECK_FALSE(try_lock_from_another_thread(second));

	first.unlock();
	second.unlock();
	CHECK(try_lock_from_another_thread(first));
	CHECK(try_lock_from_another_thread(second));
}

TEST_CASE("lock locks more than two mutexes") {
	std::mutex first;
	std::mutex second;
	std::mutex third;

	edu::lock(first, second, third);

	CHECK_FALSE(try_lock_from_another_thread(first));
	CHECK_FALSE(try_lock_from_another_thread(second));
	CHECK_FALSE(try_lock_from_another_thread(third));

	first.unlock();
	second.unlock();
	third.unlock();
}

TEST_CASE("lock avoids deadlock with opposite acquisition order") {
	std::mutex first;
	std::mutex second;

	auto first_task = std::async(std::launch::async, [&] {
		for (int i = 0; i < 100; ++i) {
			edu::lock(first, second);
			first.unlock();
			second.unlock();
		}
	});
	auto second_task = std::async(std::launch::async, [&] {
		for (int i = 0; i < 100; ++i) {
			edu::lock(second, first);
			second.unlock();
			first.unlock();
		}
	});

	CHECK_NOTHROW(first_task.get());
	CHECK_NOTHROW(second_task.get());
	CHECK(try_lock_from_another_thread(first));
	CHECK(try_lock_from_another_thread(second));
}
