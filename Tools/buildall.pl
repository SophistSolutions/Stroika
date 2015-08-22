#!/usr/bin/perl 

require "../ScriptsLib/ConfigurationReader.pl";

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building Stroika Tools...\n");
my $projectPlatformSubdir = GetProjectPlatformSubdir();

my $useBld = lc ($BLD_TRG);
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	if ($useBld eq "rebuild") {
		system ("make --directory ../IntermediateFiles/DefaultConfiguration/Tools/Frameworks/ --no-print-directory clobber");
		$useBld = "all";
	}
    system ("make --directory ../IntermediateFiles/DefaultConfiguration/Tools/Frameworks/ --no-print-directory $useBld");
}
else {
    my $useProjectDir= "Projects/" . $projectPlatformSubdir;
    system ("cd $useProjectDir; perl buildall.pl $useBld");
}
print ("Building Stroika Tools...done\n");
