#
# Makefile.am for packagemanager/src
#

SUBDIRS = include

INCLUDES = -I$(QTDIR)/include -I$(top_srcdir)/src/include -I$(includedir)

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = \
	PkgName.cc PkgEdition.cc PkgRelation.cc	PkgArch.cc	\
	PMError.cc	\
	\
	PMSolvable.cc	\
		PMObject.cc	\
			PMPackage.cc	\
	\
	PMManager.cc	\
		PMPackageManager.cc	\
	\
	PMSelectable.cc	\
		PMPackageSelectable.cc	\
	\
	InstSrcManager.cc	\
	\
	InstSrc.cc	\
	InstSrcData.cc	\
	InstSrcDescr.cc	\
	\
	PMDataProvider.cc	\
		PMPackageDataProvider.cc	\
	\
	MediaInfo.cc	\
		FAKEMediaInfo.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = -ly2util
#liby2pm_la_LIBADD = PMSolver/libPMSolver.la PMMgr/libPMMgr.la -ly2util

