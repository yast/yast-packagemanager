#
# Makefile.am for packagemanager/src/media
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
		inst/rpm/include\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	MediaError.cc		\
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaDIR.cc		\
	MediaCD.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaCurl.cc		\
	MediaWget.cc		\
	Wget.cc			\
	Mount.cc

liby2pmMedia_la_LIBADD = -lcurl -ly2util


EXTRA_DIST = Make.defines
