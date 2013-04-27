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
        #
        # NOT SURE why MAKEFLAGS= was needed, but when invoked from
        # top level makefile would fail otherwise. Low priroty to fix since I may redo the build system
        # again using ant...
        #       -- LGP 2013-04-27
        #
	if ($useBld eq "rebuild") {
                RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make clobber MAKEFLAGS=");
                RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make clobber MAKEFLAGS=");
		$useBld = "all";
	}
        RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make $useBld MAKEFLAGS=");
        RunAndStopOnFailure ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make $useBld MAKEFLAGS=");
}
else {
    	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
    	RunAndStopOnFailure ("cd $useProjectDir; perl buildall.pl $useBld");
}
