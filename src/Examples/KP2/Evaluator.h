#ifndef KP2_EVALUATOR_H_
#define KP2_EVALUATOR_H_

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "./ProblemInstance.h"
#include "./Representation.h"

namespace KP2
{


class MyEvaluator : public optframe::BasicEvaluator<RepKP2> {
private:
	ProblemInstance& pKP2;

public:
	MyEvaluator(ProblemInstance& _pKP2);
	virtual ~MyEvaluator() = default;

    // you can replace OptFrameADS with your prefered ADS
	optframe::Evaluation<> evaluate(const RepKP2& rep) override;
	
	virtual bool isMinimization() const;
};

}

#endif /*KP2_EVALUATOR_H_*/

