#ifndef OPTFRAME_KAHANACCUMULATION_HPP_
#define OPTFRAME_KAHANACCUMULATION_HPP_

#include <vector>
#include <numeric> //accumlate

namespace optframe
{

struct KahanAccumulation
{
	double sum;
	double correction;
};

static KahanAccumulation KahanSum(KahanAccumulation accumulation, double value)
{
	KahanAccumulation result;
	double y = value - accumulation.correction;
	double t = accumulation.sum + y;
	result.correction = (t - accumulation.sum) - y;
	result.sum = t;
	return result;
}


//return pair of avgStdDev
std::pair<double, double> calculateAvgStd(const std::vector<double> vTimeSamples)
{
	int nSamples = vTimeSamples.size();
	KahanAccumulation init =
	{ 0 };
	KahanAccumulation kahanSum = std::accumulate(vTimeSamples.begin(), vTimeSamples.end(), init, KahanSum);
	double avg = kahanSum.sum / nSamples;

	double stdev = 0.0;

	for_each(vTimeSamples.begin(), vTimeSamples.end(), [&](const double d)
	{
		stdev += (d - avg) * (d - avg);
	});

	if ((nSamples - 1) == 0)
		stdev = 0;
	else
		stdev = sqrt(stdev / (nSamples - 1));

	return make_pair(avg, stdev);
}




}

#endif /* OPTFRAME_KAHANACCUMULATION_HPP_ */
