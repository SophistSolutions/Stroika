#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

### THis file is minimally acceptable but mostly wrong.
### SEE http://bugzilla/show_bug.cgi?id=736
###


#print ("BEFORE SETTING PATH ENV=$ENV{'PATH'}\n");

#Only needed because I cannot find a way to run vcvarsall.bat from the perl script (and capture its environment vars)
#$ENV{'PATH'} = "/cygdrive/c/Windows/Microsoft.NET/Framework/v4.0.30319:" . $ENV{'PATH'};
$ENV{'PATH'} = "c\:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319:" . $ENV{'PATH'};


#foreach $key (sort(keys %ENV)) {
#    print "$key = $ENV{$key}<br>\n";
#}

my $VisualStudioVersion = "11.0";
$ENV{'VisualStudioVersion'}=	$VisualStudioVersion;



local $PROGRAMFILESDIR= $ENV{'PROGRAMFILES'};
local $PROGRAMFILESDIR= `cygpath \"$PROGRAMFILESDIR\"`;
chomp($PROGRAMFILESDIR);

my $VCINSTALLDIR = "C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\";
$ENV{'VCINSTALLDIR'} = 	$VCINSTALLDIR;


###TMPAHCK
$ENV{'PATH'}.="C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\Common7\\IDE\\CommonExtensions\\Microsoft\\TestWindow;C\:\\Program Files (x86)\\Microsoft SDKs\\F#\\3.0\\Framework\\v4.0\\;C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VSTSDB\\Deploy;C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\Common7\\IDE\\;$VCINSTALLDIR\BIN;C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\Common7\\Tools;C\:\\WINDOWS\\Microsoft.NET\\Framework\\v4.0.30319;C\:\\WINDOWS\\Microsoft.NET\\Framework\\v3.5;$VCINSTALLDIR\\VCPackages;C\:\\Program Files (x86)\\HTML Help Workshop;C\:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\Team Tools\\Performance Tools;C\:\\Program Files (x86)\\Windows Kits\\8.0\\bin\\x86;C\:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v8.0A\\bin\\NETFX 4.0 Tools;C\:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Bin\\;;C\:\\WINDOWS\\system32;c\:\\cygwin\\bin;C\:\\Program Files\\Microsoft Visual Studio 11.0\\VC\\;$VCINSTALLDIR;C\:\\Program Files\\7-Zip;c\:\\Program Files\\7-zip";



$ENV{'PATH'}	= 	"$PROGRAMFILESDIR/Microsoft Visual Studio $VisualStudioVersion/VC/bin:" . $ENV{'PATH'};
#$ENV{'PATH'}	= 	"/cygdrive/c/Program Files (x86)/Microsoft Visual Studio $VisualStudioVersion/VC/bin:" . $ENV{'PATH'};
$ENV{'PATH'} 	= 	"$PROGRAMFILESDIR/Microsoft Visual Studio $VisualStudioVersion/VC/:" . $ENV{'PATH'};
#$ENV{'PATH'} 	= 	"/cygdrive/c/Program Files (x86)/Microsoft Visual Studio $VisualStudioVersion/VC/:" . $ENV{'PATH'};
$ENV{'PATH'} 	= 	"$PROGRAMFILESDIR/Microsoft Visual Studio $VisualStudioVersion/VC/vcpackages/:" . $ENV{'PATH'};
#$ENV{'PATH'} 	=	"/cygdrive/c/Program Files (x86)/Microsoft Visual Studio $VisualStudioVersion/VC/vcpackages/:" . $ENV{'PATH'};
$ENV{'PATH'} 	= 	"$PROGRAMFILESDIR/Microsoft Visual Studio $VisualStudioVersion/Common7/IDE:" . $ENV{'PATH'};
#$ENV{'PATH'} 	= 	"/cygdrive/c/Program Files (x86)/Microsoft Visual Studio $VisualStudioVersion/Common7/IDE:" . $ENV{'PATH'};


### GUESS IF NEEDED (AND IF RIGHT PATH)
my $WindowsSdkDir = "C\:\\Program Files (x86)\\Windows Kits\\8.0\\";
$ENV{'WindowsSdkDir'}=$WindowsSdkDir;

my $ExtensionSdkDir = "C\:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v8.0\\ExtensionSDKs";
$ENV{'ExtensionSdkDir'}=$ExtensionSdkDir;

my $VSDIR = "C:\\Program Files (x86)\\Microsoft Visual Studio $VisualStudioVersion";
my $VSDIR_VC = "$VSDIR\\VC";

$ENV{'INCLUDE'} =	"";
$ENV{'INCLUDE'} .=	"$VSDIR_VC\\INCLUDE;";
$ENV{'INCLUDE'} .=	"$VSDIR_VC\\ATLMFC\\INCLUDE;";
$ENV{'INCLUDE'} .=	"$WindowsSdkDir\\include\\shared;";
$ENV{'INCLUDE'} .=	"$WindowsSdkDir\\include\\um;";
$ENV{'INCLUDE'} .=	"$WindowsSdkDir\\include\\winrt;";



$ENV{'LIB'} =	"$VCINSTALLDIR\\LIB;$VCINSTALLDIR\\ATLMFC\\LIB;C\:\\Program Files (x86)\\Windows Kits\\8.0\\lib\\win8\\um\\x86";




# WE SHOULD call
# 		call vcvarsall.bat x86
# and capture its results, but I'm not quite sore how yet....
### SEE http://bugzilla/show_bug.cgi?id=736


#print ("AFTER SETTING PATH ENV=$ENV{'PATH'}\n");

1
