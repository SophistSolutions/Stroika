my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $activeConfig = $ENV{'CONFIGURATION'};
my $ECHO_BUILD_LINES = $ENV{'ECHO_BUILD_LINES'};


require "../../../../ScriptsLib/ConfigurationReader.pl";
require "../../../../Library/Projects/" . GetProjectPlatformSubdir ($activeConfig) . "/SetupBuildCommonVars.pl";

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";

my $useBld =	$BLD_TRG;
if (lc ($useBld) eq "clobber") {
	$useBld = "Clean";
}

my $curConfig	=	`../../../../ScriptsLib/GetVisualStudioConfigLine.pl $ENV{'CONFIGURATION'}`;


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
		die ("Run result - FAILED - = $result\r\n");
	}
}

sub	DoRunBuilds {
	my $solutionName = $_[0];
	
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS $solutionName /p:$curConfig /target:$useBld");
}

1
