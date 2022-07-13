#include <math.h>
#include "tistdtypes.h"

#define INT16_MAX 32767
#define INT16_MIN (-32768)

Int16 quantB(Int16 input, Uint16 B)
{
	Int16 output;
	Int16 round_factor = (1<<(16-B -1));

	if(INT16_MAX - round_factor < input)
	{
		output = (INT16_MAX >> (16-B));
	}
	else
	{
		output = ((input+round_factor) >> (16-B));
	}

	return output;
}

Int16 reconstructB(Int16 input, Uint16 B)
{
	Int16 output = input << (16-B);

	return output; 
}

