#pragma once

#include <cstdint>
#include <type_traits>

namespace optframe {

//enum class SearchStatus : int
enum class SearchStatus : uint8_t
{
   // ----------------------
   // General Method Reports
   // ----------------------
   //
   NO_REPORT = 0x00, // no information to give (no good or bad news)
   // this flag should only be used when no other situation applies.
   //
   FAILED = 0x01, // fail flag: this is meant only for non-intended stops, i.e., crashes
   // if this flag IS NOT set, it means everything was fine.
   // if this flag IS set, no other flag should be used.
   //
   // -------------
   // Stop Criteria
   // -------------
   STOP_BY_TIME = 0x02,  // stop by timelimit
   //
   STOP_BY_MEMORY = 0x04,  // stop by memory limit: intended stops only! out-of-memory errors should use FAILED flag only.
   // if stop by time/memory is not set, it means method stopped by any 
   // other parameter: target, evaluation count, population count, ...
   // user must verify information logs in this case
   //
   // ---------------
   // Problem Status
   // ---------------
   // problem is infeasible (or unbounded)
   IMPOSSIBLE = 0x08, // or INFEASIBLE
   // if this flag is not set, it means problem is FEASIBLE / POSSIBLE, or unknown to solver.
   //
   // ----------------
   // Solution Status
   // ----------------
   //
   NO_SOLUTION = 0x10, // informs that no solution has been found (problem may or may not be impossible)
   // if this flag is not set, some solution is expected as return
   //
   IMPROVEMENT = 0x20, // strict improvement flag
   // this denotes ANY STRICT evolution towards the objective direction, or multi-direction ("side moves" should not trigger this).
   // this should also be set when you had no solution, and now you have! (it's an improvement, right?)
   // if this flag is NOT set, it means no improvement has been found (no evolution).
   // this flag SHOULD still be used when necessary, sometimes with or without LOCAL_OPT / GLOBAL_OPT flags.
   //
   LOCAL_OPT = 0x40, // local optimum (if not set, no information is given). 
   // this flag is NOT required when it's GLOBAL_OPT (but method designer may also provide this information, if useful)
   //
   GLOBAL_OPT = 0x80 // global optimum (if not set, it may be LOCAL_OPT)
   //
   // TODO: do we need more? which? 
   // ... LOWER_BOUND / UPPER_BOUND ? How to allow represent only evaluation and not include solution? TODO (IGOR): think... maybe some specific BoundSearch or RelaxationSearch ??..
   // ... sometimes, good also to result solution, and approximation value provided (ApproxSearch) approximative search (route + value + ~1.5 bound)
   // ... finally, maybe good to add Bound to input/output search format... return bound value or approximation in other param (with empty solution)
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