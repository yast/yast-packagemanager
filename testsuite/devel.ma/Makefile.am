#
# Makefile.am for packagemanager/testsuite/devel.ma
#
INCLUDES= $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
		query/include	\
		libPkgDep/include \
	   ) -I$(includedir)

LDADD = $(top_builddir)/src/liby2pm.la

LDFLAGS = -static

CLEANFILES = *.exe

EXTRA_PROGRAMS = \
	ma_test.exe	\
	rpmtest.exe

ma_test_exe_SOURCES = ma_test.cc
#ma_test_exe_LDADD = $(top_builddir)/src/liby2pm.la
#ma_test_exe_LDFLAGS = -static

rpmtest_exe_SOURCES = rpmtest.cc
#rpmtest_exe_LDADD = $(top_builddir)/src/liby2pm.la
#rpmtest_exe_LDFLAGS = -static

ALL: $(EXTRA_PROGRAMS)