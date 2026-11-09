#include <mutex>

#include <doctest/doctest.h>

#include <edu/lock_guard.hpp>

#include "mutex_probe.hpp"

TEST_CASE("lock_guard locks on construction and unlocks on destruction") {
	std::mutex mutex;

	{
		edu::lock_guard lock{mutex};
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("lock_guard adopts an existing lock") {
	std::mutex mutex;
	mutex.lock();

	{
		edu::lock_guard lock{mutex, edu::adopt_lock};
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}
