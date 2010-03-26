// Simple two function test file

volatile int out_a;
void test_func( int i, int j )
{
	int x,y;
	for(x=0; x<i; x++)
	{
		for(y=0; y<j; y++)
		{
			out_a = x+y;
		}
	}
} 



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
