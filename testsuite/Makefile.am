#
# Makefile.am for packagemanager/testsuite
#
SUBDIRS = query devel.ma devel.cschum
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
		query/include	\
		libPkgDep/include \
	   ) -I$(includedir)

noinst_HEADERS = show_pm.h

noinst_PROGRAMS = \
	test_rpmdb	\
	test_tofromString.exe	\
	testinstall	\
	test_content	\
	test_packages	\
	test_selections	\
	test_query	\
	order		\
	gs_test		\
	media_test	\
	test_updateinf  \
        checkpatch

test_updateinf_SOURCES = test_updateinf.cc
test_updateinf_LDADD = $(top_builddir)/src/liby2pm.la

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

test_content_SOURCES = test_content.cc show_pm.cc
test_content_LDADD = $(top_builddir)/src/liby2pm.la
test_content_LDFLAGS = -static

test_packages_SOURCES = test_packages.cc show_pm.cc
test_packages_LDADD = $(top_builddir)/src/liby2pm.la
test_packages_LDFLAGS = -static

test_selections_SOURCES = test_selections.cc show_pm.cc
test_selections_LDADD = $(top_builddir)/src/liby2pm.la
test_selections_LDFLAGS = -static

test_query_SOURCES = test_query.cc show_pm.cc
test_query_LDADD = $(top_builddir)/src/liby2pm.la
test_query_LDFLAGS = -static

order_SOURCES = order.cc
order_LDADD = $(top_builddir)/src/liby2pm.la
order_LDFLAGS = -static

media_test_SOURCES = media_test.cc
media_test_LDADD = $(top_builddir)/src/liby2pm.la
media_test_LDFLAGS = -static

checkpatch_SOURCES = checkpatch.cc
checkpatch_LDADD = $(top_builddir)/src/liby2pm.la
checkpatch_LDFLAGS = -static
