#
# Makefile.am for packagemanager/testsuite
#
SUBDIRS = devel.ma devel.cschum devel

AUTOMAKE_OPTIONS = dejagnu

PACKAGE = packagemanager

INCLUDES= -I$(top_srcdir)/src -I$(includedir)

check_PROGRAMS = test_filterarchs

test_filterarchs_SOURCES = test_filterarchs.cc
test_filterarchs_LDADD = $(top_builddir)/src/liby2pm.la
test_filterarchs_LDFLAGS = -static
