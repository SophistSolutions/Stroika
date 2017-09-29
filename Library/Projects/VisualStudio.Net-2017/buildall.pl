#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require "SetupBuildCommonVars.pl";

#my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";
my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:NoSummary";


my $activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};
my $curConfig   =       `../../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
my $level = $ENV{'MAKE_INDENT_LEVEL'} + 1;

my $useBld =	$BLD_TRG;
if (lc ($useBld) eq "clean") {
	$useBld = "Clean";
}
if (lc ($useBld) eq "clobber") {
	$useBld = "Clobber";
}


use IPC::Open3;
use Symbol qw(gensym);
use IO::File;
local *CATCHERR = IO::File->new_tmpfile;


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	if ($ECHO_BUILD_LINES == 1) {
		print ("      $cmd2Run...\n");
	}
	my $result = system ($cmd2Run);
	if ($result != 0) {
		die ("Run result = $result\r\n");
	}
}

my $cmdUseBld = $useBld;
if (lc ($cmdUseBld) eq "clobber") {
	$cmdUseBld = "Clean";
}

print(`../../../ScriptsLib/PrintLevelLeader.sh $level` . $useBld . "ing Stroika-Foundation:\n");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "MSBuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.vcxproj /p:$curConfig /target:$cmdUseBld");

print(`../../../ScriptsLib/PrintLevelLeader.sh $level` . $useBld . "ing Stroika-Frameworks:\n");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "MSBuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Led.vcxproj /p:$curConfig /target:$cmdUseBld");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Service.vcxproj /p:$curConfig /target:$cmdUseBld");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-SystemPerformance.vcxproj /p:$curConfig /target:$cmdUseBld");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebServer.vcxproj /p:$curConfig /target:$cmdUseBld");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebService.vcxproj /p:$curConfig /target:$cmdUseBld");
RunSystemWithVCVarsSetInEnvironment ($curConfig, "msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks.vcxproj /p:$curConfig /target:$cmdUseBld");
