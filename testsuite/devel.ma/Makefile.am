#
# Makefile.am for packagemanager/testsuite/devel.ma
#
INCLUDES= $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		pkg/include		\
		sel/include		\
		you/include		\
		inst/include		\
		inst/rpm/include	\
		media/include		\
		query/include		\
		libPkgDep/include	\
	   ) -I$(includedir) -I/Local/ma/yast2/current/source/core/wfm/src

LDADD = $(top_builddir)/src/liby2pm.la /Local/ma/yast2/current/source/core/wfm/src/liby2wfm.la

#LDFLAGS = -static

CLEANFILES = *.bin

EXTRA_PROGRAMS = \
	ma_test.bin		\
	rpmtest.bin		\
	test_librpm_seek.bin	\
	test_rpm.bin

ma_test_bin_SOURCES = ma_test.cc
ma_test_bin_LDADD = $(LDADD)

rpmtest_bin_SOURCES = rpmtest.cc
rpmtest_bin_LDADD = $(LDADD) -lycp

test_librpm_seek_bin_SOURCES =  test_librpm_seek.cc
test_librpm_seek_bin_LDADD = -ly2util -lrpm

test_rpm_bin_SOURCES =  test_rpm.cc
test_rpm_bin_LDADD = -ly2pm -ly2util -lrpm

ALL: $(EXTRA_PROGRAMS)