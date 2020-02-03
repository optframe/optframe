#ifndef OPTFRAME_SOLUTION_CONCEPTS_H_
#define OPTFRAME_SOLUTION_CONCEPTS_H_

#include<cstddef> // nullptr_t
#include<string>

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help us!)
typedef std::nullptr_t OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS; // more beautiful :)
typedef OPTFRAME_DEFAULT_ADS _ADS;        // more beautiful :)

// requires Self& clone() method
// there may be some "Cloneable" in std future...
// TODO: change return to unique_ptr instead of reference
template <class Self>
concept bool HasClone = requires(Self self)
{
   {
      self.clone()
   }
   ->Self &;
};

template <class Self>
concept bool HasToString = requires(Self self)
{
   {
      self.toString()
   }
   ->std::string;
};

template <class Self>
concept bool XSolution = HasClone<Self> &&HasToString<Self>;

// ==========================

template <class R>
concept bool Representation = true;

template <class ADS>
concept bool Structure = true;

template <class S, Representation R>
concept bool HasGetR = requires(S a)
{
   {
      a.getR()
   }
   ->R &;
};

template <class S, Structure ADS = _ADS>
concept bool HasGetADS = requires(S a)
{
   {
      a.getADSptr()
   }
   ->ADS *;
};

template <class S, Representation R, Structure ADS = _ADS>
//concept bool BaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;
concept bool BaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;

// Example of valid struct satisfying solution properties

template <Representation R, Structure ADS = _ADS>
struct IsSolution
{
   R &getR();
   ADS *getADSptr();

   IsSolution<R, ADS>& clone()
   {
      return *this; // TODO:
   }

   std::string toString()
   {
      return "";
   }
};

#endif // OPTFRAME_SOLUTION_CONCEPTS_H_