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
		inst/rpm/include\
		media/include	\
		libPkgDep/include\
	   ) -I$(includedir)

noinst_LTLIBRARIES = liby2pmyou.la

liby2pmyou_la_SOURCES = \
	PMYouPatch.cc \
	PMYouPatchManager.cc \
        PMYouPatchInfo.cc \
        PMYouPackageDataProvider.cc \
        PMYouPatchPaths.cc \
        PMYouServers.cc \
        PMYouPatchTags.cc \
        InstYou.cc \
        YouError.cc

EXTRA_DIST = youservers
