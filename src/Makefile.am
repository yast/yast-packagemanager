#
# Makefile.am for packagemanager/src
#

SUBDIRS = include inst pkg media

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		inst/include	\
		media/include	\
	   ) -I$(includedir) -I$(QTDIR)/include

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = \
	PkgName.cc	\
	PkgEdition.cc	\
	PkgRelation.cc	\
	PkgArch.cc	\
	PMError.cc	\
	PMSolvable.cc	\
	PMObject.cc	\
	PMManager.cc	\
	PMSelectable.cc	\
	PMDataProvider.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = -ly2util inst/liby2pminstsrc.la pkg/liby2pmpkg.la media/liby2pmMedia.la
#liby2pm_la_LIBADD = PMSolver/libPMSolver.la PMMgr/libPMMgr.la -ly2util

