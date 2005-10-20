#
# Makefile.am for packagemanager/testsuite/devel.mir
#

SUBDIRS = YUMParser.test

AUTOMAKE_OPTIONS = dejagnu

PACKAGE = YUMParser

INCLUDES = -I$(top_srcdir)/src -I$(oldincludedir)/libxml2 -I$(includedir)


check_PROGRAMS = YUMtest

YUMtest_SOURCES = YUMtest.cc
YUMtest_LDADD = $(top_builddir)/src/liby2pm.la
YUMtest_LDFLAGS = -static

EXTRA_DIST = testit.sh
