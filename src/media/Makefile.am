#
# Makefile.am for packagemanager/src/media
#

SUBDIRS = include

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		inst/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

lib_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaCD.cc		\
	MediaDIR.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaWget.cc		\
	Wget.cc			\
	Mount.cc

liby2pmMedia_la_LIBADD = -ly2util

noinst_PROGRAMS = media_test.prg

media_test_prg_SOURCES = media_test.cc
media_test_prg_LDADD = liby2pmMedia.la
#for debuggin
media_test_prg_LDFLAGS = -static
