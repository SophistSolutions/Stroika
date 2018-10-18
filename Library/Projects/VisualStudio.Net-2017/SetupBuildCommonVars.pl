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
	my $result = trim (`cygpath --unix \"$arg\"`);
	return $result;
}

sub fromCygPath_
{
	my $arg = shift;
	return trim (`cygpath --windows \"$arg\"`);
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


sub GetString2InsertIntoBatchFileToInit32BitCompiles
{
	my $result = "";
	##pushd/popd needed cuz vcvars now changes directories (no idea why)
	$result 	.=	"pushd %TEMP%\r\n";
	$result 	.=	"call \"";
	$result 	.=	"$VSDIR_VC\\Auxiliary\\Build\\vcvarsall.bat";
	$result 	.=	"\" x86 > nul;\r\n";
	$result 	.=	"popd\r\n";
	return $result;
}

sub GetString2InsertIntoBatchFileToInit64BitCompiles
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
		my $result = GetString2InsertIntoBatchFileToInit32BitCompiles();
		print $fh $result;
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit64BitCompiles();
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


sub GetEnvironmentVariablesForConfiguration
{
	my $activeConfigBits = GetConfig32Or64_ ($_[0]);
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

sub RunSystemWithVCVarsSetInEnvironment
{
	my $activeConfigBits = GetConfig32Or64_ ($_[0]);
	my $cmd2Run = $_[1];
	my $tmpFileName = "";
	$template = "runCmdInVCVarsContext_XXXXXX"; # trailing Xs are changed
	($fh, $tmpFileName) = tempfile( $template, SUFFIX => ".bat");
	print $fh '@echo off' . "\r\n";
	if (index($activeConfigBits, "32") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit32BitCompiles();
		print $fh $result;
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my $result = GetString2InsertIntoBatchFileToInit64BitCompiles();
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


sub convertWinPathVar2CygwinPathVar_
{
	my $winPath = $_[0];
	#print "doing convertWinPathVar2CygwinPathVar_ (" . $winPath . ")\n";
	my $newCygPath = "";
	foreach $pathElt (split (";",$winPath)) {
		$newCygPath .= toCygPath_ ($pathElt) . ":";
	}
	#print "returning convertWinPathVar2CygwinPathVar_ (" . $newCygPath . ")\n";
	return $newCygPath;
}


sub GetAugmentedEnvironmentVariablesForConfiguration
{
	my $activeConfigBits = GetConfig32Or64_ ($_[0]);
	my %resEnv = GetEnvironmentVariablesForConfiguration ($activeConfigBits);

	my $cwVSDIR = toCygPath_ ($VSDIR);

	if (index($activeConfigBits, "32") != -1) {
		my @exe32Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/HostX86/x86");
		my $exe32Dir = fromCygPath_ (@exe32Dirs[0]);
		$resEnv{"AS"} = toCygPath_ ($exe32Dir . "\\ml");
		$resEnv{"CC"} = toCygPath_ ($exe32Dir . "\\cl");
		$resEnv{"LD"} = toCygPath_ ($exe32Dir . "\\link");
		$resEnv{"AR"} = toCygPath_ ($exe32Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}
	elsif (index($activeConfigBits, "64") != -1) {
		my @exe64Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/HostX86/x64");
		my $exe64Dir = fromCygPath_ (@exe64Dirs[0]);
		$resEnv{"AS"} = toCygPath_ ($exe64Dir . "\\ml64");
		$resEnv{"CC"} = toCygPath_ ($exe64Dir . "\\cl");
		$resEnv{"LD"} = toCygPath_ ($exe64Dir . "\\link");
		$resEnv{"AR"} = toCygPath_ ($exe64Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}

	my $winPATH = %resEnv{"PATH"};
	#print "GOT env32 PATH=" . $winPATH . "\n";
	$resEnv{"PATH"} = convertWinPathVar2CygwinPathVar_($winPATH);

	return %resEnv;
}

1
