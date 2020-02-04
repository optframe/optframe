#ifndef OPTFRAME_SINGLE_OBJ_VALUE_HPP
#define OPTFRAME_SINGLE_OBJ_VALUE_HPP

// no includes (amazing class!)

#ifndef EVALUATION_TYPE
#define EVALUATION_TYPE double
#endif

#ifndef EVALUATION_ZERO
#define EVALUATION_ZERO 0.0001 // TODO: should be part of numerical 'concept' too
#endif

#ifndef EVALUATION_ABS
#define EVALUATION_ABS ::fabs // TODO: should be part of numerical 'concept' too
#endif

namespace optframe {
// default to many methods (TODO: unify on SingleObjValue or just leave as it is)
using evtype = EVALUATION_TYPE;
// Single Objective Value will use standard/default value
using SingleObjValue = evtype;
} // namespace optframe

#endif // OPTFRAME_SINGLE_OBJ_VALUE_HPP