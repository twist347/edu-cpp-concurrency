#include <mutex>

#include <doctest/doctest.h>

#include <edu/scoped_lock.hpp>

#include "mutex_probe.hpp"

TEST_CASE("scoped_lock supports zero mutexes") {
	CHECK_NOTHROW([] {
		edu::scoped_lock<> lock;
		edu::scoped_lock<> adopted{edu::adopt_lock};
	}());
}

TEST_CASE("scoped_lock locks and unlocks one mutex") {
	std::mutex mutex;

	{
		edu::scoped_lock lock{mutex};
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("scoped_lock locks and unlocks multiple mutexes") {
	std::mutex first;
	std::mutex second;
	std::mutex third;

	{
		edu::scoped_lock lock{first, second, third};
		CHECK_FALSE(try_lock_from_another_thread(first));
		CHECK_FALSE(try_lock_from_another_thread(second));
		CHECK_FALSE(try_lock_from_another_thread(third));
	}

	CHECK(try_lock_from_another_thread(first));
	CHECK(try_lock_from_another_thread(second));
	CHECK(try_lock_from_another_thread(third));
}

TEST_CASE("scoped_lock adopts one existing lock") {
	std::mutex mutex;
	mutex.lock();

	{
		edu::scoped_lock lock{edu::adopt_lock, mutex};
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("scoped_lock adopts multiple existing locks") {
	std::mutex first;
	std::mutex second;
	first.lock();
	second.lock();

	{
		edu::scoped_lock lock{edu::adopt_lock, first, second};
		CHECK_FALSE(try_lock_from_another_thread(first));
		CHECK_FALSE(try_lock_from_another_thread(second));
	}

	CHECK(try_lock_from_another_thread(first));
	CHECK(try_lock_from_another_thread(second));
}
