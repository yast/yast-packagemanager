#
# Makefile.am for packagemanager/testsuite
#

AUTOMAKE_OPTIONS = dejagnu

INCLUDES= -I../src/include -I$(includedir)

noinst_PROGRAMS = testinstall

testinstall_SOURCES = testinstall.cc
testinstall_LDFLAGS = -L$(libdir)	\
	-Xlinker --whole-archive	\
	$(top_srcdir)/src/libPkgDb/libPkgDb.la	\
	$(top_srcdir)/src/libPkgDep/libPkgDep.la	\
	-Xlinker --no-whole-archive

