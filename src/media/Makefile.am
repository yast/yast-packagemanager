#
# Makefile.am for packagemanager/src/media
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall -imacros Make.defines
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	MediaError.cc		\
	MediaCallbacks.cc	\
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaDIR.cc		\
	MediaCD.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaCIFS.cc		\
	MediaCurl.cc		\
	Mount.cc

liby2pmMedia_la_LIBADD = -lcurl -ly2util


EXTRA_DIST = Make.defines
