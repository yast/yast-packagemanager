#
# Makefile.am for packagemanager/src/pkg
#

SUBDIRS = include

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_LTLIBRARIES = liby2pmpkg.la

liby2pmpkg_la_SOURCES = \
	PMYouPatch.cc \
	PMYouPatchDataProvider.cc \
	PMYouPatchManager.cc
