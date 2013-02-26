#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

system ("cd Xerces; perl buildall.pl $BLD_TRG");
system ("cd doxygen; perl buildall.pl $BLD_TRG");
system ("cd astyle; perl buildall.pl $BLD_TRG");
