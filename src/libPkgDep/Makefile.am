## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libPkgDep.la

INCLUDES = -I. -I$(top_srcdir)/src/libPkgDb -I/usr/include/YaST2
#LDADD = $(top_srcdir)/src/libPkgDb/libPkgDb.la

#LINK = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
#libPkgDep_la_LDFLAGS = -release @VERSION@ -rpath $(libdir) \
#

libPkgDep_la_SOURCES = install.cc remove.cc consistency.cc alternatives.cc \
		       upgrade.cc results.cc output.cc utils.cc \
		       PkgDep.h PkgDep_int.h

libPkgDep_la_LIBADD =  $(top_srcdir)/src/libPkgDb/libPkgDb.la

# Local Variables:
# tab-width: 4
# End:
