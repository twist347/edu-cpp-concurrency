#pragma once

#include <tuple>

#include "lock.hpp"
#include "lock_tags.hpp"

namespace edu {
    template<typename... MutexTypes>
    class scoped_lock {
    public:
        explicit scoped_lock(MutexTypes &... ms) : ms_{ms...} {
            edu::lock(ms...);
        }

        scoped_lock(adopt_lock_t, MutexTypes &... ms) noexcept : ms_{ms...} {}

        scoped_lock(const scoped_lock &) = delete;
        scoped_lock &operator=(const scoped_lock &) = delete;

        scoped_lock(scoped_lock &&) noexcept = delete;
        scoped_lock &operator=(scoped_lock &&) noexcept = delete;

        ~scoped_lock() {
            std::apply([](MutexTypes &... ms) { (ms.unlock(), ...); }, ms_);
        }

    private:
        std::tuple<MutexTypes &...> ms_;
    };

    template<typename Mutex>
    class scoped_lock<Mutex> {
    public:
        using mutex_type = Mutex;

        explicit scoped_lock(mutex_type &m) : m_{m} {
            m_.lock();
        }

        scoped_lock(adopt_lock_t, mutex_type &m) noexcept : m_{m} {}

        scoped_lock(const scoped_lock &) = delete;
        scoped_lock &operator=(const scoped_lock &) = delete;

        scoped_lock(scoped_lock &&) noexcept = delete;
        scoped_lock &operator=(scoped_lock &&) noexcept = delete;

        ~scoped_lock() {
            m_.unlock();
        }

    private:
        mutex_type &m_;
    };

    template<>
    class scoped_lock<> {
    public:
        explicit scoped_lock() = default;
        explicit scoped_lock(adopt_lock_t) noexcept {}

        scoped_lock(const scoped_lock &) = delete;
        scoped_lock &operator=(const scoped_lock &) = delete;

        scoped_lock(scoped_lock &&) noexcept = delete;
        scoped_lock &operator=(scoped_lock &&) noexcept = delete;

        ~scoped_lock() = default;
    };

    template<typename... MutexTypes>
    scoped_lock(MutexTypes &...) -> scoped_lock<MutexTypes...>;
}
