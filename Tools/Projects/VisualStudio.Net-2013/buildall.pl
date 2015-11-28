#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};
my $level = $ENV{'MAKE_INDENT_LEVEL'};


require "../../../Library/Projects/VisualStudio.Net-2013/SetupBuildCommonVars.pl";

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
		print "Run result = $result\r\n";
	}
}



print(`../../../ScriptsLib/PrintLevelLeader.sh $level` . "Building StroikaTools-Frameworks-WebServer-HTMLViewCompiler:\n");
my $curConfig	=	`../../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS StroikaTools-Frameworks-WebServer-HTMLViewCompiler.vcxproj /p:$curConfig /target:$useBld");

