#pragma once

#include <future>
#include <mutex>

inline bool try_lock_from_another_thread(std::mutex &mutex) {
	return std::async(std::launch::async, [&mutex] {
		std::unique_lock lk{mutex, std::try_to_lock};
		return lk.owns_lock();
	}).get();
}
