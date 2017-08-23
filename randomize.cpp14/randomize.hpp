/**
 * Uses the random and chrono C++11 libraries to generate
 * random values. This is simply syntaxic sugar on top of
 * the STL facilities.
 */

#ifndef randomize_h
#define randomize_h

#include <chrono>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>
#include <unordered_map>

namespace randomize {
    namespace details {
        /**
         * It is a pity, but there is no support
         * for std::hash<std::pair<T1, T2>> in C++14
         * when std::hash is defined for both T1 and 
         * T2 so every library has to do it on its own.
         * If you have Boost in your prerequisite, you
         * just need to make sure to include 
         * boost/functional/hash/pair.hpp before this
         * file so that it uses it.
         * If Boost is not used, a simple hash function
         * is provided.
         * Warning: the default function may overflow so
         * you need to use Boost for a production usage.
         */
        template <typename T>
        struct hash_pair {
            std::size_t operator()(const std::pair<T, T>& p) const {
                #if defined(BOOST_FUNCTIONAL_HASH_PAIR_HPP)
                    return boost::hash_value(p);
                #else
                    // Reference: Algorithms, 4th edition (Sedgewick, Wayne), p.462
                    std::size_t hash{17};
                    hash = hash * 31 + p.first;
                    hash = hash * 31 + p.second;
                    return (hash & 0x7fffffff) % 100003;
                #endif
            }
        };
        
        /**
         * Range structure for min/max type management.
         */
        template <typename T, typename TEnable = void>
        struct range {};
        
        /**
         * The default range for an integral type is simply
         * its numeric limits.
         */
        template <typename T>
        struct range<T, std::enable_if_t<std::is_integral<T>::value>> {
            using type = T;
            using value_type = T;
            
            static constexpr const T actual_min = std::numeric_limits<T>::min();
            static constexpr const T actual_max = std::numeric_limits<T>::max();
            
            static constexpr const T min = std::numeric_limits<T>::min();
            static constexpr const T max = std::numeric_limits<T>::max();
        };
        
        /**
         * Template parameters do not support floating-point
         * numbers. Hence, its range is limited to the long long type.
         */
        template <typename T>
        struct range<T, std::enable_if_t<std::is_floating_point<T>::value>> {
            using type = T;
            using value_type = long long;
            
            static constexpr const T actual_min = std::numeric_limits<T>::min();
            static constexpr const T actual_max = std::numeric_limits<T>::max();
            
            // Limitation: if you need a larger range, use the function parameter version
            static constexpr const value_type min = std::numeric_limits<value_type>::min();
            static constexpr const value_type max = std::numeric_limits<value_type>::max();
        };
        
        /**
         * Generate a unique seed based on the current time of day.
         * @return - a seed to feed to a random number generator.
         */
        unsigned gen_seed() noexcept {
            using seed_time_t = std::chrono::high_resolution_clock;
            auto seed_time = seed_time_t::now().time_since_epoch().count();
            return static_cast<unsigned>(seed_time);
        }
        
        /**
         * Uniform distribution for integral types, with min and max
         * passed as template parameters.
         * @return - the uniform distribution.
         */
        template <
            typename T,
            T min = range<T>::min,
            T max = range<T>::max,
            typename std::enable_if<std::is_integral<T>::value, int>::type = 0
        >
        auto uniform_distribution() noexcept {
            return std::uniform_int_distribution<T>{min, max};
        }
        
        /**
         * Uniform distribution for floating point types, with min and max
         * passed as template parameters.
         * @return - the uniform distribution.
         */
        template <
            typename T,
            long long min = range<T>::min,
            long long max = range<T>::max,
            typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0
        >
        auto uniform_distribution() noexcept {
            return std::uniform_real_distribution<T>{static_cast<T>(min), static_cast<T>(max)};
        }
        
        /**
         * Uniform distribution for integral types, with min and max
         * passed as function parameters.
         * @return - the uniform distribution.
         */
        template <
            typename T,
            typename std::enable_if<std::is_integral<T>::value, int>::type = 0
        >
        auto uniform_distribution(T min, T max) noexcept {
            return std::uniform_int_distribution<T>{min, max};
        }
        
        /**
         * Uniform distribution for floating point types, with min and max
         * passed as function parameters.
         * @return - the uniform distribution.
         */
        template <
            typename T,
            typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0
        >
        auto uniform_distribution(T min, T max) noexcept {
            return std::uniform_real_distribution<T>{min, max};
        }
        
        /**
         * Implementation of random number generation with min and
         * max as template parameters.
         * @return - random number in the range [min, max].
         */
        template <
            typename T,
            typename range<T>::value_type min = range<T>::min,
            typename range<T>::value_type max = range<T>::max
        >
        auto rand_impl() noexcept {
            static unsigned seed_time{gen_seed()};
            static auto generator = uniform_distribution<T, min, max>();
            static auto engine = std::mt19937_64{seed_time};
            return generator(engine);
        }
        
        /**
         * Implementation of random number generation with min and
         * max as function parameters. This version uses memoization
         * technique and actually returns a function.
         * @return - random number function in the range [min, max].
         */
        template <typename T>
        auto get_rand_impl(T min, T max) noexcept {
            static unsigned seed_time{gen_seed()};
            static auto engine = std::mt19937_64{seed_time};
            
            using distrib_type = decltype(uniform_distribution<T>(min, max));
            using gen_hashmap = std::unordered_map<std::pair<T, T>, distrib_type, hash_pair<T>>;
            static auto generators = gen_hashmap{};
            
            const auto key = std::make_pair(min, max);
            generators.emplace(key, uniform_distribution<T>(min, max));
            
            auto f = [key] {
                return generators[key](engine);
            };
            
            return f;
        }
        
        /**
         * Implementation of random number generation with min and
         * max as function parameters.
         * @return - random number in the range [min, max].
         */
        template <typename T>
        auto rand_impl(T min, T max) noexcept {
            auto f = get_rand_impl(min, max);
            return f();
        }
    }
    
    /**
     * Random number generation with min and
     * max as function parameters.
     * @return - random number in the range [min, max].
     */
    template <typename T>
    auto rand(T min, T max) noexcept {
        static_assert(std::is_arithmetic<T>::value, "the provided type must be arithmetic");
        return details::rand_impl<T>(min, max);
    }

    /**
     * Random number generation with min and
     * max as function parameters.
     * @return - random number function in the range [min, max].
     */
    template <typename T>
    auto get_rand(T min, T max) noexcept {
        static_assert(std::is_arithmetic<T>::value, "the provided type must be arithmetic");
        return details::get_rand_impl<T>(min, max);
    }
    
    /**
     * Random number generation with min and
     * max as template parameters.
     * @return - random number in the range [min, max].
     */
    template <
        typename T,
        typename details::range<T>::value_type min = details::range<T>::min,
        typename details::range<T>::value_type max = details::range<T>::max
    >
    auto rand() noexcept {
        static_assert(std::is_arithmetic<T>::value, "the provided type must be arithmetic");
        return details::rand_impl<T, min, max>();
    }
}

#endif
