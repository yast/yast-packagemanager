#
# Makefile.am for packagemanager/doc/PHI

all-local:
	echo "Either run 'make dvi-local' or 'make ps-local'"
dvi-local: dstrukt-entwurf.dvi libPkgDb.dvi libPkgDep.dvi
ps-local: dstrukt-entwurf.ps libPkgDb.ps libPkgDep.ps

clean-local:
	rm -f *.dvi *.aux *.log

%.ps: %.dvi
	dvips $< -o

%.dvi: %.tex
	$(top_srcdir)/doc/Solver/run-tex.pl latex $<

%.eps: %.fig
	fig2dev -L ps $< $@

pathmacros.tex: $(top_srcdir)/doc/Solver/gen-pathmacros.pl $(top_srcdir)/src/libPkgDb/paths.h
	$(top_srcdir)/doc/Solver/gen-pathmacros.pl

libPkgDb.tex: relationmatch.eps pathmacros.tex header.eps

# also pack generated *.ps and *.eps to dist
# so we dont need TeX for building
EXTRA_DIST =

# contains obsolete libPkgDb stuff
# \
	dstrukt-entwurf.tex	\
	libPkgDb.tex		\
	libPkgDep.tex		\
	macros.tex		\
	header.fig		\
	relationmatch.fig	\
	phi-update.txt		\
	gen-pathmacros.pl	\
	run-tex.pl		\
	header.eps		\
	relationmatch.eps	\
	dstrukt-entwurf.ps	\
	libPkgDb.ps		\
	libPkgDep.ps

