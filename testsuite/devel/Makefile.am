#
# Makefile.am for packagemanager/testsuite/devel
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

## ###################################################################

##CLEANFILES =

EXTRA_HEADERS = show_pm.h

EXTRA_PROGRAMS = \
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
	autosolvertest	\
	test_updateinf

EXTRA_DIST=README.autosolvertest rpm2hdr $(EXTRA_HEADERS)

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

#test_speed_SOURCES = test_speed.cc
#test_speed_LDADD = $(top_builddir)/src/liby2pm.la
#test_speed_LDFLAGS = -static

order_SOURCES = order.cc
order_LDADD = $(top_builddir)/src/liby2pm.la
order_LDFLAGS = -static

media_test_SOURCES = media_test.cc
media_test_LDADD = $(top_builddir)/src/liby2pm.la
media_test_LDFLAGS = -static

autosolvertest_SOURCES = autosolvertest.cc show_pm.cc
autosolvertest_LDADD = $(top_builddir)/src/liby2pm.la
autosolvertest_LDFLAGS = -static

## ###################################################################

ALL: $(EXTRA_PROGRAMS)
