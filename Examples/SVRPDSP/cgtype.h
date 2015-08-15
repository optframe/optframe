#ifndef CGTYPE_H_
#define CGTYPE_H_


//typedef int         small_type;  // integer
#define numeric_type long long
#define small_type int

// known big values are 48504419000000 and 37767488000000, so...
//const numeric_type MY_INFINITY = 1000000000000000000;
//const numeric_type MY_INFINITY = 9000000000000000000;
const numeric_type BIG_INFINITY = 9000000000000000000;
const small_type SMALL_INFINITY = 1000000000;

const numeric_type INFEASIBLE = 100000;

#endif /*CGTYPE_H_*/
