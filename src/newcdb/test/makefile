###############################
# makefile		              #
# newcdb test programs        #
# (C) Ricky White March 2007  #
###############################

# --noinduction and --nooverlay ensures that our test symbols are all visible
CFLAGS = -mmcs51 --model-small --debug --code-loc 0x0000 --iram-size 0xFF --noinduction --nooverlay

%.rel : %.a51
	asx8051 -losgc $<

%.rel : %.c
	sdcc -c $(CFLAGS) $<


all:	simple.ihx			\
	term_types.ihx			\
	term_types_array.ihx		\
	twofunc.ihx			\
	twofile.ihx			\
	types_singlefile.ihx		\
	types_twofile.ihx		\
	nested_struct.ihx		\
	nested_types.ihx		\
	regtest.ihx			\
	asmtest.ihx


simple.ihx : simple.rel
	sdcc $(CFLAGS) -o simple.ihx simple.rel

term_types.ihx : term_types.rel
	sdcc $(CFLAGS) -o term_types.ihx term_types.rel

term_types_array.ihx : term_types_array.rel
	sdcc $(CFLAGS) -o term_types_array.ihx term_types_array.rel

twofunc.ihx : twofunc.rel
	sdcc $(CFLAGS) -o twofunc.ihx twofunc.rel

twofile.ihx : twofileA.rel twofileB.rel
	sdcc $(CFLAGS) -o twofile.ihx twofileA.rel twofileB.rel

types_singlefile.ihx : types_singlefile.rel
	sdcc $(CFLAGS) -o types_singlefile.ihx types_singlefile.rel


types_twofile.ihx : types_twofileA.rel types_twofileB.rel
	sdcc $(CFLAGS) -o types_twofileA.ihx types_twofileA.rel types_twofileB.rel


nested_struct.ihx : nested_struct.rel
	sdcc $(CFLAGS) -o nested_struct.ihx nested_struct.rel

nested_types.ihx : nested_types.rel
	sdcc $(CFLAGS) -o nested_types.ihx nested_types.rel

regtest.ihx: regtest.rel
	sdcc $(CFLAGS) -o regtest.ihx regtest.rel


asmtest.ihx: asmtest_c.rel asmtest_a51.rel
	sdcc $(CFLAGS) -o asmtest.ihx asmtest_c.rel asmtest_a51.rel 
	
	
clean:
	rm -f *.asm *.rel *.rst *.cdb *.ihx *.adb *.lnk *.lst *.map *.sym *.mem
	rm -f simple term_types term_types_array twofunc twofile types_twofileA
	rm -f types_twofileB types_singlefile types_twofile nested_struct
	rm -f nested_types regtest asmtest
	




