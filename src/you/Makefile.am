#
# Makefile.am for packagemanager/src/pkg
#

SUBDIRS = include

# first look into source tree, then into $(includedir)
INCLUDES = $(addprefix -I$(top_srcdir)/src/,	\
		include	\
		pkg/include	\
		sel/include	\
		you/include	\
		inst/include	\
		inst/rpm/include\
		media/include	\
	   ) -I$(includedir) -I$(includedir)/y2util/qt

noinst_LTLIBRARIES = liby2pmyou.la

liby2pmyou_la_SOURCES = \
	PMYouPatch.cc \
	PMYouPatchDataProvider.cc \
	PMYouPatchManager.cc \
        PMYouPatchInfo.cc \
        PMYouPackageDataProvider.cc \
        PMYouPatchPaths.cc \
        InstYou.cc \
        YouError.cc
