## Process this file with automake to produce Makefile.in

SUBDIRS = include

noinst_LTLIBRARIES = libPkgDep.la

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		media/include	\
		libPkgDep/include \
	   ) -I$(includedir) -I$(QTDIR)/include

#LDADD = $(top_srcdir)/src/libPkgDb/libPkgDb.la

#LINK = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
#libPkgDep_la_LDFLAGS = -release @VERSION@ -rpath $(libdir) \
#

libPkgDep_la_SOURCES = install.cc remove.cc consistency.cc alternatives.cc \
		       upgrade.cc results.cc output.cc utils.cc \
		       PkgDep.h PkgDep_int.h PkgSet.cc

#libPkgDep_la_LIBADD =  $(top_srcdir)/src/libPkgDb/libPkgDb.la

# Local Variables:
# tab-width: 4
# End:
