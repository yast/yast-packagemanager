#
# Makefile.am for packagemanager/testsuite
#
SUBDIRS = media
AUTOMAKE_OPTIONS = dejagnu
PACKAGE = packagemanager
INCLUDES= $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		libPkgDep/include \
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_PROGRAMS = \
	test_selections

test_selections_SOURCES = test_selections.cc
test_selections_LDADD = $(top_srcdir)/src/liby2pm.la
test_selections_LDFLAGS = -static

