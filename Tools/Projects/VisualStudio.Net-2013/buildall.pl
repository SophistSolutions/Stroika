#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

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
	print ("      $cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
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


print("   Building StroikaTools-Frameworks-WebServer-HTMLViewCompiler...\n");
foreach (@kConfigurations) {
	my $curConfig	=	$_;
	RunAndPrint ("msbuild.exe $EXTRA_MSBUILD_ARGS StroikaTools-Frameworks-WebServer-HTMLViewCompiler.vcxproj /p:$curConfig /target:$useBld");
}

