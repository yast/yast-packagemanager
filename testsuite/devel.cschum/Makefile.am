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

EXTRA_PROGRAMS = \
	cschum_test	\
	test_libcurl

cschum_test_SOURCES = cschum_test.cc
cschum_test_LDADD = $(top_builddir)/src/liby2pm.la
cschum_test_LDFLAGS = -static

test_libcurl_SOURCES = test_libcurl.cc
test_libcurl_LDADD = -lcurl $(top_builddir)/src/liby2pm.la
test_libcurl_LDFLAGS = -static

ALL: $(EXTRA_PROGRAMS)
