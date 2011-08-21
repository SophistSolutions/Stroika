#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("**************************** STARTING Stroika ****************************\n");
system ("sh -c 'date'");

system ("cd Library; perl buildall.pl $BLD_TRG");
system ("cd Tests; perl buildall.pl $BLD_TRG");

system ("sh -c 'date'");
if (lc ($BLD_TRG) ne 'clean') {
	system ("perl checkall.pl");
	system ("cd Tests; perl checkall.pl");
	system ("cd Tests; perl run.pl");
}
print ("**************************** ENDING Stroika ****************************\n");
