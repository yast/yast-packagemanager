#
# Makefile.am for packagemanager/src/media/instance
#

SUBDIRS = include

INCLUDES = -I$(top_srcdir)/src/include -I$(QTDIR)/include -I$(includedir) -I$(top_srcdir)/src/media/include

lib_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	FAKEMediaInfo.cc \
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaCD.cc		\
	MediaDIR.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaFTP.cc		\
	MediaHTTP.cc

