// Test of application with a simple loop
#include <C8051F310.h>

int main()
{
	_asm MOV B,#42		_endasm;
	_asm MOV DPTR,#0x200	_endasm;
	while(1)
	{
		_asm INC DPTR	_endasm;
		_asm CPL C	_endasm;
	}
}

