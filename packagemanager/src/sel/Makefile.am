#
# Makefile.am for packagemanager/src/sel
#

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pmsel.la

liby2pmsel_la_SOURCES = \
	PMSelection.cc \
	PMSelectionDataProvider.cc \
	PMSelectionManager.cc PMSelectionManagerCallbacks.cc
