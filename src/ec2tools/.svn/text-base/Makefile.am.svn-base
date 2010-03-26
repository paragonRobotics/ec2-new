INCLUDES = -I$(top_srcdir)/src/ec2drv $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = ec2writeflash ec2readflash ec2-update-fw ec2device ec3adapters ec2readfw \
				ec2test-any

ec2writeflash_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la
ec2writeflash_SOURCES = writeflash.c ihex.c

noinst_HEADERS = ihex.h

ec2readflash_SOURCES = ihex.c readflash.c
ec2readflash_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la

ec2_update_fw_SOURCES = ec2fw.c
ec2_update_fw_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la

ec2readfw_SOURCES = ec2rfw.c
ec2readfw_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la

ec2device_SOURCES = ec2device.cpp
ec2device_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la

ec3adapters_SOURCES = ec3adapters.c
ec3adapters_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la -lusb

ec2test_any_SOURCES = ec2test-any.cpp
ec2test_any_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la
