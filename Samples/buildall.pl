#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building Samples...\n");

my $useBld = lc ($BLD_TRG);
system ("cd ActiveLedIt; perl buildall.pl");
system ("cd LedIt; perl buildall.pl");
system ("cd LedLineIt; perl buildall.pl");
system ("cd SimpleLedTest; perl buildall.pl");
system ("cd SSDPClient; perl buildall.pl");
system ("cd SSDPServer; perl buildall.pl");
system ("cd SimpleService; perl buildall.pl");
system ("cd SystemPerformanceClient; perl buildall.pl");
system ("cd WebServer; perl buildall.pl");
