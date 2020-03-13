#pragma once

#include <cstdint>
#include <type_traits>

namespace optframe {

//enum class SearchStatus : int
enum class SearchStatus : uint8_t
{
   // better to keep a single flag '0x00' that indicates nothing good happened (no evolution)
   // it means: no improvement on current solution or no valid/feasible solution found
   //NO_CHANGE = 0x00,
   NO_NEWSOL = 0x00, // no (new) solution found on search.. rename to NOTHING, or NONE... for no information.
   // TODO: rename to UNCHANGED (?). 'NO_CHANGE' was already bad... 'NO_PROGRESS' (?) maybe.

   // general flag
   VALID_SOL = 0x01, // VALID_SOLUTION (?) nothing interesting to say...
   // TODO: rename 'VALID_SOL' to 'HEURISTIC' (general return, since all working solutions are valid).
   //          Better than 'APPROXIMATED' naming, which conflicts to 'APPROXIMATIVE'.
   // improvement flag
   IMPROVEMENT = 0x02,
   // stop criteria (general)
   STOP_BY_TIME = 0x04,  // stop by timelimit
   STOP_BY_OTHER = 0x08, // stop by specific method parameter (target, evaluation count, population count, ..., view logs)
   //STOP_BY_TARGET = 0x10, // stop by target value
   //STOP_BY_MEMORY = 0x10, // stop by memory (useful for branch&bound and other enumeration techniques)

   // other flags (SEARCH TERMINATION CONDITIONS)
   // problem is infeasible (or unbounded)
   IMPOSSIBLE = 0x20, // TODO: rename to 'INFEASIBLE' (?)
   // local optima
   // global optima
   // ... LOWER_BOUND / UPPER_BOUND ? How to allow represent only evaluation and not include solution? TODO (IGOR): think... maybe some specific BoundSearch or RelaxationSearch ??..
   // ... sometimes, good also to result solution, and approximation value provided (ApproxSearch) approximative search (route + value + ~1.5 bound)
   // ... finally, maybe good to add Bound to input/output search format... return bound value or approximation in other param (with empty solution)
   LOCAL_OPT = 0x40, // local optimum
   GLOBAL_OPT = 0x80 // global optimum
};

// advice from: https://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c
// not working yet...

/*
template<typename T = typename std::enable_if<std::is_enum<T>::value, T>::type>
class auto_bool
{
    T val_;
public:
    constexpr auto_bool(T val) : val_(val) {}
    constexpr operator T() const { return val_; }
    constexpr explicit operator bool() const
    {
        return static_cast<std::underlying_type_t<T>>(val_) != 0;
    }
};

template <typename T = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr auto_bool<T> operator&(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) &
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

template <typename T = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator|(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) |
        static_cast<typename std::underlying_type<T>::type>(rhs));
}
*/

// manual solution

inline constexpr SearchStatus
operator|(SearchStatus a, SearchStatus b)
{
   return static_cast<SearchStatus>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline constexpr SearchStatus operator&(SearchStatus a, SearchStatus b)
{
   return static_cast<SearchStatus>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline bool operator!(SearchStatus e)
{
   return e == static_cast<SearchStatus>(0);
}

} // namespace optframe