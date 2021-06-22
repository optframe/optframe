#ifndef KP_REPRESENTATION_H_
#define KP_REPRESENTATION_H_

#include <OptFrame/printable/printable.hpp>
#include <vector>

#include <OptFrame/Util/Matrix.hpp>

#include <OptFrame/Evaluation.hpp>

#include <vector>

using namespace std;
using namespace optframe;

// Solution Representation
typedef vector<bool> RepKP;

typedef Evaluation<> EvaluationKP;

static_assert(XEvaluation<EvaluationKP>);

#endif /*KP_REPRESENTATION_H_*/
