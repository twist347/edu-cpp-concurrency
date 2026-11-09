#pragma once

#include <mutex>
#include <thread>

namespace edu::detail {
    template<typename L1, typename... Ln>
    bool try_lock_all(L1 &l1, Ln &... ln) {
        std::unique_lock lk{l1, std::try_to_lock};

        if (!lk.owns_lock()) {
            return false;
        }

        if constexpr (sizeof...(Ln) != 0) {
            if (!try_lock_all(ln...)) {
                return false;
            }
        }

        lk.release();
        return true;
    }
}

namespace edu {
    template<typename Lockable1, typename Lockable2, typename... LockableN>
    void lock(Lockable1 &lock1, Lockable2 &lock2, LockableN &... lockn) {
        while (true) {
            {
                std::unique_lock lead{lock1};
                if (detail::try_lock_all(lock2, lockn...)) {
                    lead.release();
                    return;
                }
            }
            std::this_thread::yield();
        }
    }
}
