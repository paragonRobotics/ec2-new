#include "types_twofileA.h"

void test_func( int i, int j );


int main()
{
	LOCAL_TYPE a;
	LOCAL_TYPE b;
	int i=1;

	while(1)
	{
		test_func(i,i*2);
		i++;
		if(i>4)
			i=1;
	}
	
}
