; asmtest.asm
;	This file contains the main function for a debugging test involving a
;	stand alone asm file.
;	This allows testing with mixed mode (asm+c) and with a asm only file in 
;	one porject.
;	
;	(C) Ricky White 2007
;	Liscence see COPYING
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.area CSEG    (CODE)
 
.globl _asmfunc
; extern int asm_func(unsigned int);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_asmfunc:
	PUSH	ACC
;	MOV	A,dpl
;	CPL	A
;	MOV	dpl,a
;	MOV	A,dph
;	CPL	A
;	MOV	dpl,a
	CPL	P1.0
	POP	ACC
	RET
    
    
    .globl _asm_func_PARM_2  
            .globl _asm_func  
            .area OSEG  
    _asm_func_PARM_2: 
            .ds 1  
            .area CSEG  
    _asm_func:  
            mov    a,dpl  
            add    a,_asm_func_PARM_2  
            mov    dpl,a  
            mov    dph,#0x00  
            ret 