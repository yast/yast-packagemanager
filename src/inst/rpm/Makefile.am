#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		inst/include	\
		inst/rpm/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_LTLIBRARIES = liby2pminstrpmsrc.la

liby2pminstrpmsrc_la_SOURCES = 	\
	RpmDb.cc \
	PMRpmPackageDataProvider.cc

liby2pminstrpmsrc_la_LIBADD = -ly2util
