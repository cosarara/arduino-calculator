#include "math_extra.h"
#include <math.h>

double fact(double x) {
	//return tgamma(x+1.);
	
	int i;
	unsigned int ux = (unsigned int)x;
	unsigned int value = 1;

	for(i = 2; i <= ux; i++)
	{
		value *= i;
	}

	return value;
}


