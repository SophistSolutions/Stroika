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
		lc ($i) eq "all+" ||
		lc ($i) eq "docs" 
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



my $useExtraConfigDefines	=	"--default-for-platform";

my $buildDocs = 0;

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


if ((lc ($BLD_TRG) eq 'docs') or (lc ($BLD_TRG) eq 'all') or (lc ($BLD_TRG) eq 'all+')
	or (lc ($BLD_TRG) eq 'build') or (lc ($BLD_TRG) eq 'rebuild+')
	) {
	$buildDocs = 1;
}


my $isBuildingMode  = 0;
if ((lc ($BLD_TRG) eq 'all') or (lc ($BLD_TRG) eq 'all+') or
    (lc ($BLD_TRG) eq 'build') or (lc ($BLD_TRG) eq 'build+') or
    (lc ($BLD_TRG) eq 'rebuild') or (lc ($BLD_TRG) eq 'rebuild+')
    ) {
	$isBuildingMode = 1;
}




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
	system ("cd Tools; perl buildall.pl $useBld");
	system ("cd Samples; perl buildall.pl $useBld");
	system ("cd Tests; perl buildall.pl $useBld");
}

if ($buildDocs) {
	system ("cd Documentation; cd Doxygen; perl ./RunDoxygen.pl");
}

system ("sh -c 'date'");
if ($isBuildingMode) {
	system ("perl checkall.pl");
}
if ($runRegressionTests) {
	system ("cd Tests; perl Run.pl");
}
print ("**************************** ENDING Stroika ****************************\n");
