#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("**************************** STARTING Tests ****************************\n");
system ("sh -c 'date'");

system ("cd 1; perl buildall.pl $BLD_TRG");

system ("sh -c 'date'");
print ("**************************** ENDING Tests ****************************\n");
