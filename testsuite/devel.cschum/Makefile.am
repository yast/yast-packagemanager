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
	test_libcurl \
        err_test \
        callback_test \
        test_patchinfo \
        products

cschum_test_SOURCES = cschum_test.cc
cschum_test_LDADD = $(top_builddir)/src/liby2pm.la
cschum_test_LDFLAGS = -static

test_libcurl_SOURCES = test_libcurl.cc
test_libcurl_LDADD = -lcurl $(top_builddir)/src/liby2pm.la
test_libcurl_LDFLAGS = -static

err_test_SOURCES = err_test.cc
err_test_LDADD = -lcurl $(top_builddir)/src/liby2pm.la
err_test_LDFLAGS = -static

callback_test_SOURCES = callback_test.cc
callback_test_LDADD = $(top_builddir)/src/liby2pm.la
callback_test_LDFLAGS = -static

test_patchinfo_SOURCES = test_patchinfo.cc
test_patchinfo_LDADD = $(top_builddir)/src/liby2pm.la
test_patchinfo_LDFLAGS = -static

products_SOURCES = products.cc
products_LDADD = $(top_builddir)/src/liby2pm.la
products_LDFLAGS = -static

ALL: $(EXTRA_PROGRAMS)
