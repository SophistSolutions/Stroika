#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

system ("cd 1; perl buildall.pl $BLD_TRG");
system ("cd 2; perl buildall.pl $BLD_TRG");
system ("cd 3; perl buildall.pl $BLD_TRG");
system ("cd 4; perl buildall.pl $BLD_TRG");
