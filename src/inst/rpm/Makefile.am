#
# Makefile.am for packagemanager/src/inst/rpm
#

SUBDIRS = include

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall -imacros Make.defines
# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include	\
		media/include	\
		libPkgDep/include	\
	   ) -I$(includedir) -I/usr/include/rpm

noinst_LTLIBRARIES = liby2pminstrpmsrc.la

liby2pminstrpmsrc_la_SOURCES = 	librpm.h	\
	librpmDb.cc librpmDb.cv3.cc		\
	binHeader.cc binHeaderCache.cc		\
	RpmHeader.cc RpmHeaderCache.cc		\
	RpmDb.cc RpmDbCallbacks.cc		\
	PMRpmPackageDataProvider.cc

liby2pminstrpmsrc_la_LIBADD = -ly2util -lrpm -lpopt

# also define FAKELIBRPM in librpm.h
#liby2pminstrpmsrc_la_LIBADD = -ly2util

EXTRA_DIST = Make.defines
