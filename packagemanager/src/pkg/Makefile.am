#
# Makefile.am for packagemanager/src/pkg
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmpkg.la

liby2pmpkg_la_SOURCES = \
	PMPackage.cc \
	PMPackageDataProvider.cc \
	PMPackageManager.cc PMPackageManagerCallbacks.cc PMPackageManager_update.cc	\
	PMPackageImEx.cc
