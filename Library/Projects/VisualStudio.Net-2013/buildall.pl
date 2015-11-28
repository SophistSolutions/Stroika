#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};
my $curConfig   =       `../../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
my $level = $ENV{'MAKE_INDENT_LEVEL'} + 1;

require "SetupBuildCommonVars.pl";

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";


my $useBld =	$BLD_TRG;
if (lc ($useBld) eq "clobber") {
	$useBld = "Clean";
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


print(`../../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Stroika-Foundation:\n");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Foundation.vcxproj /p:$curConfig /target:$useBld");

print(`../../../ScriptsLib/PrintLevelLeader.sh $level` . "Building Stroika-Frameworks:\n");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Led.vcxproj /p:$curConfig /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-Service.vcxproj /p:$curConfig /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebServer.vcxproj /p:$curConfig /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks-WebService.vcxproj /p:$curConfig /target:$useBld");
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS Stroika-Frameworks.vcxproj /p:$curConfig /target:$useBld");
