#pragma once

#include <limits>
#include <concepts>
#include <type_traits>
#include <fmt/core.h>

#define TOKEN_CONCAT_IMPL(x, y) x ## y
#define TOKEN_CONCAT(x, y) TOKEN_CONCAT_IMPL(x, y)
#define ANONYMOUS_VARIABLE(prefix) TOKEN_CONCAT(prefix, __COUNTER__)

namespace vc::util {

    template<size_t Bits, typename Output>
    constexpr Output signExtend(auto value) {
        if ((value & (1 << (Bits - 1))) == 0)
            return value;
        else {
            std::make_unsigned_t<Output> mask = std::numeric_limits<Output>::max() << Bits;
            return value | mask;
        }
    }

    #define SCOPE_GUARD ::vc::util::scope_guard::ScopeGuardOnExit() + [&]()
    #define ON_SCOPE_EXIT auto ANONYMOUS_VARIABLE(SCOPE_EXIT_) = SCOPE_GUARD

    namespace scope_guard {

        template<class F>
        class ScopeGuardImpl {
        private:
            F m_func;
            bool m_active;
        public:
            constexpr ScopeGuardImpl(F func) : m_func(std::move(func)), m_active(true) { }
            ~ScopeGuardImpl() { if (this->m_active) { this->m_func(); } }

            void release() { this->m_active = false; }

            ScopeGuardImpl(ScopeGuardImpl &&other) noexcept : m_func(std::move(other.m_func)), m_active(other.m_active) {
                other.cancel();
            }

            ScopeGuardImpl& operator=(ScopeGuardImpl &&) = delete;
        };

        enum class ScopeGuardOnExit { };

        template <typename F>
        constexpr ScopeGuardImpl<F> operator+(ScopeGuardOnExit, F&& f) {
            return ScopeGuardImpl<F>(std::forward<F>(f));
        }

    }

    namespace detail {
        template<std::size_t...Is, class Model>
        auto build_array_impl(std::index_sequence<Is...>, Model&& model)
        {
            constexpr auto size = sizeof...(Is) + 1;
            return std::array<std::decay_t<Model>, size>
                    {
                            // N-1 copies
                            (Is, model)...,

                            // followed by perfect forwarding for the last one
                            std::forward<Model>(model)
                    };
        }
    }

    template<std::size_t N, class Type>
    auto build_array(std::integral_constant<std::size_t, N>, Type&& model) {
        return detail::build_array_impl(std::make_index_sequence<N-1>(),
                                        std::forward<Type>(model));
    }

}

constexpr auto operator ""_kiB(unsigned long long kiB) {
    return kiB * 1024;
}

constexpr auto operator ""_MiB(unsigned long long MiB) {
    return operator ""_kiB(MiB) * 1024;
}

constexpr auto operator ""_GiB(unsigned long long GiB) {
    return operator ""_MiB(GiB) * 1024;
}