#!/usr/bin/perl

my $BLD_TRG;

foreach $i (@ARGV) {
	if (lc ($i) eq "build" ||
		lc ($i) eq "build+" ||
		lc ($i) eq "clean" ||
		lc ($i) eq "clobber" ||
		lc ($i) eq "rebuild" ||
		lc ($i) eq "rebuild+" ||
		lc ($i) eq "all" ||
		lc ($i) eq "all+" 
		) {
		$BLD_TRG = $i;
	}
	if (	(lc ($i) eq "help") ||
		(lc ($i) eq "-help") || 
		(lc ($i) eq "--help") || 
		(lc ($i) eq "-h") || 
		(lc ($i) eq "-?") || 
		(lc ($i) eq "/?"))
		{
		print ("buildall.pl TARGET\n\t with TARGET = Build, Build+, Clean, Clobber, All, All+, Rebuild, Rebuild+\n");
		exit (0);
	}
}


my $useExtraConfigDefines	=	"";
my $useExtraConfigDefines_LINUX	=	"-c-define '#define qTraceToFile 1'";

if ("$^O" eq "linux") {
    $useExtraConfigDefines = $useExtraConfigDefines_LINUX;
}


if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $runRegressionTests = 0;
if (lc ($BLD_TRG) eq 'build+') {
	$BLD_TRG = 'Build';
	$runRegressionTests = 1;
}
if (lc ($BLD_TRG) eq 'all+') {
	$BLD_TRG = 'all';
	$runRegressionTests = 1;
}
if (lc ($BLD_TRG) eq 'rebuild+') {
	$BLD_TRG = 'rebuild';
	$runRegressionTests = 1;
}


my $isBuildingMode = not ((lc ($BLD_TRG) eq "clobber") or (lc ($BLD_TRG) eq "clean"));


print ("**************************** STARTING Stroika ****************************\n");
system ("sh -c 'date'");

system ("cd ThirdPartyLibs; perl buildall.pl $BLD_TRG");

if ((lc ($BLD_TRG) eq "clobber") || (lc ($BLD_TRG) eq "rebuild")) {
	system ("rm -rf Builds");
	system ("rm -f Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h");
}
if ((lc ($BLD_TRG) eq "clobber") or (lc ($BLD_TRG) eq "clean") or (lc ($BLD_TRG) eq "rebuild")) {
	system ("rm -rf IntermediateFiles");
}

if ($isBuildingMode) {
	system ("perl configure.pl --only-if-unconfigured $useExtraConfigDefines");
}

my $useBld	=	$BLD_TRG;
if (lc ($BLD_TRG) eq "rebuild") {
	$useBld = "Build";	# we already deleted everything above
}

# No point in sub-builds for clean/clobber, since we adequately cleaned with the
# above
if ($isBuildingMode) {
	system ("cd Library; perl buildall.pl $useBld");
	system ("cd Tests; perl buildall.pl $useBld");
}

system ("sh -c 'date'");
if ($isBuildingMode) {
	system ("perl checkall.pl");
}
if ($runRegressionTests) {
	system ("cd Tests; perl Run.pl");
}
print ("**************************** ENDING Stroika ****************************\n");
