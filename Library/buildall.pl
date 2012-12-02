#!/usr/bin/perl 

require "../ScriptsLib/BuildUtils.pl";
require "../ScriptsLib/ConfigurationReader.pl";

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building library...\n");

my $useBld = lc ($BLD_TRG);
if ("$^O" eq "linux") {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	if ($useBld eq "rebuild") {
	    	RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make clobber");
	    	RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make clobber");
		$useBld = "all";
	}
    	RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make $useBld");
    	RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make $useBld");
}
else {
    	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
    	RunAndStopOnFailure ("cd $useProjectDir; perl buildall.pl $useBld");
}
