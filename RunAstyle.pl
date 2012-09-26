#!/usr/bin/perl

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


my $aStyleFileTargets = "";
$aStyleFileTargets .= "Library/*.cpp ";
$aStyleFileTargets .= "Library/*.h ";
$aStyleFileTargets .= "Library/*.inl ";

$aStyleFileTargets .= "Samples/*.cpp ";
$aStyleFileTargets .= "Samples/*.h ";
$aStyleFileTargets .= "Samples/*.inl ";

$aStyleFileTargets .= "Tests/*.cpp ";
$aStyleFileTargets .= "Tests/*.h ";
$aStyleFileTargets .= "Tests/*.inl ";

$aStyleFileTargets .= "Tools/*.cpp ";
$aStyleFileTargets .= "Tools/*.h ";
$aStyleFileTargets .= "Tools/*.inl ";


if ($runFile == "") {
	system ("astyle $aStyleArgs --recursive $aStyleFileTargets");
}
else {
	system ("astyle $aStyleArgs < $runFile > $runFile.XXX");
	system ("mv $runFile.XXX $runFile");
}





