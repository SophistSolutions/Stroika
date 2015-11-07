#!/usr/bin/perl 

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}







#print (">>>>>>>>******************** STARTING ThirdPartyLibs/Xerces ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf CURRENT");
}

require "../../ScriptsLib/ConfigurationReader.pl";



my $projectPlatformSubdir = GetProjectPlatformSubdir();

print ($projectPlatformSubdir);



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
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=Win32 /target:$BLD_TRG");
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=Win32 /target:$BLD_TRG");
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=x64 /target:$BLD_TRG");
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=x64 /target:$BLD_TRG");
	chdir ("../../../../../../");
}



if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	if (-e "CURRENT/config.status") {
		print ("No need to re-Configure Xerces\n");
	}
	else {
		print ("Configurating Xerces...\n");
		#NB: we disable ICO and CURL because these gave some problem with Gentoo (link error), and
		#not worth tracking down further cuz I don't think we need either -- LGP 2011-09-27
		system ("cd CURRENT ; ./configure --enable-static --disable-shared --without-icu --without-curl");
	}
}

print ("Building Xerces...\n");
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	system ("make --directory CURRENT --no-print-directory -s all");
}
else {
	my $myPlatformSubDir =	GetProjectPlatformSubdir ();
	print ("Target Platform subdir: $myPlatformSubDir\n");
	my $myBinOutDir = '';
	if ($myPlatformSubDir eq 'VisualStudio.Net-2010') {
		$myBinOutDir = 'VC10';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2012') {
		$myBinOutDir = 'VC11';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2013') {
		$myBinOutDir = 'VC12';
	}
	if ($myPlatformSubDir eq 'VisualStudio.Net-2015') {
		$myBinOutDir = 'VC14';
	}
	BuildVCDotNet ($myPlatformSubDir, $myBinOutDir);
}


DONE:

