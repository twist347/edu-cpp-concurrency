#pragma once

namespace edu {
    struct adopt_lock_t {};
    constexpr adopt_lock_t adopt_lock{};

    struct defer_lock_t {};
    constexpr defer_lock_t defer_lock{};

    struct try_to_lock_t {};
    constexpr try_to_lock_t try_to_lock{};
};
