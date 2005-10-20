#
# Makefile.am for packagemanager/src/pkg
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -DY2_UPDATE_SCRIPT_ARGUMENTS_FILE=\"/var/adm/YaST/you_update_arguments\" -Wall

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmyou.la

liby2pmyou_la_SOURCES = \
	PMYouPatch.cc \
	PMYouPatchManager.cc \
        PMYouPatchInfo.cc \
        PMYouPackageDataProvider.cc \
        PMYouSettings.cc \
        PMYouServers.cc \
        PMYouPatchTags.cc \
        InstYou.cc \
        YouError.cc \
        PMYouProduct.cc \
        PMYouMedia.cc

ybin_SCRIPTS = create_you_patch

EXTRA_DIST = youservers $(ybin_SCRIPTS)

dist_bin_SCRIPTS = machid_hash
