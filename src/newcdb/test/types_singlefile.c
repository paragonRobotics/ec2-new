// Simple two function test file

void test_func( int i, int j )
{
	volatile int q;	
	q = i+j;
} 


int main()
{
	typedef struct
	{
		int x;
		int y;
		char ch;
		float fp;
		char ar[4];
		char *p;
	} LOCAL_TYPE;

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
