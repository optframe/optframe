#ifndef KP_REPRESENTATION_H_
#define KP_REPRESENTATION_H_

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>
#include <vector>

using namespace std;
using namespace optframe;

// Solution Representation
typedef vector<bool> RepKP;

typedef Evaluation<> EvaluationKP;

static_assert(XEvaluation<EvaluationKP>);

#endif /*KP_REPRESENTATION_H_*/
