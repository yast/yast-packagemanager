#
# Makefile.am for packagemanager/src/pkg
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

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
