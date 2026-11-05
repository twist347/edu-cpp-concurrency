#pragma once

#include <system_error>
#include <utility>

#include "lock_tags.hpp"

namespace edu {
	template<typename Mutex>
	class unique_lock {
	public:
		using mutex_type = Mutex;

		unique_lock() noexcept = default;

		unique_lock(mutex_type &mutex) : mutex_{&mutex} {
			mutex_->lock();
			owns_ = true;
		}

		unique_lock(mutex_type &mutex, adopt_lock_t) : mutex_{&mutex}, owns_{true} {}
		unique_lock(mutex_type &mutex, defer_lock_t) : mutex_{&mutex}, owns_{false} {}
		unique_lock(mutex_type &mutex, try_to_lock_t) : mutex_{&mutex}, owns_{mutex_->try_lock()} {}

		unique_lock(const unique_lock &) = delete;
		unique_lock &operator=(const unique_lock &) = delete;

		unique_lock(unique_lock &&other) noexcept : mutex_{std::exchange(other.mutex_, nullptr)}, owns_{std::exchange(other.owns_, false)} {}

		unique_lock &operator=(unique_lock &&other) noexcept {
			if (this == &other) {
				return *this;
			}
			if (owns_) {
				mutex_->unlock();
			}
			mutex_ = std::exchange(other.mutex_, nullptr);
			owns_ = std::exchange(other.owns_, false);

			return *this;
		}

		~unique_lock() {
			if (owns_) {
				mutex_->unlock();
			}
		}

		void lock() {
			check_is_m();
			check_is_owns();
			mutex_->lock();
			owns_ = true;
		}

		bool try_lock() {
			check_is_m();
			check_is_owns();
			owns_ = mutex_->try_lock();
			return owns_;
		}

		void unlock() {
			check_is_not_owns();
			mutex_->unlock();
			owns_ = false;
		}

	private:
		void check_is_m() const {
			if (!mutex_) {
				throw std::system_error{
					std::make_error_code(std::errc::operation_not_permitted)
				};
			}
		}

		void check_is_owns() const {
			if (owns_) {
				throw std::system_error{
					std::make_error_code(std::errc::resource_deadlock_would_occur)
				};
			}
		}

		void check_is_not_owns() const {
			if (!owns_) {
				throw std::system_error{
					std::make_error_code(std::errc::operation_not_permitted)
				};
			}
		}

		mutex_type *mutex_ = nullptr;
		bool owns_ = false;
	};
};
