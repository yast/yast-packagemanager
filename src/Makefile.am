#
# Makefile.am for packagemanager/src
#

SUBDIRS = include inst pkg sel you media libPkgDep query

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include			\
		query/include		\
		pkg/include		\
		sel/include		\
		you/include		\
		inst/include		\
		inst/rpm/include	\
		media/include		\
		libPkgDep/include	\
	   ) -I$(includedir) -I$(includedir)/y2util/qt

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = Y2PM.cc \
	PkgName.cc	\
	PkgEdition.cc	\
	PkgRelation.cc	\
	PkgArch.cc	\
	PMError.cc	\
	PMSolvable.cc	\
	PMObject.cc	\
	PMManager.cc	\
	PMSelectable.cc	SelState.cc \
	PMDataProvider.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = 	pkg/liby2pmpkg.la	\
			sel/liby2pmsel.la	\
			you/liby2pmyou.la	\
			inst/liby2pminstsrc.la	\
			media/liby2pmMedia.la	\
			query/src/liby2pmquery.la\
			libPkgDep/libPkgDep.la	\
			-ly2util

bin_PROGRAMS = online_update

online_update_SOURCES = online_update.cc
online_update_LDADD = liby2pm.la
online_update_LDFLAGS = -static
