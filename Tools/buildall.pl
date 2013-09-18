#!/usr/bin/perl 

require "../ScriptsLib/ConfigurationReader.pl";

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building tools...\n");

my $useBld = lc ($BLD_TRG);
if ("$^O" eq "linux") {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	if ($useBld eq "rebuild") {
		system ("make --directory ../IntermediateFiles/Platform_Linux/Debug/Tools/Frameworks/ --no-print-directory clobber");
		$useBld = "all";
	}
    system ("make --directory ../IntermediateFiles/Platform_Linux/Debug/Tools/Frameworks/ --no-print-directory $useBld");
}
else {
    my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
    system ("cd $useProjectDir; perl buildall.pl $useBld");
}
