#pragma once

#include "lock_tags.hpp"

namespace edu {
    template<typename Mutex>
    class lock_guard {
    public:
        using mutex_type = Mutex;

        explicit lock_guard(mutex_type &mutex) : mutex_{mutex} {
            mutex_.lock();
        }

        lock_guard(mutex_type &mutex, adopt_lock_t) : mutex_{mutex} {}

        lock_guard(const lock_guard &) = delete;
        lock_guard &operator=(const lock_guard &) = delete;

        lock_guard(lock_guard &&) noexcept = delete;
        lock_guard &operator=(lock_guard &&) noexcept = delete;

        ~lock_guard() {
            mutex_.unlock();
        }

    private:
        mutex_type &mutex_;
    };
}
