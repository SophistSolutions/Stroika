#!/usr/bin/perl

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

if (	(lc ($BLD_TRG) eq "help") ||
	(lc ($BLD_TRG) eq "-help") || 
	(lc ($BLD_TRG) eq "-h") || 
	(lc ($BLD_TRG) eq "-?") || 
	(lc ($BLD_TRG) eq "/?"))
	{
	print ("buildall.pl TARGET\n\t with TARGET = Clean, Clobber, All, or Rebuild\n");
	exit (0);
}

my $isBuildingMode = not ((lc ($BLD_TRG) eq "clobber") or (lc ($BLD_TRG) eq "clean"));
my $runRegressionTests = $isBuildingMode;


print ("**************************** STARTING Stroika ****************************\n");
system ("sh -c 'date'");

if ((lc ($BLD_TRG) eq "clobber") || (lc ($BLD_TRG) eq "rebuild")) {
	system ("rm -rf Builds");
	system ("rm -f Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h");
}
if ((lc ($BLD_TRG) eq "clobber") or (lc ($BLD_TRG) eq "clean") or (lc ($BLD_TRG) eq "rebuild")) {
	system ("rm -rf IntermediateFiles");
}

if ($isBuildingMode) {
	system ("perl configure.pl --only-if-unconfigured");
}

system ("cd Library; perl buildall.pl $BLD_TRG");
system ("cd Tests; perl buildall.pl $BLD_TRG");

system ("sh -c 'date'");
if ($isBuildingMode) {
	system ("perl checkall.pl");
}
if ($runRegressionTests) {
	system ("cd Tests; perl Run.pl");
}
print ("**************************** ENDING Stroika ****************************\n");
