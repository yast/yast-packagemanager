#
# Makefile.am for packagemanager/testsuite
#

AUTOMAKE_OPTIONS = dejagnu

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

noinst_PROGRAMS = test_rpmdb ma_test.exe testinstall

ma_test_exe_SOURCES = ma_test.cc
ma_test_exe_LDADD = $(top_builddir)/src/liby2pm.la
ma_test_exe_LDFLAGS = -static

test_rpmdb_SOURCES = test_rpmdb.cc
test_rpmdb_LDADD = $(top_builddir)/src/liby2pm.la
test_rpmdb_LDFLAGS = -static

testinstall_SOURCES = testinstall.cc
testinstall_LDADD = $(top_builddir)/src/liby2pm.la -lreadline -lncurses
testinstall_LDFLAGS = -static
