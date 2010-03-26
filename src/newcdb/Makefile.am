bin_PROGRAMS = newcdb
newcdb_SOURCES = parsecmd.cpp cmdcommon.cpp cmdbreakpoints.cpp \
	cmddisassemble.cpp cmdmaintenance.cpp main.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/src/debug-core -I$(top_srcdir)/src/ec2drv \
	$(all_includes)

# the library search path.
newcdb_LDFLAGS = $(all_libraries) 
noinst_HEADERS = parsecmd.h cmdshow.h cmdcommon.h cmdbreakpoints.h \
	cmddisassemble.h cmdmaintenance.h newcdb.h
newcdb_LDADD = $(top_builddir)/src/ec2drv/libec2drv.la \
	$(top_builddir)/src/debug-core/libec2debugcore.la -lboost_regex -lpthread

##AUTOMAKE_OPTIONS = dejagnu
