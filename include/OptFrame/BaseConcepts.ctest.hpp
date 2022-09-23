#ifndef OPTFRAME_BASECONCEPTS_CTEST_HPP
#define OPTFRAME_BASECONCEPTS_CTEST_HPP

#include "BaseConcepts.hpp"

// basic printabilty for std::pair<,>
#include <OptFrame/printable/printable.hpp>

//using namespace optframe; // importing optframe::operator<< stuff

// testing BaseConcepts: compilation tests (these are NOT unit tests!)
// unit tests can be found on tests/ folder
//
// these tests may help understanding deep compilation errors caused by concepts

// ============================

namespace optframe {

// only for <assert> like stuff
#ifndef NDEBUG
// Example of valid struct satisfying solution properties

template<XRepresentation R, class ADS = _ADS>
struct IsSolution
{
   R& getR();
   ADS* getADSptr();

   IsSolution<R, ADS>& clone()
   {
      return *this; // TODO:
   }

   std::string toString() const
   {
      return "";
   }

   friend std::ostream& operator<<(std::ostream& os, const IsSolution<R, ADS>& me)
   {
      os << me.toString();
      return os;
   }
};

// basic test
static_assert(XSolution<IsSolution<int, std::nullptr_t>>);

template<class T>
struct IsEvaluation
{
   using objType = T;
   //
   T& evaluation(); // this is required!

   bool outdated;  // this is required!
   bool estimated; // this is required!

   bool betterStrict(const IsEvaluation<T>& e);
   bool betterNonStrict(const IsEvaluation<T>& e);
   bool equals(const IsEvaluation<T>& e);
   bool isStrictImprovement();
   bool isNonStrictImprovement();

   void update(IsEvaluation<T>& e);                // required ('cost' operation)
   IsEvaluation<T> diff(const IsEvaluation<T>& e); // required ('cost' operation)

   IsEvaluation<T>& clone()
   {
      return *this; // TODO:
   }

   std::string toString() const
   {
      return "";
   }

   friend std::ostream& operator<<(std::ostream& os, const IsEvaluation<T>& me)
   {
      os << me.toString();
      return os;
   }
};

// basic test
static_assert(XSolution<IsEvaluation<int>>);

template<class T>
struct IsESolution
{
   // declaring types
   using first_type = IsESolution<T>;
   using second_type = IsEvaluation<T>;

   //T& evaluation();
   IsESolution<T>& first{ *this };
   IsEvaluation<T> second;

   IsESolution<T>& clone()
   {
      return *this; // TODO:
   }

   std::string toString() const
   {
      return "";
   }

   friend std::ostream& operator<<(std::ostream& os, const IsESolution<T>& me)
   {
      os << me.toString();
      return os;
   }
};

// basic test
static_assert(XSolution<IsESolution<int>::first_type>);
// debugging error on XEvaluation for IsEvaluation<int>
static_assert(optframe::evgoal<IsEvaluation<int>>);
static_assert(HasClone<IsEvaluation<int>>);
static_assert(HasToString<IsEvaluation<int>>);
static_assert(HasGetObj<IsEvaluation<int>>);
static_assert(optframe::ostreamable<IsEvaluation<int>>);
static_assert(XEvaluation<IsESolution<int>::second_type>); // IsEvaluation<T>
static_assert(XESolution<IsESolution<int>>);

// =======================================

// test BaseSolution concept
template<XBaseSolution<double> S>
class TestBaseSol
{
public:
};

template<XRepresentation R, XRSolution<R> XRS>
class TestXRSolution
{
public:
};

template<XSolution XS>
class TestXSol
{
public:
};

// test evaluation concept
template<XEvaluation XE>
class TestEv
{
public:
};

template<XESolution XES>
class TestXESol
{
public:
};

// compile tests
static_assert(XBaseSolution<IsSolution<double>, double>); //TestBaseSol<IsSolution<double>> test;
static_assert(XSolution<IsSolution<double>>);             // TestXSol<IsSolution<double>> testXsol;
static_assert(XRSolution<IsSolution<double>, double>);    //TestXRSolution<double, IsSolution<double>> testxrs;
static_assert(XEvaluation<IsEvaluation<int>>);            // TestEv<IsEvaluation<short>> testev;
//static_assert(XEvaluation<IsEvaluation<short>>); // TestEv<IsEvaluation<short>> testev; // won't work wth 'short' on C++20
// XESolution tests
static_assert(XESolution<IsESolution<double>>); // TestXESol<IsESolution<double>> testXesol;

struct optframe_test_debug_testsol_issolution_disable_runtime
{
   /*
// test if following structure is valid
TestBaseSol<IsSolution<double>> test;
TestXSol<IsSolution<double>> testXsol;
TestXRSolution<double, IsSolution<double>> testxrs;
TestEv<IsEvaluation<short>> testev;
// XESolution tests
TestXESol<IsESolution<double>> testXesol;
*/
   void fprint()
   {
      // testing pair printability
      std::pair<int, int> pp1;
      std::cout << pp1 << std::endl;
      IsSolution<double> issol;
      std::cout << issol << std::endl;
      IsEvaluation<double> isev;
      std::cout << isev << std::endl;
      std::pair<IsSolution<double>, IsEvaluation<double>> pp;
      std::cout << pp << std::endl;
   }
   //TestXESol<std::pair<IsSolution<double>,IsEvaluation<double>>> testXesol_pair;
};

// =========================================================

// test class for X2ES (MultiESolution Space 2^S)
template<XESolution XES, X2ESolution<XES> X2ES>
class TestPopOrPareto
{
public:
   // nothing required here
};

// ----------------

// example of a Population element
template<XESolution XES>
struct IsPopulation
{
   size_t size();     // required
   XES& at(size_t i); // required
};

// compilation test
static_assert(X2ESolution<IsPopulation<IsESolution<double>>, IsESolution<double>>); //TestPopOrPareto<IsSolution<double>, IsEvaluation<int>, IsPopulation<IsSolution<double>,IsEvaluation<int>> > testLocal1;

// ---------

// example of a Pareto element
template<XESolution XES>
struct IsPareto
{
   size_t size();     // required
   XES& at(size_t i); // required
};

// compilation test
static_assert(X2ESolution<IsPareto<IsESolution<double>>, IsESolution<double>>); //TestPopOrPareto<IsSolution<double>, IsEvaluation<int>, IsPopulation<IsSolution<double>,IsEvaluation<int>> > testLocal1;

// perform instantiation tests
struct optframe_test_debug_testpareto_ispop_ispareto_disable_runtime
{
   TestPopOrPareto<IsESolution<double>, IsPopulation<IsESolution<double>>> testLocal1;
   TestPopOrPareto<IsESolution<double>, IsPareto<IsESolution<double>>> testLocal2;
};

#endif // NDEBUG clause

} // namespace optframe

#endif // OPTFRAME_BASECONCEPTS_CTEST_HPP