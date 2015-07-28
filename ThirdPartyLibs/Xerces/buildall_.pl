#!/usr/bin/perl 

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}


my $BASENAME	=	"xerces-c-3.1.1";

my $EXTRACTED_DIRNAME	=	$BASENAME;
my $trgDirName	=			$BASENAME;
my $SLINKDIRNAME	=		$BASENAME;

# DoCreateSymLink - this isn't currently used, and doesn't work well with windows (sets sys file attribute which casues
# DOS delete file issues) - and slink doesnt really work except in cygwin tools
my $DoCreateSymLink = 0;

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	$DoCreateSymLink = 1;
}


#print (">>>>>>>>******************** STARTING ThirdPartyLibs/Xerces ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf $trgDirName CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf $trgDirName CURRENT");
}

if (not -e "../Origs-Cache/$BASENAME.tar.gz") {
	#my $MIRROR_PREFIX_ = "http://www.fightrice.com/mirrors/apache/";
	my $MIRROR_PREFIX_ = "http://psg.mtu.edu/pub/apache/";
	system ("wget --quiet --tries=10 --output-document=../Origs-Cache/$BASENAME.tar.gz $MIRROR_PREFIX_/xerces/c/3/sources/$BASENAME.tar.gz || (rm -f ../Origs-Cache/$BASENAME.tar.gz && false)");
}
	

require "../../ScriptsLib/ConfigurationReader.pl";


# Only extract if its not already extracted. Make clobber if its partly 
# extracted
if (not (-e "CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp")) {
	
	print ("Extracting Xerces...\n");
	
	system ("rm -rf $trgDirName CURRENT");
	system ("tar xf ../Origs-Cache/$BASENAME.tar.gz 2> /dev/null");
	sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
	system ("mv $EXTRACTED_DIRNAME CURRENT");
	sleep(1);  # hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
	if ($DoCreateSymLink) {
		system ("ln -s CURRENT $SLINKDIRNAME");
	}
	
	print ("Patching Xerces...\n");
	system ("cd CURRENT; tar xf ../Patches/VC11Projects.tar.gz");
	system ("cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz");
	system ("chmod -R +rw CURRENT/projects");
	system ("chmod -R +rwx CURRENT/projects CURRENT/projects/Win32 CURRENT/projects/Win32/VC* CURRENT/projects/Win32/VC*/xerces-all CURRENT/projects/Win32/VC*/xerces-all/*");
}




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

	# cleaning needless objs (leave libs)
	#RunAndPrint ("rm -rf 'CURRENT/Build/Win32/$SHORTVCVERDIR/Static Debug/obj/'");
	#RunAndPrint ("rm -rf 'CURRENT/Build/Win32/$SHORTVCVERDIR/Static Release/obj/'");
	#RunAndPrint ("rm -rf 'CURRENT/Build/Win64/$SHORTVCVERDIR/Static Debug/obj/'");
	#RunAndPrint ("rm -rf 'CURRENT/Build/Win64/$SHORTVCVERDIR/Static Release/obj/'");
}



if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
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
if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
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
		$myBinOutDir = 'VC12';
	}
	BuildVCDotNet ($myPlatformSubDir, $myBinOutDir);
}

#system ("perl checkall.pl");

DONE:
#print (">>>>>>>>******************** ENDING ThirdPartyLibs/Xerces ******************\n");

