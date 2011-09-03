#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building Tests...\n");
system ("cd Projects/VisualStudio.Net-2010/1; perl buildall.pl $BLD_TRG");
system ("cd Projects/VisualStudio.Net-2010/2; perl buildall.pl $BLD_TRG");
system ("cd Projects/VisualStudio.Net-2010/3; perl buildall.pl $BLD_TRG");
system ("cd Projects/VisualStudio.Net-2010/4; perl buildall.pl $BLD_TRG");
system ("cd Projects/VisualStudio.Net-2010/5; perl buildall.pl $BLD_TRG");
