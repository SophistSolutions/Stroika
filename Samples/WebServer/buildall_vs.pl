#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";


$activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};
my $level = $ENV{'MAKE_INDENT_LEVEL'};

my $useProjectDir= "../../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}



sub	GetMSBuildArgs {
	my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:NoSummary";
	return $EXTRA_MSBUILD_ARGS;
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
		print (`../../ScriptsLib/PrintLevelLeader.sh $level` . "cmd2Run...\n");
	}
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;

my $useBld = NormalizeBuildArg ($ARGV[0]);
my $level = $ENV{'MAKE_INDENT_LEVEL'};


print(`../../ScriptsLib/PrintLevelLeader.sh $level` . $useBld . "ing Samples/WebServer:\n");
if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64") {
	my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
	my $extraArgs = GetMSBuildArgs();
	RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs WebServer.vcxproj /p:$curConfig /target:$useBld");
}
