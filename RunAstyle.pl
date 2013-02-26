#!/usr/bin/perl

my $astyleEXE	=	"astyle";


my $astyleEXE_GCC	=	"ThirdPartyLibs/astyle/CURRENT/AStyle/build/gcc/bin/astyle";
if (-e "$astyleEXE_GCC") {
	$astyleEXE	=	$astyleEXE_GCC;
}
my $astyleEXE_GCC	=	"ThirdPartyLibs/astyle/CURRENT/AStyle/build/gcc/bin/astyle.exe";
if (-e "$astyleEXE_GCC") {
	$astyleEXE	=	$astyleEXE_GCC;
}

my $runFile;

my $aStyleArgs = "";


$aStyleArgs .= "--style=stroustrup ";
#$aStyleArgs .= "--style=whitesmith ";
#$aStyleArgs .= "--style=kr ";
#$aStyleArgs .= "--style=java ";

$aStyleArgs .= "--convert-tabs ";
$aStyleArgs .= "--indent-namespaces ";
$aStyleArgs .= "--indent-cases ";
$aStyleArgs .= "--indent-switches ";
$aStyleArgs .= "--indent-preprocessor ";
$aStyleArgs .= "--pad-oper ";
$aStyleArgs .= "--break-closing-brackets ";
$aStyleArgs .= "--keep-one-line-blocks ";
$aStyleArgs .= "--indent=spaces ";
$aStyleArgs .= "--preserve-date ";

$aStyleArgs .= "--align-pointer=type ";
$aStyleArgs .= "--align-reference=type ";
$aStyleArgs .= "--mode=c ";
$aStyleArgs .= "--suffix=none ";




foreach $i (@ARGV) {
	$runFile = $i;
}


if ($runFile == "") {
	## Had to break the run of system() into parts, cuz otherwise params
	## at end cutoff on cygwin/Windows 8...
	##
	
	my $aStyleFileTargets = "";
	$aStyleFileTargets .= "Library/*.cpp ";
	$aStyleFileTargets .= "Library/*.h ";
	$aStyleFileTargets .= "Library/*.inl ";
	system ("$astyleEXE $aStyleArgs --recursive $aStyleFileTargets");
	
	my $aStyleFileTargets = "";
	$aStyleFileTargets .= "Samples/*.cpp ";
	$aStyleFileTargets .= "Samples/*.h ";
	$aStyleFileTargets .= "Samples/*.inl ";
	system ("$astyleEXE $aStyleArgs --recursive $aStyleFileTargets");
	
	my $aStyleFileTargets = "";
	$aStyleFileTargets .= "Tests/*.cpp ";
	$aStyleFileTargets .= "Tests/*.h ";
	$aStyleFileTargets .= "Tests/*.inl ";
	system ("$astyleEXE $aStyleArgs --recursive $aStyleFileTargets");
	
	my $aStyleFileTargets = "";
	$aStyleFileTargets .= "Tools/*.cpp ";
	$aStyleFileTargets .= "Tools/*.h ";
	$aStyleFileTargets .= "Tools/*.inl ";
	system ("$astyleEXE $aStyleArgs --recursive $aStyleFileTargets");
}
else {
	system ("$astyleEXE $aStyleArgs < $runFile > $runFile.XXX");
	system ("mv $runFile.XXX $runFile");
}





