#!/usr/bin/perl

my $runFile;

my $aStyleArgs = "";


#$aStyleArgs .= "--style=stroustrup ";
#$aStyleArgs .= "--style=whitesmith ";
#$aStyleArgs .= "--style=kr ";
$aStyleArgs .= "--style=java ";

$aStyleArgs .= "--convert-tabs ";
$aStyleArgs .= "--indent-namespaces ";
$aStyleArgs .= "--indent-cases ";
$aStyleArgs .= "--indent-switches ";
$aStyleArgs .= "--indent-preprocessor ";
$aStyleArgs .= "--pad-oper ";
$aStyleArgs .= "--break-closing-brackets ";
$aStyleArgs .= "--keep-one-line-blocks ";
$aStyleArgs .= "--indent=spaces ";

$aStyleArgs .= "--align-pointer=type ";
$aStyleArgs .= "--align-reference=type ";
$aStyleArgs .= "--mode=c ";
$aStyleArgs .= "--suffix=none ";




foreach $i (@ARGV) {
	$runFile = $i;
}

if ($runFile == "") {
	system ("astyle $aStyleArgs --recursive Library/*.cpp Library/*.h Library/*.inl Tools/*.cpp Tools/*.h Tools/*.inl Tests/*.cpp Tests/*.h Tests/*.inl");
}
else {
	system ("astyle $aStyleArgs < $runFile > $runFile.XXX");
	system ("mv $runFile.XXX $runFile");
}





