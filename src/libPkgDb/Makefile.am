## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libPkgDb.la

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
	   ) -I$(includedir) -I$(QTDIR)/include

#LINK = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
#libPkgDb_la_LIBADD = @Z_LIBS@
#libPkgDb_la_LDFLAGS = -release @VERSION@ -rpath $(libdir)
#libPkgDb_la_LIBADD = /usr/lib/liby2util.la

libPkgDb_la_SOURCES = Package.cc PkgDb.cc PkgSet.cc PkgName.cc	\
		      PkgEdition.cc PkgRelation.cc DbHeader.cc	\
		      Exception.cc misc.cc pkgdbheader.c paths.cc \
		      PackageSource.cc Solvable.cc \
		      Package.h DbHeader.h Exception.h \
		      DistTag.h misc.h pkgdbheader.h paths.h

# Local Variables:
# tab-width: 4
# End:
