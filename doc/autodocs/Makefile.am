#
# Makefile.am for packagemanager/doc/autodocs
#
htmldir = $(docdir)/autodocs

html_DATA = index.html

EXTRA_DIST = $(html_DATA)

CLEANFILES = $(html_DATA)

SRC_INCLUDEDIRS =	$(addprefix $(top_srcdir)/src/,	\
				include			\
				pkg/include		\
				inst/include		\
				inst/pkg/include	\
				media/include		\
			)

SRC_INCLUDEHEADER =	$(wildcard	\
				$(addsuffix /*.h, $(SRC_INCLUDEDIRS))		\
				$(addsuffix /y2pm/*.h, $(SRC_INCLUDEDIRS))	\
			)

$(filter-out index.html, $(html_DATA)): index.html
#index.html: $(srcdir)/../../src/include/*.h $(srcdir)/../../src/include/y2pm/*.h
#	cd $(srcdir)/../../src/include && @ybindir@/ydoc -p @RPMNAME@ *.h y2pm/*.h
#	mv -f $(srcdir)/../../src/include/*.html .

index.html: $(SRC_INCLUDEHEADER)
	@ybindir@/ydoc -p @RPMNAME@ $(SRC_INCLUDEHEADER)

