#ifndef AUTOCORR_H
#define AUTOCORR_H

#include <vector>

class acorrInfo
{
private:
	acorrInfo(const acorrInfo &rhs);
	std::vector<double> points_;
	/** slope of the autocorrelation line on a log/log plot */
	double slope_;
	double slopeErr_;
public:
	acorrInfo()
	{
		slope_ = 0;
	}
	~acorrInfo()
	{
	}

	double slope()
	{
		return slope_;
	}
	void slope(double s)
	{
		slope_ = s;
	}

	double slopeErr()
	{
		return slopeErr_;
	}
	void slopeErr(double sE)
	{
		slopeErr_ = sE;
	}

	std::vector<double> &points()
	{
		return points_;
	}
};
// class acorrInfo

/**
 Calculate the autocorrelation function for a vector


 <h4>
 Copyright and Use
 </h4>

 You may use this source code without limitation and without
 fee as long as you include:

 <p>
 <i>
 This software was written and is copyrighted by Ian Kaplan, Bear
 Products International, www.bearcave.com, 2001.
 </i>
 </p>

 This software is provided "as is", without any warranty or
 claim as to its usefulness.  Anyone who uses this source code
 uses it at their own risk.  Nor is any support provided by
 Ian Kaplan and Bear Products International.

 Please send any bug fixes or suggested source changes to:

 <pre>
 iank@bearcave.com
 </pre>

 */
class autocorr
{
private:
	autocorr(const autocorr &rhs);
	// Minimum correlation value
	const double limit_;
	// Total number of points to calculate
	const size_t numPoints_;

public:

	/**
	 A container for the autocorrelation function result.
	 */

private:
	void acorrSlope(acorrInfo &info);

public:
	autocorr(double lim = 0.01, size_t numPts = 32) :
			limit_(lim), numPoints_(numPts)
	{
	}
	~autocorr()
	{
	}

	// Autocorrelation function
	void ACF(const double *v, const size_t N, acorrInfo &info);
};
// autocorr

#endif
