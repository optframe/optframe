#include <math.h>

#include "./lregress.h"
#include "./autocorr.h"

using namespace std;

/**
 Calculate the slope the the log of the autocorrelation
 curve.

 Autocorrelation is one measure for long memory (long range
 dependence) in a data set.  The idea behind implementing this
 function was that the slope of the log/log plot of the
 autocorrelation curve could be compared for various data
 sets.

 The Hurst exponent provides another measure for long memory
 processes.  My original idea was to use the slope of the log/log
 ACF as a comparision for the various Hurst values.  As it turned
 out, the slope of the ACF was not a very useful metric since the
 slopes did not differ much for various Hurst exponents

 */
void autocorr::acorrSlope(acorrInfo &info)
{

	const size_t len = info.points().size();
	if (len > 0)
	{
		lregress::lineInfo regressInfo;
		lregress lr;
		vector<lregress::point> regressPoints;
		for (size_t i = 0; i < len; i++)
		{
			double x = log(i + 1);
			double y = log((info.points())[i]);
			regressPoints.push_back(lregress::point(x, y));
		}
		lr.lr(regressPoints, regressInfo);
		info.slope(regressInfo.slope());
		info.slopeErr(regressInfo.slopeErr());
	}
} // acorrSlope

/**
 Calculate the autocorrelation function
 */
void autocorr::ACF(const double *v, const size_t N, acorrInfo &info)
{
	if (!info.points().empty())
	{
		info.points().clear();
	}

	// The devMean array will contain the deviation from the mean.
	// That is, v[i] - mean(v).
	double *devMean = new double[N];

	double sum;
	size_t i;

	// Calculate the mean and copy the vector v, into devMean
	sum = 0;
	for (i = 0; i < N; i++)
	{
		sum = sum + v[i];
		devMean[i] = v[i];
	}
	double mean = sum / static_cast<double>(N);

	// Compute the values for the devMean array.  Also calculate the
	// denominator for the autocorrelation function.
	sum = 0;
	for (i = 0; i < N; i++)
	{
		devMean[i] = devMean[i] - mean;
		sum = sum + (devMean[i] * devMean[i]);
	}
	double denom = sum / static_cast<double>(N);

	// Calculate a std::vector of values which will make up
	// the autocorrelation function.
	double cor = 1.0;
	for (size_t shift = 1; shift <= numPoints_ && cor > limit_; shift++)
	{
		info.points().push_back(cor);
		size_t n = N - shift;
		sum = 0.0;
		for (i = 0; i < n; i++)
		{
			sum = sum + (devMean[i] * devMean[i + shift]);
		}
		double numerator = sum / static_cast<double>(n);
		cor = numerator / denom;
	}

	// calculate the log/log slope of the autocorrelation
	acorrSlope(info);

	//delete [] m;
} // ACF
