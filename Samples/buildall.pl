#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building Samples...\n");

my $useBld = lc ($BLD_TRG);
system ("cd ActiveLedit; perl buildall.pl");
system ("cd Ledit; perl buildall.pl");
system ("cd SimpleLedTest; perl buildall.pl");
system ("cd WebServer; perl buildall.pl");
