#include <mutex>
#include <system_error>
#include <utility>

#include <doctest/doctest.h>

#include <edu/unique_lock.hpp>

#include "mutex_probe.hpp"

TEST_CASE("unique_lock default constructor creates an empty lock") {
	edu::unique_lock<std::mutex> lock;

	CHECK_FALSE(lock.owns_lock());
	CHECK_FALSE(static_cast<bool>(lock));
	CHECK(lock.mutex() == nullptr);
}

TEST_CASE("unique_lock locks on construction and unlocks on destruction") {
	std::mutex mutex;

	{
		edu::unique_lock lock{mutex};
		CHECK(lock.owns_lock());
		CHECK(static_cast<bool>(lock));
		CHECK(lock.mutex() == &mutex);
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock supports deferred locking") {
	std::mutex mutex;
	edu::unique_lock lock{mutex, edu::defer_lock};

	CHECK_FALSE(lock.owns_lock());
	CHECK(lock.mutex() == &mutex);
	CHECK(try_lock_from_another_thread(mutex));

	lock.lock();
	CHECK(lock.owns_lock());
	CHECK_FALSE(try_lock_from_another_thread(mutex));

	lock.unlock();
	CHECK_FALSE(lock.owns_lock());
	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock supports try_to_lock") {
	std::mutex mutex;

	{
		edu::unique_lock lock{mutex, edu::try_to_lock};
		CHECK(lock.owns_lock());
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock adopts an existing lock") {
	std::mutex mutex;
	mutex.lock();

	{
		edu::unique_lock lock{mutex, edu::adopt_lock};
		CHECK(lock.owns_lock());
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock move constructor transfers ownership") {
	std::mutex mutex;

	{
		edu::unique_lock source{mutex};
		edu::unique_lock destination{std::move(source)};

		CHECK_FALSE(source.owns_lock());
		CHECK(source.mutex() == nullptr);
		CHECK(destination.owns_lock());
		CHECK(destination.mutex() == &mutex);
		CHECK_FALSE(try_lock_from_another_thread(mutex));
	}

	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock move assignment releases its previous lock") {
	std::mutex first;
	std::mutex second;

	{
		edu::unique_lock first_lock{first};
		edu::unique_lock second_lock{second};
		first_lock = std::move(second_lock);

		CHECK(try_lock_from_another_thread(first));
		CHECK_FALSE(try_lock_from_another_thread(second));
		CHECK(first_lock.owns_lock());
		CHECK(first_lock.mutex() == &second);
		CHECK_FALSE(second_lock.owns_lock());
		CHECK(second_lock.mutex() == nullptr);
	}

	CHECK(try_lock_from_another_thread(second));
}

TEST_CASE("unique_lock release keeps the mutex locked") {
	std::mutex mutex;
	edu::unique_lock lock{mutex};

	auto *released = lock.release();

	CHECK(released == &mutex);
	CHECK_FALSE(lock.owns_lock());
	CHECK(lock.mutex() == nullptr);
	CHECK_FALSE(try_lock_from_another_thread(mutex));

	released->unlock();
	CHECK(try_lock_from_another_thread(mutex));
}

TEST_CASE("unique_lock swap exchanges mutexes and ownership") {
	std::mutex first;
	std::mutex second;
	edu::unique_lock first_lock{first};
	edu::unique_lock second_lock{second, edu::defer_lock};

	swap(first_lock, second_lock);

	CHECK(first_lock.mutex() == &second);
	CHECK_FALSE(first_lock.owns_lock());
	CHECK(second_lock.mutex() == &first);
	CHECK(second_lock.owns_lock());
	CHECK_FALSE(try_lock_from_another_thread(first));
	CHECK(try_lock_from_another_thread(second));
}

TEST_CASE("unique_lock rejects invalid operations") {
	edu::unique_lock<std::mutex> empty;
	CHECK_THROWS_AS(empty.lock(), std::system_error);
	CHECK_THROWS_AS(empty.try_lock(), std::system_error);
	CHECK_THROWS_AS(empty.unlock(), std::system_error);

	std::mutex mutex;
	edu::unique_lock lock{mutex};
	CHECK_THROWS_AS(lock.lock(), std::system_error);
	CHECK_THROWS_AS(lock.try_lock(), std::system_error);

	lock.unlock();
	CHECK_THROWS_AS(lock.unlock(), std::system_error);
}
