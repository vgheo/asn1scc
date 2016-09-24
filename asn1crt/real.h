/*
 * real.h
 *
 */

#ifndef REAL_H_
#define REAL_H_

#include "core.h"

void CalculateMantissaAndExponent(double d, int* exp, asn1SccUint64* mantissa);
double GetDoubleByMantissaAndExp(asn1SccUint mantissa, int exp);


#endif /* REAL_H_ */
