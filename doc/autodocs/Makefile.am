#
# Makefile.am for packagemanager/doc/autodocs
#

htmldir = $(docdir)/autodocs

html_DATA = index.html

EXTRA_DIST = $(html_DATA)

CLEANFILES = $(html_DATA)

$(filter-out index.html, $(html_DATA)): index.html
index.html: $(srcdir)/../../src/include/*.h $(srcdir)/../../src/include/y2pm/*.h
	cd $(srcdir)/../../src/include && @ybindir@/ydoc -p @RPMNAME@ *.h y2pm/*.h
	mv -f $(srcdir)/../../src/include/*.html .
