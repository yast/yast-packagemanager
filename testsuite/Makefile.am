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
	   ) -I$(includedir) -I$(includedir)/y2util/qt

noinst_HEADERS = show_pm.h
 
noinst_PROGRAMS = \
	test_rpmdb	\
	test_tofromString.exe	\
	testinstall	\
	test_patchinfo	\
	test_content	\
	test_packages	\
	test_selections	\
	order	\
	gs_test \
        test_you \
        cschum_test

#	ma_test.exe
#
#ma_test_exe_SOURCES = ma_test.cc
#ma_test_exe_LDADD = $(top_builddir)/src/liby2pm.la
#ma_test_exe_LDFLAGS = -static

gs_test_SOURCES = gs_test.cc
gs_test_LDADD =  $(top_builddir)/src/liby2pm.la		\
		$(top_builddir)/src/inst/liby2pminstsrc.la
gs_test_LDFLAGS = -static

test_tofromString_exe_SOURCES = test_tofromString.cc
test_tofromString_exe_LDADD = $(top_builddir)/src/liby2pm.la
test_tofromString_exe_LDFLAGS = -static

test_rpmdb_SOURCES = test_rpmdb.cc show_pm.cc
test_rpmdb_LDADD = $(top_builddir)/src/liby2pm.la
test_rpmdb_LDFLAGS = -static

testinstall_SOURCES = testinstall.cc show_pm.cc
testinstall_LDADD = $(top_builddir)/src/liby2pm.la -lreadline -lncurses
testinstall_LDFLAGS = -static

test_patchinfo_SOURCES = test_patchinfo.cc
test_patchinfo_LDADD = $(top_builddir)/src/liby2pm.la
test_patchinfo_LDFLAGS = -static

test_content_SOURCES = test_content.cc show_pm.cc
test_content_LDADD = $(top_builddir)/src/liby2pm.la
test_content_LDFLAGS = -static

test_packages_SOURCES = test_packages.cc show_pm.cc
test_packages_LDADD = $(top_builddir)/src/liby2pm.la
test_packages_LDFLAGS = -static

test_selections_SOURCES = test_selections.cc show_pm.cc
test_selections_LDADD = $(top_builddir)/src/liby2pm.la
test_selections_LDFLAGS = -static

order_SOURCES = order.cc
order_LDADD = $(top_builddir)/src/liby2pm.la
order_LDFLAGS = -static

test_you_SOURCES = test_you.cc
test_you_LDADD = $(top_builddir)/src/liby2pm.la
test_you_LDFLAGS = -static

cschum_test_SOURCES = cschum_test.cc
cschum_test_LDADD = $(top_builddir)/src/liby2pm.la
cschum_test_LDFLAGS = -static
