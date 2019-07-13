#ifndef HFM_EVALUATION_H_
#define HFM_EVALUATION_H_

//TODO -- Increased the size of Epsilon for avoiding similar solutions
//A small different on forecasting acuracies was leading to similar forecasting models
//On the other hand, at the end of the optimization we might loose models or deteriote the behavior of the algorithm
//#define EVALUATION_ZERO 0.001

#include "../../OptFrame/Evaluation.hpp"

using namespace optframe;

typedef Evaluation EvaluationEFP;

#endif /*HFM_EVALUATION_H_*/



