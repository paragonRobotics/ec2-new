INCLUDES = $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libec2drv.la
libec2drv_la_SOURCES = ec2drv.c devices.c device_table.c jtag_mode.c c2_mode.c \
	boot.c

libec2drv_la_LIBADD = -lusb
noinst_HEADERS = devices.h ec2drv.h jtag_mode.h c2_mode.h boot.h device_enum.h \
	ec2types.h
noinst_SCRIPTS = csv2c.py
EXTRA_DIST = csv2c.py


BUILT_SOURCES = device_table.c device_enum.h
CLEANFILES = device_table.c device_enum.h

device_table.c device_enum.h: device_table.csv
	$(PYTHON_BIN) $(srcdir)/csv2c.py -i $(srcdir)/device_table.csv \
	-c $(srcdir)/device_table.c \
	-e $(srcdir)/device_enum.h

device_enum.h: device_table.c

devices.h: device_enum.h

ec2drv_incdir = @prefix@/include/ec2drv
ec2drv_inc_HEADERS = ec2drv.h device_enum.h ec2types.h devices.h

