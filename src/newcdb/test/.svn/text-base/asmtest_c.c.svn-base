/** asmtest.c
	This file contains the main function for a debugging test involving a stand alone asm file.
	This allows testing with mixed mode (asm+c) and with a asm only file in one porject.
	
	(C) Ricky White 2007
	Lincence see COPYING
*/

extern int asmfunc(unsigned int);
extern int asm_func(unsigned char, unsigned char);


int main()
{
	volatile int i,j;
	
	i = 0;
	while(1)
	{
		i++;
		j = asmfunc(i);
		j += asm_func(1,2);
	}
}

