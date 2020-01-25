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

use File::Glob ':bsd_glob';
use File::Temp qw(tempfile);

### This file is minimally acceptable but mostly wrong.
### SEE http://bugzilla/show_bug.cgi?id=736
###

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



sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}


local $PROGRAMFILESDIR= $ENV{'PROGRAMFILES'};
local $PROGRAMFILESDIR86= $ENV{'ProgramFiles(x86)'};


sub toCygPath_
{
	my $arg = shift;
	{
		my $len = length ($arg);
		if ($len > 0 and substr ($arg, -1, 1) eq "\\") {
			$arg = substr ($arg, 0, $len-1);
		}
	}
	if ($arg eq "") {
		return "";
	}
	my $result = trim (`cygpath --unix \"$arg\"`);
	return $result;
}

sub fromCygPath_
{
	my $arg = shift;
	if ($arg eq "") {
		return "";
	}
	return trim (`cygpath --mixed \"$arg\"`);
}

sub toExternallyUsedPath_
{
	my $arg = shift;
	if ($arg eq "") {
		return "";
	}
	return trim (`cygpath --mixed \"$arg\"`);
}



my $VSDIR = "$PROGRAMFILESDIR86\\Microsoft Visual Studio\\2017";
if (! (-e toCygPath_ ($VSDIR))) {
	$VSDIR = "$PROGRAMFILESDIR\\Microsoft Visual Studio\\2017";
}
@VSDIRs = bsd_glob (toCygPath_ ("$VSDIR\\*"));
$VSDIR = fromCygPath_ (@VSDIRs[0]);
if (! (-e toCygPath_ ($VSDIR))) {
	die ("directory '$VSDIR' doesn't exist");
} 


my $VSDIR_VC = "$VSDIR\\VC";

### SEE https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2017 for docs on calling vcvarsall.bat

sub GetString2InsertIntoBatchFileToInit32BitCompiles_
{
	my $result = "";
	##pushd/popd needed cuz vcvars now changes directories (no idea why)
	$result 	.=	"pushd %TEMP%\r\n";
	$result 	.=	"call \"";
	$result 	.=	"$VSDIR_VC\\Auxiliary\\Build\\vcvarsall.bat";
	$result 	.=	"\" x64_x86 > nul;\r\n";
	$result 	.=	"popd\r\n";
	return $result;
}

sub GetString2InsertIntoBatchFileToInit64BitCompiles_
{
	my $result = "";
	##pushd/popd needed cuz vcvars now changes directories (no idea why)
	$result 	.=	"pushd %TEMP%\r\n";
	$result 	.=	"call \"";
	$result 	.=	"$VSDIR_VC\\Auxiliary\\Build\\vcvarsall.bat";
	$result 	.=	"\" x64 > nul;\r\n";
	$result 	.=	"popd\r\n";
	return $result;
}



sub RunBackTickWithVCVarsSetInEnvironment_
{
	my $activeConfigBits = $_[0];
	my $cmd2Run = $_[1];
	my $tmpFileName = "";
	$template = "runCmdInVCVarsContext_XXXXXX"; # trailing Xs are changed
	($fh, $tmpFileName) = tempfile( $template, SUFFIX => ".bat");
	print $fh '@echo off' . "\r\n";
	if (index($activeConfigBits, "32") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit32BitCompiles_();
		print $fh $result;
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit64BitCompiles_();
		print $fh $result;
	}
	else {
		die ("hardwired/to fix logic about mapping config names to 32/64")
	}
	print $fh $cmd2Run . "\r\n";
	close $fh;
	my $result = `cmd /C $tmpFileName`;
	unlink ($tmpFileName);
	return $result;
}


sub GetEnvironmentVariablesForConfiguration_
{
	my $activeConfigBits = $_[0];
	my $resultStr = RunBackTickWithVCVarsSetInEnvironment_($activeConfigBits, "set");
	my %result =     ();
	foreach $line (split /[\r\n]/, $resultStr) {
		my @splitLine = split (/=/, $line);
		my $envVar = @splitLine[0];
		my $envVarValue = @splitLine[1];
		$result{$envVar} .= $envVarValue;
	}
	#print "GOT (ACTIVECONFIG=$activeConfig) PATH=" . %result{"PATH"} . "\n";
	return %result;
}


sub GetConfig32Or64_
{
	my $activeConfig = $_[0];
	if ($activeConfig eq "x86") {
		return "32";
	}
	if ($activeConfig eq "x86_64") {
		return "64";
	}
	print STDERR "WARNING BAD ARCH passed to SetupBuildCommonVars module: ARCH= $activeConfig\n";
	if ($activeConfig =~ m/32/) {
	#print ("GetConfig32Or64_ with config=$activeConfig   RETURN 32\r\n");
		return "32";
	}
	if ($activeConfig =~ m/64/) {
	#print ("GetConfig32Or64_ with config=$activeConfig   RETURN 64\r\n");
		return "64";
	}
	die ("failed to map config $activeConfig to 32/64")
}


### USED TO RUN ScriptsLib\RunArgumentsWithCommonBuildVars
sub RunSystemWithVCVarsSetInEnvironment
{
	my $activeConfigBits = GetConfig32Or64_ ($_[0]);
	my $cmd2Run = $_[1];
	my $tmpFileName = "";
	$template = "runCmdInVCVarsContext_XXXXXX"; # trailing Xs are changed
	($fh, $tmpFileName) = tempfile( $template, SUFFIX => ".bat");
	print $fh '@echo off' . "\r\n";
	if (index($activeConfigBits, "32") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit32BitCompiles_();
		print $fh $result;
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit64BitCompiles_();
		#print "64 case and result=$result\r\n";
		print $fh $result;
	}
	else {
		die ("hardwired/to fix logic about mapping config names to 32/64")
	}
	print $fh $cmd2Run . "\r\n";
	close $fh;

	#print "TMPFILENAME=$tmpFileName; cfg=$_[0], activeconfigbits=$activeConfigBits, and cmd2run=$cmd2Run\r\n";
	my $result = system ("cmd /C $tmpFileName");
	unlink ($tmpFileName);
	return $result;
}


# IF WE STILL NEEDEED THIS - cygpath --path does it faster/simpler
#sub convertWinPathVar2CygwinPathVar_
#{
#	my $winPath = $_[0];
#	#print "doing convertWinPathVar2CygwinPathVar_ (" . $winPath . ")\n";
#	my $newCygPath = "";
#	foreach $pathElt (split (";",$winPath)) {
#		$newCygPath .= toCygPath_ ($pathElt) . ":";
#	}
#	#print "returning convertWinPathVar2CygwinPathVar_ (" . $newCygPath . ")\n";
#	return $newCygPath;
#}


sub GetAugmentedEnvironmentVariablesForConfiguration
{
	my $activeConfigBits = GetConfig32Or64_ ($_[0]);
	my %resEnv = GetEnvironmentVariablesForConfiguration_ ($activeConfigBits);

	my $cwVSDIR = toCygPath_ ($VSDIR);

	#my $HOSTSTR="HostX86";	-- I doubt anyone develops on 32bit anymore, but if they do we can easily detect and fix this
	my $HOSTSTR="HostX64";
	if (index($activeConfigBits, "32") != -1) {
		my @exe32Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/$HOSTSTR/x86");
		my $exe32Dir = fromCygPath_ (@exe32Dirs[0]);
		$resEnv{"AS"} = toExternallyUsedPath_ ($exe32Dir . "\\ml");
		$resEnv{"CC"} = toExternallyUsedPath_ ($exe32Dir . "\\cl");
		$resEnv{"LD"} = toExternallyUsedPath_ ($exe32Dir . "\\link");
		$resEnv{"AR"} = toExternallyUsedPath_ ($exe32Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my @exe64Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/$HOSTSTR/x64");
		my $exe64Dir = fromCygPath_ (@exe64Dirs[0]);
		$resEnv{"AS"} = toExternallyUsedPath_ ($exe64Dir . "\\ml64");
		$resEnv{"CC"} = toExternallyUsedPath_ ($exe64Dir . "\\cl");
		$resEnv{"LD"} = toExternallyUsedPath_ ($exe64Dir . "\\link");
		$resEnv{"AR"} = toExternallyUsedPath_ ($exe64Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}

	{
		my $sdkPath = %resEnv{'WindowsSdkVerBinPath'};
		$sdkPath = toCygPath_($sdkPath);
		my $exeDir = "$sdkPath/x64/";
		$resEnv{"MIDL"} = toExternallyUsedPath_ ($exeDir . "midl");
		$resEnv{"RC"} = toExternallyUsedPath_ ($exeDir . "rc");
	}

	my $myOrigFullPath = $ENV{'PATH'};
	#print "myOrigFullPath=$myOrigFullPath\n";
	my $newFullPath = $resEnv{"PATH"};
	$newFullPath=`cygpath --unix --path \"$newFullPath\"`;

	my %myOrigFullPathHash;
	foreach my $i (split /:/, $myOrigFullPath) {
	  $myOrigFullPathHash{$i} = "";
	}

	my %newFullPathHash;
	foreach my $i (split /:/, $newFullPath) {
	  $newFullPathHash{$i} = "";
	}

	#print "ORIG:\n";
	#foreach my $ip (keys %myOrigFullPathHash) {
	# print "  ip=$ip\n";
	#}

	#print "NEW:\n";
	foreach my $ip (keys %newFullPathHash) {
	 #print "  new-ip=$ip\n";
	  if (exists($myOrigFullPathHash{$ip})) {
	    #print "deleting $ip\n";
		delete $newFullPathHash{$ip};
	  }
	}

	#print "NEW-SUBSET-ADDED:\n";
	#foreach my $ip (keys %newFullPathHash) {
	# print "  new-ip=$ip\n";
	#}

	## Sorting is not desirable, but has doesn't preserve order anyhow, so best the output is regular (the same across runs)
	my $param_string = "";
	foreach my $ip (sort (keys %newFullPathHash)) {
		#print "  new-ip=$ip\n";
		$ip = trim ($ip);
		if (! ($param_string eq "")) {
			$param_string.= ":";
		}
		$param_string.= "$ip";
	}
	$resEnv{"TOOLS_PATH_ADDITIONS"} = $param_string;
	#todo must so similar but be careful about how to treat colon in paths.
	#$resEnv{"TOOLS_PATH_ADDITIONS"} =`cygpath --unix --path \"$param_string\"`;

	return %resEnv;
}

1
