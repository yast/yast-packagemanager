#
# Makefile.am for packagemanager/src/pkg
#

SUBDIRS = include

INCLUDES = -I$(QTDIR)/include -I$(includedir) -I$(top_srcdir)/src/include \
	-I$(top_srcdir)/src/pkg/include

noinst_LTLIBRARIES = liby2pmpkg.la

liby2pmpkg_la_SOURCES = \
	PMPackage.cc \
	PMPackageDataProvider.cc \
	PMPackageManager.cc \
	PMPackageSelectable.cc
