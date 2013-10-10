#!/usr/bin/perl 

require "../ScriptsLib/BuildUtils.pl";
require "../ScriptsLib/ConfigurationReader.pl";

my $activeConfig = GetActiveConfigurationName ();

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
		RunAndStopOnFailure ("make --directory ../IntermediateFiles/$activeConfig/Library/Foundation/ --no-print-directory clobber MAKEFLAGS=");
		RunAndStopOnFailure ("make --directory ../IntermediateFiles/$activeConfig/Library/Frameworks/ --no-print-directory clobber MAKEFLAGS=");
		$useBld = "all";
	}
	RunAndStopOnFailure ("make --directory ../IntermediateFiles/$activeConfig/Library/Foundation/ --no-print-directory $useBld MAKEFLAGS=");
	RunAndStopOnFailure ("make --directory ../IntermediateFiles/$activeConfig/Library/Frameworks/ --no-print-directory $useBld MAKEFLAGS=");
}
else {
    	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
    	RunAndStopOnFailure ("cd $useProjectDir; perl buildall.pl $useBld");
}
