#
# Makefile.am for packagemanager/src
#

SUBDIRS = include inst pkg sel you media libPkgDep query

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

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
	   ) -I$(includedir)

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = Y2PM.cc \
	Timecount.cc	\
	PkgName.cc	\
	PkgEdition.cc	\
	PkgRelation.cc	\
	PkgSplit.cc	\
	PkgArch.cc	\
	PkgDu.cc	\
	PMError.cc	\
	PMSolvable.cc	\
	PMObject.cc	\
	PMManager.cc PMManager_solver.cc	\
	PMSelectable.cc	SelState.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = 	pkg/liby2pmpkg.la	\
			sel/liby2pmsel.la	\
			you/liby2pmyou.la	\
			inst/liby2pminstsrc.la	\
			media/liby2pmMedia.la	\
			query/src/liby2pmquery.la\
			libPkgDep/libPkgDep.la	\
			-ly2util -lrpm -lpopt

bin_PROGRAMS = online_update installation_sources

online_update_SOURCES = online_update.cc
online_update_LDADD = liby2pm.la
online_update_LDFLAGS = -static

installation_sources_SOURCES = installation_sources.cc
installation_sources_LDADD = liby2pm.la
installation_sources_LDFLAGS = -static
