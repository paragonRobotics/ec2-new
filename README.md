Linux support for EC2 debugger from Silicon Laboritories
========================================================

This is a fork from Ricky White's ec2drv (http://ec2drv.sourceforge.net), and has recently merged in lots of fixes from and Andreas Gustafsson, Jonas Danielsson and YanJun Yang.

This package provides several command-line tools for working with the EC2/EC3 and ToolStick debuggers, as well as a command-line debugger which is fairly full-featured.

ec2tools contain programs that use the core library to perform various actions.
+ ec2test-any - test the opperation of a micro / debugger combination
+ ec2readflash - read the target flash memory.
+ ec2writeflash - write to the target flash memory.
+ ec2device - identify connected microprocessor
+ ec3adapters - list all USB debug adaptors and their serial numbers
+ ec2readfw - read the debugger firmware image
+ ec2-update-fw - write new firmware into the debugger

newcdb is the text-based interactive debugger, which can be used to fully debug programs
+ flash firmware to devices
+ inspect all registers, SFRs, RAM, code, and XRAM
+ modify all registers, SFRs, RAM, and XRAM
+ set breakpoints
+ run, stop, and step through programs
	
