#
# Makefile.am for packagemanager/src/pkg
#

SUBDIRS = include

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		media/include	\
		libPkgDep/include	\
	   ) -I$(includedir)

noinst_LTLIBRARIES = liby2pmpkg.la

liby2pmpkg_la_SOURCES = \
	PMPackage.cc \
	PMPackageDataProvider.cc \
	PMPackageManager.cc
