#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include

CPPFLAGS = -DY2LOG=__FILE__

INCLUDES = -I$(QTDIR)/include -I$(includedir) -I$(top_srcdir)/src/include \
	-I$(top_srcdir)/src/media/include \
	-I$(top_srcdir)/src/instsrc/include \
	-I$(top_srcdir)/src/pkg/include

noinst_LTLIBRARIES = liby2pminstsrc.la

liby2pminstsrc_la_SOURCES = 	\
	InstSrc.cc \
	InstSrcData.cc \
	InstSrcDataCommonPkd.cc \
	InstSrcDescr.cc \
	InstSrcManager.cc
