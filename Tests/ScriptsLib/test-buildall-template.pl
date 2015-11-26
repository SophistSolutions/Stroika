my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $activeConfig = $ENV{'CONFIGURATION'};

require "../../../../ScriptsLib/ConfigurationReader.pl";
require "../../../../Library/Projects/" . GetProjectPlatformSubdir ($activeConfig) . "/SetupBuildCommonVars.pl";

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";

my $useBld =	$BLD_TRG;
if (lc ($useBld) eq "clobber") {
	$useBld = "Clean";
}



my @kConfigurations = (
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					"Configuration=Release-Logging-U-32,Platform=Win32",
					"Configuration=Release-Logging-U-64,Platform=x64",
					"Configuration=Release-DbgMemLeaks-U-32,Platform=Win32"
					);


sub getCFGStr
{
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		if (index($curConfig, $ENV{'CONFIGURATION'}) != -1) {
			return $curConfig;
		}
	}
	die ("unrecognized config");
}

my $curConfig	=	getCFGStr ();


use IPC::Open3;
use Symbol qw(gensym);
use IO::File;
local *CATCHERR = IO::File->new_tmpfile;


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
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
