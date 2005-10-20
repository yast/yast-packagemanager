#!/usr/bin/perl

open( F, "< ../libPkgDb/paths.h" )
	or die "Can't open ../libPkgDb/paths.h: $!\n";
open( OUT, "> pathmacros.tex" )
	or die "Can't create pathmacros.tex: $!\n";

while( <F> ) {
	next unless (	/^#define\s+(\w+)\s+.*[\/]([^\/]*)"\s*$/ ||
			/^#define\s+(\w+)\s+"(\S+)"\s*$/
	);
	my ($name, $val) = ($1, $2);
	$name =~ s/_//g;
	$name =~ s/2/TO/g;
	print OUT "\\def\\$name\{\\file\{$val\}\}\n";
}

close( F );
close( OUT );
exit 0;
