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

# avoid quirks - ubuntu stores find in one place, and windows has own find, and cygin stores
# elsewhere... Sigh...
my $findEXE	=	"find";
if (-e "/bin/find") {
	$findEXE	=	"/bin/find";
}
if (-e "/usr/bin/find") {
	$findEXE	=	"/usr/bin/find";
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
	
	$aStyleArgs = "$aStyleArgs --formatted ";
	
	print ("Library...\n");
	system ("$findEXE Library -name \\*.cpp -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Library -name \\*.h -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Library -name \\*.inl -exec $astyleEXE $aStyleArgs {} \\;");
	print ("           ...done\n");

	print ("Samples...\n");
	system ("$findEXE Samples -name \\*.cpp -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Samples -name \\*.h -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Samples -name \\*.inl -exec $astyleEXE $aStyleArgs {} \\;");
	print ("           ...done\n");

	print ("Tests...\n");
	system ("$findEXE Tests -name \\*.cpp -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Tests -name \\*.h -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Tests -name \\*.inl -exec $astyleEXE $aStyleArgs {} \\;");
	print ("           ...done\n");

	print ("Tools...\n");
	system ("$findEXE Tools -name \\*.cpp -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Tools -name \\*.h -exec $astyleEXE $aStyleArgs {} \\;");
	system ("$findEXE Tools -name \\*.inl -exec $astyleEXE $aStyleArgs {} \\;");
	print ("           ...done\n");
}
else {
	system ("$astyleEXE $aStyleArgs < $runFile > $runFile.XXX");
	system ("mv $runFile.XXX $runFile");
}





