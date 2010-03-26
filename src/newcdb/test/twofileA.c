// Simple two function test file

extern void test_func( int i, int j );

int main()
{
	volatile int p,q;

	p = 1;
	q = 4;
	while(1)
	{
		test_func( p,q);
		p++;
		q--;
		if(q<0)
			q = 4;
	}
}

