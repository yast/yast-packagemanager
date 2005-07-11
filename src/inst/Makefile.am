#
# Makefile.am for packagemanager/src/instsrc
#

SUBDIRS = rpm srcYUM

AM_CPPFLAGS = -DY2LOG=\"packagemanager\" -Wall
AM_LDFLAGS = -L$(libdir)

# first look into source tree, then into $(includedir)
INCLUDES = -I$(top_srcdir)/src -I$(includedir)

noinst_LTLIBRARIES = liby2pminstsrc.la

liby2pminstsrc_la_SOURCES = \
	PkgPubkeyCache.cc		\
	InstSrcManager.cc InstSrcManagerCallbacks.cc	\
	\
	InstSrc.cc 			\
	InstData.cc 			\
	InstSrcError.cc			\
	InstSrcDescr.cc 		\
	InstSrcData.cc			\
	\
	InstSrcDataUL.cc		\
	ULSelectionParser.cc		\
	ULPackagesParser.cc		\
	ULParsePackagesLang.cc		\
	PMULPackageDataProvider.cc	\
	PMULSelectionDataProvider.cc	\
	F_Media.cc \
	\
	InstSrcDataPLAIN.cc		\
	PMPLAINPackageDataProvider.cc	\
	\
	InstTarget.cc			\
	InstTargetError.cc		\
	InstTargetCallbacks.cc		\
	InstTargetSelDB.cc		\
	InstTargetProdDB.cc		\
	UpdateInfParser.cc

liby2pminstsrc_la_LIBADD = rpm/liby2pminstrpmsrc.la
