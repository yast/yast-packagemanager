#!/usr/bin/perl
#
# run TeX until there are no messages to rerun TeX anymore

my $rv;
my $redo;
do {
	my $output = "";
	my $pid = open( PIPE, "-|" );
	if ($pid) {
		while( <PIPE> ) {
			$output .= $_;
			print;
		}
		close( PIPE );
		$rv = $?
	}
	else {
		open( STDERR, ">&STDOUT" );
		exec @ARGV;
		die "Cannot exec $ARGV[0]: $!\n";
	}

	$redo = 0;
	if ($rv == 0 && $output =~ /^latex warning:.*rerun/mi) {
		print "\nRe-running TeX for changed labels.\n\n";
		$redo = 1;
	}
	
} while( $redo );

exit $rv;
