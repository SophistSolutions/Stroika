#!/usr/bin/perl 


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

if ("$^O" eq "linux") {
	$DoCreateSymLink = 1;
}


print (">>>>>>>>******************** STARTING ThirdPartyLibs/Xerces ******************\n");

if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf $trgDirName CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf $trgDirName CURRENT");
}

if (-e "CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp") {
	print ("already up to date\n");
	goto DONE;
	exit (0);
}

require "../../ScriptsLib/ConfigurationReader.pl";


if (not -e "../Origs-Cache/$BASENAME.tar.gz") {
	#my $MIRROR_PREFIX_ = "http://www.fightrice.com/mirrors/apache/";
	my $MIRROR_PREFIX_ = "http://psg.mtu.edu/pub/apache/";
	system ("wget --tries=10 --output-document=../Origs-Cache/$BASENAME.tar.gz $MIRROR_PREFIX_/xerces/c/3/sources/$BASENAME.tar.gz");
}


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
#For some reason, patch not working with dos-format files, so map to unix (and then return)
if ("$^O" eq "cygwin") {
	system ("dos2unix CURRENT/projects/Win32/VC10/xerces-all/XercesLib/XercesLib.vcxproj Patches/XercesLib.vcxproj.PATCH");
}
system ("patch -t CURRENT/projects/Win32/VC10/xerces-all/XercesLib/XercesLib.vcxproj Patches/XercesLib.vcxproj.PATCH");
if ("$^O" eq "cygwin") {
	system ("unix2dos CURRENT/projects/Win32/VC10/xerces-all/XercesLib/XercesLib.vcxproj Patches/XercesLib.vcxproj.PATCH");
}
system ("cd CURRENT; tar xf ../Patches/VC11Projects.tar.gz");
system ("cd CURRENT; tar xf ../Patches/VC12Projects.tar.gz");


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
	RunAndPrint ("rm -rf 'CURRENT/Build/Win32/$SHORTVCVERDIR/Static Debug/obj/'");
	RunAndPrint ("rm -rf 'CURRENT/Build/Win32/$SHORTVCVERDIR/Static Release/obj/'");
	RunAndPrint ("rm -rf 'CURRENT/Build/Win64/$SHORTVCVERDIR/Static Debug/obj/'");
	RunAndPrint ("rm -rf 'CURRENT/Build/Win64/$SHORTVCVERDIR/Static Release/obj/'");
}



if ("$^O" eq "linux") {
	print ("Configurating Xerces...\n");
	#NB: we disable ICO and CURL because these gave some problem with Gentoo (link error), and
	#	not worth tracking down further cuz I don't think we need either -- LGP 2011-09-27
	system ("cd CURRENT ; ./configure --enable-static --disable-shared --without-icu --without-curl");
}

print ("Building Xerces...\n");
if ("$^O" eq "linux") {
	system ("cd CURRENT ; make -s all");
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
	BuildVCDotNet ($myPlatformSubDir, $myBinOutDir);
}

system ("perl checkall.pl");

DONE:
print (">>>>>>>>******************** ENDING ThirdPartyLibs/Xerces ******************\n");

