#include <iostream>

// https://zh.cppreference.com/w/cpp/concepts/same_as
// c++20

/*
namespace detail {
    template< class T, class U >
    concept SameHelper = std::is_same_v<T, U>;
}
template< class T, class U >
concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;
*/

template<class Me, class Other>
concept bool my_same_as = std::is_same_v<Me, Other>&& std::is_same_v<Other, Me>;

// https://en.cppreference.com/w/cpp/concepts/convertible_to
/*
  template <class From, class To>
concept convertible_to =
  std::is_convertible_v<From, To> &&
  requires(std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };
  */
template<class From, class To>
concept bool my_convertible_to =
  std::is_convertible_v<From, To>&& requires(std::add_rvalue_reference_t<From> (&f)())
{
   static_cast<To>(f());
};

// https://stackoverflow.com/questions/60358244/implementing-a-concept-for-pair-with-c20-concepts

// Pair1 only works on C++17 (not C++20). It explores a GCC bug! So it is commented here!
/*
template<typename _pair>
concept bool Pair1 = requires(_pair p)
{
   {
      p.first
   }
   ->typename _pair::first_type; // c++20 error: return-type-requirement is not a type-constraint

   {
      p.second
   }
   ->typename _pair::second_type;
};
*/

template<class Me, class Other>
concept bool ref_same_as = std::is_same_v<
  std::remove_reference_t<Me>,
  std::remove_reference_t<Other>>&&
  std::is_same_v<
    std::remove_reference_t<Other>,
    std::remove_reference_t<Me>>;

template<class Me, class Other>
concept bool ref2_same_as = my_same_as<std::remove_reference_t<Me>, std::remove_reference_t<Other>>;

template<typename _pair>
concept bool Pair2 = requires(_pair p)
{
   {
      //std::is_same<std::remove_reference_t<decltype(p.first), typename _pair::first_type>
      std::is_same_v<
        decltype(p.first),
        typename _pair::first_type>
   }
   //->typename _pair::first_type
   ;
   /*
   {
      std::is_same_v<std::remove_reference_t<decltype(p.second)>, _pair::second_type>
   }
   //->typename _pair::second_type
   ;
   */
};

template<typename _pair>
concept bool Pair3 = requires(_pair p)
{
   {
      //std::is_same<std::remove_reference_t<decltype(p.first), typename _pair::first_type>
      //std::is_same_v<
      // decltype(p.first),
      // typename _pair::first_type
      //>
      p.first
   }
   ->my_same_as<typename _pair::first_type>;
}
|| requires(_pair p)
{
   {
      //std::is_same<std::remove_reference_t<decltype(p.first), typename _pair::first_type>
      //std::is_same_v<
      // decltype(p.first),
      // typename _pair::first_type
      //>
      p.first
   }
   //-> my_same_as< typename _pair::first_type& > // GUD
   ->my_same_as<typename _pair::first_type&>;
   /*
   {
      std::is_same_v<std::remove_reference_t<decltype(p.second)>, _pair::second_type>
   }
   //->typename _pair::second_type
   ;
   */
};

template<typename _pair>
concept bool Pair4 = requires(_pair p)
{
   {
      //std::is_same<std::remove_reference_t<decltype(p.first), typename _pair::first_type>
      //std::is_same_v<
      // decltype(p.first),
      // typename _pair::first_type
      //>
      p.first
   }
   ->ref2_same_as<typename _pair::first_type> //ref_same_as or ref2_same_as
     ;
   /*
   {
      std::is_same_v<std::remove_reference_t<decltype(p.second)>, _pair::second_type>
   }
   //->typename _pair::second_type
   ;
   */
};

template<typename P>
concept bool Pair5 = requires(P p)
{
   typename P::first_type;
   typename P::second_type;
   p.first;
   p.second;
   requires my_same_as<decltype(p.first), typename P::first_type>;
   requires my_same_as<decltype(p.second), typename P::second_type>;
};

// Pair6 accepts reference or non-reference local values, works perfect on C++20 (not on c++17)
template<typename P>
concept bool Pair6 = requires(P p)
{
   typename P::first_type;
   typename P::second_type;
   {
      p.first
   }
   ->my_same_as<typename P::first_type&>;
   {
      p.second
   }
   ->my_same_as<typename P::second_type&>;
   //requires my_same_as<decltype(p.first), typename P::first_type>;
   //requires my_same_as<decltype(p.second), typename P::second_type>;
};

template<typename P>
concept bool Pair7 = requires(P p)
{
   typename P::first_type;
   typename P::second_type;
   {
      p.first
   }
   ->my_convertible_to<typename P::first_type>;
   {
      p.second
   }
   ->my_convertible_to<typename P::second_type>;
   //requires my_same_as<decltype(p.first), typename P::first_type>;
   //requires my_same_as<decltype(p.second), typename P::second_type>;
};

template<class T1, class T2>
class MyPair
{
public:
   T1 first;
   T2 second;
   using first_type = T1;
   using second_type = T2;
};

template<class T1, class T2>
class MyNotPair
{
public:
};

using mypair = MyPair<int, double>;
//static_assert(Pair1<mypair>, "MyPair is not Pair1");
static_assert(Pair2<mypair>, "MyPair is not Pair2");
static_assert(Pair3<mypair>, "MyPair is not Pair3");
static_assert(Pair4<mypair>, "MyPair is not Pair4");
static_assert(Pair5<mypair>, "MyPair is not Pair5");
static_assert(Pair6<mypair>, "MyPair is not Pair6"); // ok on c++20, not c++17
static_assert(Pair7<mypair>, "MyPair is not Pair7");

using mynotpair = MyNotPair<int, double>;
//static_assert(!Pair1<mynotpair>, "MyNotPair is Pair1");
static_assert(!Pair2<mynotpair>, "MyNotPair is Pair2");
static_assert(!Pair3<mynotpair>, "MyNotPair is Pair3");
static_assert(!Pair4<mynotpair>, "MyNotPair is Pair4");
static_assert(!Pair5<mynotpair>, "MyNotPair is Pair5");
static_assert(!Pair6<mynotpair>, "MyNotPair is Pair6");
static_assert(!Pair7<mynotpair>, "MyNotPair is Pair7");

using stdpair = std::pair<int, double>;
//static_assert(Pair1<stdpair>, "std::pair is not Pair1");
static_assert(Pair2<stdpair>, "std::pair is not Pair2");
static_assert(Pair3<stdpair>, "std::pair is not Pair3");
static_assert(Pair4<stdpair>, "std::pair is not Pair4");
static_assert(Pair5<stdpair>, "std::pair is not Pair5");
static_assert(Pair6<stdpair>, "std::pair is not Pair6"); // ok on c++20, not c++17
static_assert(Pair7<stdpair>, "std::pair is not Pair7");

template<class T1, class T2>
class MyAltPair
{
private:
   T1 _first;
   T2 _second;

public:
   MyAltPair(T1& t1, T2& t2)
     : first(_first)
     , second(_second)
   {
      first = t1;
      second = t2;
   }

   T1& first;
   T2& second;
   using first_type = T1;
   using second_type = T2;
};

using myaltpair = MyAltPair<int, double>;
//static_assert(Pair1<myaltpair>, "MyPair is not Pair1");
static_assert(Pair2<myaltpair>, "MyAltPair is not Pair2");
static_assert(Pair3<myaltpair>, "MyAltPair is not Pair3");
static_assert(Pair4<myaltpair>, "MyAltPair is not Pair4");
static_assert(!Pair5<myaltpair>, "MyAltPair is Pair5 (should not be)");
static_assert(Pair6<myaltpair>, "MyAltPair is not Pair6 (should be)"); // ok on c++20, not c++17
static_assert(Pair7<myaltpair>, "MyAltPair is not Pair6 (should be)");

int
main()
{
   // Pair1 is commented, as it explores GCC Bug!
   // Pair6 works only on c++20, not on c++17
   // Pair7 looks like a portable solution

   return 0;
}