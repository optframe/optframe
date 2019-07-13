
#include <math.h>

#include "./lregress.h"

using namespace std;

/**
   Calculate the mean of the x values in a vector of point
   objects.

 */
double lregress::meanX( vector<lregress::point> &data ) const
{
  double mean = 0.0;
  double sum = 0.0;

  const size_t N = data.size();
  size_t i;
  for (i = 0; i < N; i++) {
    sum = sum + data[i].x();
  }
  mean = sum / N;
  return mean;
} // meanX

/**
   Calculate the mean of the y values in a vector of point
   objects.

 */
double lregress::meanY( vector<lregress::point> &data ) const
{
  double mean = 0.0;
  double sum = 0.0;

  const size_t N = data.size();
  size_t i;
  for (i = 0; i < N; i++) {
    sum = sum + data[i].y();
  }
  mean = sum / N;
  return mean;
} // meanY



/**
  Calculate the linear regression coefficients, a and b, along with
  the standard regression error (e.g., the error of b), the standard
  deviation of the points and the correlation.

  A regression line is described by the equation y' = a + bx.  The
  coefficients a and b are returned in a lineInfo object, along
  with the other values.

  Formally, linear regression of a set of {x,y} points is described in
  terms of independent and dependent variables.  The array x contains
  the independent variable, which is exactly known.  The array y
  contains the dependent variable, which is "measured".  The y values
  are assumed to be random values, dependent on the x values.

 */
void lregress::lr( vector<lregress::point> &data,
		   lregress::lineInfo &info ) const
{
  const size_t N = data.size();
  if (N > 0) {

    double muX = meanX( data );

    double muY = meanY( data );

    //     N-1
    //     ---
    //     \   (Xi - meanX)(Yi - meanY)
    //     /__ 
    //     i=0
    // b = -----------------------------
    //     N-1
    //     ---             2
    //     \   (Xi - meanX)
    //     /__ 
    //     i=0
    //

    double SSxy = 0;
    double SSxx = 0;
    double SSyy = 0;
    double Sx = 0;
    double Sy = 0;
    double Sxy = 0;
    double SSy = 0;
    double SSx = 0;
    for (size_t i = 0; i < N; i++) {
      Sx = Sx + data[i].x();
      Sy = Sy + data[i].y();
      Sxy = Sxy + (data[i].x() * data[i].y());
      SSx = SSx + (data[i].x() * data[i].x());
      SSy = SSy + (data[i].y() * data[i].y());
      double subX = (data[i].x() - muX);
      double subY = (data[i].y() - muY);
      SSyy = SSyy + subY * subY;
      SSxy = SSxy + subX * subY;
      SSxx = SSxx + subX * subX;
    }
    
    // slope
    double b = SSxy / SSxx;
    // intercept
    double a = muY - b * muX;

    // standard deviation of the points
    double stddevPoints = sqrt( (SSyy - b * SSxy)/(N-2) );

    // Error of the slope
    double bError = stddevPoints / sqrt( SSxx );

    double r2Numerator = (N * Sxy) - (Sx * Sy);
    double r2Denominator = ((N*SSx) - (Sx * Sx))*((N*SSy) - (Sy * Sy));
    double r2 = (r2Numerator * r2Numerator) / r2Denominator;

    double signB = (b < 0) ? -1.0 : 1.0;

    double r = signB * sqrt( r2 );
  
    info.corr( r );
    info.stddev( stddevPoints );
    info.slopeErr( bError );
    info.slope( b );
    info.intercept( a );
  } // if N > 0

} // lineInfo
