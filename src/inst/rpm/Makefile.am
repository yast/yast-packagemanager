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

liby2pminstrpmsrc_la_SOURCES = 	\
	RpmDb.cc \
	RpmLibDb.cc \
	RpmLibHeader.cc \
	PMRpmPackageDataProvider.cc

liby2pminstrpmsrc_la_LIBADD = -ly2util -lrpm -lpopt

EXTRA_DIST = Make.defines RpmLib.h
