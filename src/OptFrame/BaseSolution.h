#ifndef OPTFRAME_SOLUTION_CONCEPTS_H_
#define OPTFRAME_SOLUTION_CONCEPTS_H_

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help us!)
typedef int OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS; // more beautiful :)
typedef OPTFRAME_DEFAULT_ADS _ADS;        // more beautiful :)

template<class S, class R>
concept bool HasGetR = requires(S a)
{
   {
      a.getR()
   }
   ->R&;
};

template<class S, class ADS = _ADS>
concept bool HasGetADS = requires(S a)
{
   {
      a.getADSptr()
   }
   ->ADS*;
};

template<class S, class R, class ADS = _ADS>
concept bool BaseSolution = HasGetR<S, R>&& HasGetADS<S, ADS>;

// Example of valid struct satisfying solution properties

template<class R, class ADS = _ADS>
struct IsSolution
{
   R& getR();
   ADS* getADSptr();
};

#endif // OPTFRAME_SOLUTION_CONCEPTS_H_