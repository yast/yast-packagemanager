#
# Makefile.am for packagemanager/src/media
#

SUBDIRS = include

CPPFLAGS = -DY2LOG=__FILE__
INCLUDES = -I$(top_srcdir)/src/include -I$(QTDIR)/include -I$(includedir) -I$(top_srcdir)/src/media/include

lib_LTLIBRARIES = liby2pmMedia.la

liby2pmMedia_la_SOURCES = \
	MediaAccess.cc		\
	MediaHandler.cc		\
	MediaCD.cc		\
	MediaDIR.cc		\
	MediaNFS.cc		\
	MediaDISK.cc		\
	MediaSMB.cc		\
	MediaFTP.cc		\
	MediaHTTP.cc		\
	Wget.cc

liby2pmMedia_la_LIBADD = -ly2util

noinst_PROGRAMS = media_test_ftp

media_test_ftp_SOURCES = media_test_ftp.cc
media_test_ftp_LDADD = liby2pmMedia.la
#for debuggin
media_test_ftp_LDFLAGS = -static
