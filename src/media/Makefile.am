#
# Makefile.am for packagemanager/src/media
#

SUBDIRS = include

# override commandline define for Y2LOG
AM_CXXFLAGS = -imacros Make.defines

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

lib_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	MediaError.cc		\
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaDIR.cc		\
	MediaCD.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaWget.cc		\
	Wget.cc			\
	Mount.cc

liby2pmMedia_la_LIBADD = -ly2util


EXTRA_DIST = Make.defines
#
# MOVE IT TO TESTSUITE
# needs liby2pm to link
#
# noinst_PROGRAMS = media_test.prg
#
# media_test_prg_SOURCES = media_test.cc
# media_test_prg_LDADD = liby2pmMedia.la
#for debuggin
# media_test_prg_LDFLAGS = -static
