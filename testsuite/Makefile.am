#
# Makefile.am for packagemanager/testsuite
#

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
	test_content \
	test_packages

test_content_SOURCES = test_content.cc
test_content_LDADD = $(top_srcdir)/src/liby2pm.la
test_content_LDFLAGS = -static

test_packages_SOURCES = test_packages.cc
test_packages_LDADD = $(top_srcdir)/src/liby2pm.la
test_packages_LDFLAGS = -static


