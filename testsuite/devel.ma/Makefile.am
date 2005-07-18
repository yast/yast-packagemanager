#
# Makefile.am for packagemanager/testsuite/devel.ma
#
INCLUDES= -I$(top_srcdir)/src -I$(includedir) -I$(top_srcdir)/src/y2pm

LDADD = $(top_builddir)/src/liby2pm.la

AM_LDFLAGS = -static

CLEANFILES = *.bin

EXTRA_PROGRAMS = \
	ma_test.bin		\
	wfm_test.bin		\
	rpmtest.bin		\
	test_librpm_seek.bin	\
	test_rpm.bin		\
	VComp

ma_test_bin_SOURCES = ma_test.cc
ma_test_bin_LDADD = $(LDADD)

wfm_test_bin_SOURCES = wfm_test.cc
wfm_test_bin_LDADD = $(LDADD)

rpmtest_bin_SOURCES = rpmtest.cc
rpmtest_bin_LDADD = $(LDADD)

test_librpm_seek_bin_SOURCES =  test_librpm_seek.cc
test_librpm_seek_bin_LDADD = -ly2util -lrpm

test_rpm_bin_SOURCES =  test_rpm.cc
test_rpm_bin_LDADD = -ly2pm -ly2util -lrpm

VComp_SOURCES = VComp.cc
VComp_LDADD = $(LDADD)

ALL: $(EXTRA_PROGRAMS)