#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include

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
	   ) -I$(includedir) -I$(includedir)/y2util/qt

noinst_LTLIBRARIES = liby2pminstrpmsrc.la

liby2pminstrpmsrc_la_SOURCES = 	\
	RpmDb.cc \
	PMRpmPackageDataProvider.cc

liby2pminstrpmsrc_la_LIBADD = -ly2util
