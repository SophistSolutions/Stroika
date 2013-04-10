#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}
my	$thisScriptDir	=	GetThisScriptDir ();


### This file is minimally acceptable but mostly wrong.
### SEE http://bugzilla/show_bug.cgi?id=736
###

require ("$thisScriptDir/../../../ScriptsLib/StringUtils.pl");


sub PRINT_ENV_
{
 	my $msg = shift;
	print ($msg);
	foreach $key (sort(keys %ENV)) {
	   print "$key = $ENV{$key}<br>\n";
	}
}

sub PRINT_PATH_
{
 	my $msg = shift;
	print ($msg);
	##### WRONG - must parse envpath as : sep list
	foreach $key (sort(keys %ENV)) {
	   print "$key = $ENV{$key}<br>\n";
	}
}

if (0) {
	PRINT_PATH_ ("BEFORE SETTING PATH ENV=$ENV{'PATH'}\n");
}


# WE SHOULD call
# 		call vcvarsall.bat x86
# and capture its results, but I'm not quite sore how yet....
### SEE http://bugzilla/show_bug.cgi?id=736




my $VisualStudioVersion = "11.0";
$ENV{'VisualStudioVersion'}=	$VisualStudioVersion;



local $PROGRAMFILESDIR= $ENV{'PROGRAMFILES'};
local $PROGRAMFILESDIR86= $ENV{'ProgramFiles(x86)'};


sub toCygPath_
{
	my $arg = shift;
	return trim (`cygpath \"$arg\"`);
}


#print ("progrifles = " . toCygPath_($PROGRAMFILESDIR86));

my $VSDIR = "$PROGRAMFILESDIR86\\Microsoft Visual Studio $VisualStudioVersion";
if (! (-e toCygPath_ ($VSDIR))) {
	$VSDIR = "$PROGRAMFILESDIR\\Microsoft Visual Studio $VisualStudioVersion";
} 
if (! (-e toCygPath_ ($VSDIR))) {
	die ("directory $VSDIR doesn't exist");
} 

 

my $VSDIR_VC = "$VSDIR\\VC";
$ENV{'VCINSTALLDIR'} = 	$VSDIR_VC;


# Lots of environment variable patches/sets because I cannot find a way to run vcvarsall.bat
# from the perl script (and capture its environment vars)
$ENV{'PATH'} 	= 	"/cygdrive/c/Windows/Microsoft.NET/Framework/v4.0.30319" .":" . $ENV{'PATH'};

$ENV{'PATH'}	= 	"/cygdrive/c/WINDOWS/system32". ":" . $ENV{'PATH'};

$ENV{'PATH'}	= 	toCygPath_ ($VSDIR) . "/VC/bin". ":" . $ENV{'PATH'};
$ENV{'PATH'} 	= 	toCygPath_ ($VSDIR) . "/VC/vcpackages" . ":" . $ENV{'PATH'};
$ENV{'PATH'} 	= 	toCygPath_ ($VSDIR) . "/Common7/IDE" . ":" . $ENV{'PATH'};


### GUESS IF NEEDED (AND IF RIGHT PATH)
my $WindowsSdkDir = "C\:\\Program Files (x86)\\Windows Kits\\8.0";
if (! (-e toCygPath_ ($WindowsSdkDir))) {
	$WindowsSdkDir = "C\:\\Program Files\\Windows Kits\\8.0";
} 
$ENV{'WindowsSdkDir'}="$WindowsSdkDir\\";

#my $ExtensionSdkDir = "C\:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v8.0\\ExtensionSDKs";
#$ENV{'ExtensionSdkDir'}=$ExtensionSdkDir;

$ENV{'INCLUDE'}	=	"";
$ENV{'INCLUDE'} 	.=	"$VSDIR_VC\\INCLUDE;";
$ENV{'INCLUDE'} 	.=	"$VSDIR_VC\\ATLMFC\\INCLUDE;";
$ENV{'INCLUDE'} 	.=	"$WindowsSdkDir\\include\\shared;";
$ENV{'INCLUDE'} 	.=	"$WindowsSdkDir\\include\\um;";
$ENV{'INCLUDE'} 	.=	"$WindowsSdkDir\\include\\winrt;";
#print "INCLUDES=", $ENV{'INCLUDE'}, "\n";


$ENV{'LIB'} 	=	"";
$ENV{'LIB'} 	.=	"$VSDIR_VC\\LIB;";
$ENV{'LIB'} 	.=	"$VSDIR_VC\\ATLMFC\\LIB;";
$ENV{'LIB'} 	.=	"$WindowsSdkDir\\lib\\win8\\um\\x86;";


#Mostly for debugging - make sure paths setup properly
my $x = trim (`cmd /c 'which cl'`);
if ($x ne "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 11.0/VC/bin/cl") {
	print "[WARNING] - Differnt CL: '$x'\n";
	PRINT_PATH_ ("PATH ENV=$ENV{'PATH'}\n");
}
my $x = trim (`cmd /c 'which link'`);
if ($x ne "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 11.0/VC/bin/link") {
	print "[WARNING] - Differnt link: '$x'\n";
	PRINT_PATH_ ("PATH ENV=$ENV{'PATH'}\n");
}

if (0) {
	PRINT_PATH_ ("AFTER SETTING PATH ENV=$ENV{'PATH'}\n");
}

1
