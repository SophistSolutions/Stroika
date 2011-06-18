#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("**************************** STARTING Stroika ****************************\n");
system ("sh -c 'date'");

system ("cd Projects/VisualStudio.Net-2010; perl buildall.pl $BLD_TRG");

system ("sh -c 'date'");
system ("perl checkall.pl");
print ("**************************** ENDING Stroika ****************************\n");
