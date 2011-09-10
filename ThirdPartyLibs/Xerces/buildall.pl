#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}


my $EXTRACTED_DIRNAME	=	"xerces-c-3.1.1";
my $trgDirName	=			"$EXTRACTED_DIRNAME";
my $SLINKDIRNAME	=		"$EXTRACTED_DIRNAME";



if ((lc ("$BLD_TRG") eq "clean") || (lc ("$BLD_TRG") eq "clobber")) {
	system ("rm -rf $trgDirName CURRENT");
	exit (0);
}
if (lc ("$BLD_TRG") eq "rebuild") {
	system ("rm -rf $trgDirName CURRENT");
}

if (-e "CURRENT/src/xercesc/dom/impl/DOMLocatorImpl.hpp") {
	print ("already up to date\n");
	exit (0);
}

print ("Extracting Xerces...\n");

system ("rm -rf xerces-c-3.1.1 CURRENT");
system ("tar xvf Origs/xerces-c-3.1.1.tar.gz");
system ("mv $EXTRACTED_DIRNAME CURRENT");
system ("ln -s CURRENT $SLINKDIRNAME");

print ("Patching Xerces...\n");
system ("patch -t CURRENT/projects/Win32/VC10/xerces-all/XercesLib/XercesLib.vcxproj Patches/XercesLib.vcxproj.PATCH");


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}


if ("$^O" eq "linux") {
	print ("Configurating Xerces...\n");
	system ("cd CURRENT ; ./configure --enable-static --disable-shared");
}

print ("Building Xerces...\n");
if ("$^O" eq "linux") {
	system ("cd CURRENT ; make all");
}
else {
	require "../../Library/Projects/VisualStudio.Net-2010/SetupBuildCommonVars.pl";
	
	chdir ("CURRENT/Projects/Win32/VC10/xerces-all/XercesLib");
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Release\" /target:$BLD_TRG\n");
		RunAndPrint ("MSBuild.exe $EXTRA_MSBUILD_ARGS XercesLib.vcxproj /p:Configuration=\"Static Debug\" /target:$BLD_TRG\n");
	chdir ("../../../../../../");
}

system ("perl checkall.pl");
