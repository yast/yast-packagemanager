#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = include

CPPFLAGS = -DY2LOG=__FILE__

INCLUDES = -I$(QTDIR)/include -I$(includedir) -I$(top_srcdir)/src/include \
	-I$(top_srcdir)/src/media/include \
	-I$(top_srcdir)/src/inst/include \
	-I$(top_srcdir)/src/inst/rpm/include \
	-I$(top_srcdir)/src/pkg/include

noinst_LTLIBRARIES = liby2pminstrpmsrc.la
noinst_PROGRAMS = test_rpmdb.prg

liby2pminstrpmsrc_la_SOURCES = 	\
	RpmDb.cc

liby2pminstrpmsrc_la_LIBADD = -ly2util

test_rpmdb_prg_SOURCES = test_rpmdb.cc
test_rpmdb_prg_LDADD = liby2pminstrpmsrc.la
