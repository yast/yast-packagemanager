#
# Makefile.am for packagemanager/src
#

#SUBDIRS = include inst pkg sel you media libPkgDep query
SUBDIRS = include media inst pkg sel you  libPkgDep

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		pkg/include		\
		sel/include	\
		you/include	\
		inst/include		\
		inst/rpm/include	\
		media/include		\
	   ) -I$(includedir) -I$(QTDIR)/include

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = Y2PM.cc \
	PkgName.cc	\
	PkgEdition.cc	\
	PkgRelation.cc	\
	PkgArch.cc	\
	PkgAttributeValue.cc	\
	PMError.cc	\
	PMSolvable.cc	\
	PMObject.cc	\
	PMManager.cc	\
	PMSelectable.cc	\
	PMDataProvider.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = 	pkg/liby2pmpkg.la	\
			sel/liby2pmsel.la	\
			you/liby2pmyou.la	\
			inst/liby2pminstsrc.la	\
			media/liby2pmMedia.la	\
			libPkgDep/libPkgDep.la	\
			-ly2util

