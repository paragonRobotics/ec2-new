#include "types_twofileA.h"

void test_func( int i, int j )
{
	volatile int q;	
	volatile LOCAL_TYPE t;
	q = i+j;
	t.x = -i;
	t.y = -j;
	t.fp = i+j;
} 
