#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("**************************** STARTING Stroika ****************************\n");
system ("sh -c 'date'");

system ("cd Library; perl buildall.pl $BLD_TRG");

system ("sh -c 'date'");
if ($BLD_TRG ne 'Clean') {
	system ("perl checkall.pl");
}
print ("**************************** ENDING Stroika ****************************\n");
