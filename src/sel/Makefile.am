#
# Makefile.am for packagemanager/src/sel
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
	   ) -I$(includedir) -I$(includedir)/y2util/qt

noinst_LTLIBRARIES = liby2pmsel.la

liby2pmsel_la_SOURCES = \
	PMSelection.cc \
	PMSelectionDataProvider.cc \
	PMSelectionManager.cc
