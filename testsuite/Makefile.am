#
# Makefile.am for packagemanager/testsuite
#

AUTOMAKE_OPTIONS = dejagnu

INCLUDES= $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

noinst_PROGRAMS = test_rpmdb ma_test.exe

# testinstall_SOURCES = testinstall.cc
# testinstall_LDFLAGS = -L$(libdir)	\
# 	-Xlinker --whole-archive	\
# 	$(top_srcdir)/src/libPkgDb/libPkgDb.la	\
# 	$(top_srcdir)/src/libPkgDep/libPkgDep.la	\
# 	-Xlinker --no-whole-archive
# testinstall_LDADD = -ly2util

ma_test_exe_SOURCES = ma_test.cc
ma_test_exe_LDADD = $(top_srcdir)/src/liby2pm.la
ma_test_exe_LDFLAGS = -static

test_rpmdb_SOURCES = test_rpmdb.cc
test_rpmdb_LDADD = $(top_srcdir)/src/liby2pm.la
test_rpmdb_LDFLAGS = -static
