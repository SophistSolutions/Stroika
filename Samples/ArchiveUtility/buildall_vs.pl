#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";

my $activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};
my $level = $ENV{'MAKE_INDENT_LEVEL'};

my $useProjectDir= "../../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}





sub	NormalizeBuildArg {
	my $useBld =	$_[0];
	if ($useBld eq '') {
		$useBld = 'Build';
	}
	if (lc ($useBld) eq "clobber") {
		$useBld = "Clean";
	}
	return $useBld;
}


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	if ($ECHO_BUILD_LINES == 1) {
		print (`../../ScriptsLib/PrintLevelLeader.sh $level` . "$cmd2Run...\n");
	}
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}


my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;
my $level = $ENV{'MAKE_INDENT_LEVEL'};

my $useBld = NormalizeBuildArg ($ARGV[0]);


# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};


print(`../../ScriptsLib/PrintLevelLeader.sh $level` . $useBld . "ing Samples/ArchiveUtility...\n");
if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64" || $activeConfig eq "Release-Logging-U-64" || $activeConfig eq "Release-DbgMemLeaks-U-32") {
	my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
	my $extraArgs = "/nologo /v:quiet /clp:NoSummary";
	RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs ArchiveUtility.vcxproj /p:$curConfig /target:$useBld");
}
