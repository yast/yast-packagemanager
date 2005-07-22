#
# Makefile.am
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmLanguageManager.la

liby2pmLanguageManager_la_SOURCES = \
	PMLanguage.cc \
	PMLanguageManager.cc \
	PMLanguageManagerCallbacks.cc
