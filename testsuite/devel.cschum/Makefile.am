INCLUDES= -I$(top_srcdir)/src -I$(includedir)

LDADD = $(top_builddir)/src/liby2pm.la

AM_LDFLAGS = -static

EXTRA_PROGRAMS = \
	cschum_test	\
	test_libcurl \
        err_test \
        callback_test \
        test_patchinfo \
        products \
        test_media \
        test_proddb \
	checkpatch

cschum_test_SOURCES = cschum_test.cc
cschum_test_LDADD = $(top_builddir)/src/liby2pm.la
cschum_test_LDFLAGS = -static

test_libcurl_SOURCES = test_libcurl.cc
test_libcurl_LDADD = -lcurl $(top_builddir)/src/liby2pm.la
test_libcurl_LDFLAGS = -static

test_media_SOURCES = test_media.cc
test_media_LDADD = $(top_builddir)/src/liby2pm.la
test_media_LDFLAGS = -static

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

test_proddb_SOURCES = test_proddb.cc
test_proddb_LDADD = $(top_builddir)/src/liby2pm.la
test_proddb_LDFLAGS = -static

checkpatch_SOURCES = checkpatch.cc
checkpatch_LDADD = -lcurl $(top_builddir)/src/liby2pm.la

ALL: $(EXTRA_PROGRAMS)
