#
# Makefile.am for packagemanager/src
#

SUBDIRS = inst pkg sel you media libPkgDep y2pm

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -DLOCALEDIR=\"${localedir}\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

lib_LTLIBRARIES = liby2pm.la

liby2pm_la_SOURCES = Y2PM.cc Y2PMCallbacks.cc \
	Timecount.cc	\
	PkgName.cc	\
	PkgEdition.cc	\
	PkgRelation.cc	\
	PkgSplit.cc	\
	PkgArch.cc	\
	PkgDu.cc	\
	PkgChangelog.cc	\
	PMError.cc	\
	ModulePkgError.cc	\
	PMCallbacks.cc	\
	PMSolvable.cc	\
	PMObject.cc	\
	PMManager.cc PMManager_solver.cc	\
	PMSelectable.cc	SelState.cc

liby2pm_la_LDFLAGS = -version-info 2:0

liby2pm_la_LIBADD = 	pkg/liby2pmpkg.la	\
			sel/liby2pmsel.la	\
			you/liby2pmyou.la	\
			inst/liby2pminstsrc.la	\
			media/liby2pmMedia.la   \
			libPkgDep/libPkgDep.la	\
			-ly2util

include_HEADERS = Y2PM.h

bin_PROGRAMS = online_update installation_sources genIS_PLAINcache

online_update_SOURCES = online_update.cc
online_update_LDADD = liby2pm.la
#online_update_LDFLAGS = -static

installation_sources_SOURCES = installation_sources.cc
installation_sources_LDADD = liby2pm.la
#installation_sources_LDFLAGS = -static

genIS_PLAINcache_SOURCES =	genIS_PLAINcache.cc
genIS_PLAINcache_LDADD =	liby2pm.la
#genIS_PLAINcache_LDFLAGS =	-static


admdir = /var/adm/fillup-templates

adm_DATA = sysconfig.onlineupdate

EXTRA_DIST = $(adm_DATA)
