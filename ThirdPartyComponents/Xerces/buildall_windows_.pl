#!/usr/bin/perl 

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}



require "../../ScriptsLib/ConfigurationReader.pl";


my $activeConfig = $ENV{'CONFIGURATION'};;
my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);



sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}

sub BuildVCDotNet
{
	my $PROJVVCVERSUBDIR = $_[0];
	my $SHORTVCVERDIR = $_[1];

	my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";
	require "../../Library/Projects/$PROJVVCVERSUBDIR/SetupBuildCommonVars.pl";
	
	chdir ("CURRENT/Projects/Win32/$SHORTVCVERDIR/xerces-all/XercesLib");
		if ($activeConfig eq "Debug-U-32") {
			RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=Win32 /target:$BLD_TRG");
		}
		if ($activeConfig eq "Release-U-32" || $activeConfig eq "Release-DbgMemLeaks-U-32" || $activeConfig eq "Release-Logging-U-32") {
			RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=Win32 /target:$BLD_TRG");
		}
		if ($activeConfig eq "Debug-U-64") {
			RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=x64 /target:$BLD_TRG");
		}
		if ($activeConfig eq "Release-U-64" || $activeConfig eq "Release-Logging-U-64") {
			RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=x64 /target:$BLD_TRG");
		}
	chdir ("../../../../../../");
}



print ("Building Xerces...\n");
my $myPlatformSubDir =	GetProjectPlatformSubdir ($activeConfig);
my $myBinOutDir = '';
if ($myPlatformSubDir eq 'VisualStudio.Net-2013') {
	$myBinOutDir = 'VC12';
}
if ($myPlatformSubDir eq 'VisualStudio.Net-2015') {
	$myBinOutDir = 'VC14';
}
if ($myPlatformSubDir eq 'VisualStudio.Net-2017') {
	$myBinOutDir = 'VC15';
}
BuildVCDotNet ($myPlatformSubDir, $myBinOutDir);
