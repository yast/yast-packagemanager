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
	   ) -I$(includedir) -I/usr/include/rpm

LDADD = $(top_builddir)/src/liby2pm.la

LDFLAGS =

CLEANFILES = *.bin

EXTRA_PROGRAMS = \
	ma_test.bin	\
	rpmtest.bin

ma_test_bin_SOURCES = ma_test.cc
ma_test_bin_LDADD = $(LDADD)

rpmtest_bin_SOURCES = rpmtest.cc
rpmtest_bin_LDADD = $(LDADD)

ALL: $(EXTRA_PROGRAMS)