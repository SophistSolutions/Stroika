#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

use File::Glob ':bsd_glob';
use File::Temp qw(tempfile);


sub trim($)
{
	local $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}


sub PRINT_ENV_
{
 	local $msg = shift;
	print ($msg);
	foreach my $key (sort(keys %ENV)) {
	   print "$key = $ENV{$key}<br>\n";
	}
}



my $PROGRAMFILESDIR= $ENV{'PROGRAMFILES'};
my $PROGRAMFILESDIR86= $ENV{'ProgramFiles(x86)'};


sub toCygPath_
{
	local $arg = shift;
	{
		local $len = length ($arg);
		if ($len > 0 and substr ($arg, -1, 1) eq "\\") {
			$arg = substr ($arg, 0, $len-1);
		}
	}
	if ($arg eq "") {
		return "";
	}
	local $result = trim (`cygpath --unix \"$arg\"`);
	return $result;
}

sub fromCygPath_
{
	local $arg = shift;
	if ($arg eq "") {
		return "";
	}
	return trim (`cygpath --mixed \"$arg\"`);
}

sub toExternallyUsedPath_
{
	local $arg = shift;
	if ($arg eq "") {
		return "";
	}
	return trim (`cygpath --mixed \"$arg\"`);
}

###
### Return the default (guessed) Visual Studio instllation directory
### e.g. $PROGRAMFILESDIR\\Microsoft Visual Studio\\2019
###
sub GetDefaultToolsBuildDir ()
{
### @todo consider somehow redoing this logic with use of vswhere, but not 100% sure how to fetch efficiently to use
	local $VSDIR = "$PROGRAMFILESDIR86\\Microsoft Visual Studio\\2019";
	if (! (-e toCygPath_ ($VSDIR))) {
		$VSDIR = "$PROGRAMFILESDIR\\Microsoft Visual Studio\\2019";
	}
	@VSDIRs = bsd_glob (toCygPath_ ("$VSDIR\\*"));
	$VSDIR = fromCygPath_ (@VSDIRs[0]);
	if (! (-e toCygPath_ ($VSDIR))) {
		die ("directory '$VSDIR' doesn't exist");
	}
	return $VSDIR;
}



### SEE https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019 for docs on calling vcvarsall.bat

sub GetString2InsertIntoBatchFileToInitCompiles_
{
	my $ARCH = shift;
	my $VSDIR = shift;
	my $arg = undef;
	if ($ARCH eq "x86") {
		$arg ="x64_x86";
	}
	elsif ($ARCH eq "x86_64") {
		$arg ="x64";
	}
	else {
		die ("hardwired/to fix logic about mapping config names to 32/64")
	}
	my $VSDIR_VC = "$VSDIR\\VC";
	my $result = "";
	##pushd/popd needed cuz vcvars now changes directories (no idea why)
	$result 	.=	"pushd %TEMP%\r\n";
	$result 	.=	"call \"";
	$result 	.=	"$VSDIR_VC\\Auxiliary\\Build\\vcvarsall.bat";
	$result 	.=	"\" $arg > nul;\r\n";
	$result 	.=	"popd\r\n";
	return $result;
}



sub RunBackTickWithVCVarsSetInEnvironment_
{
	local $ARCH = shift;
	local $cmd2Run = shift;
	local $tmpFileName = "";
	$template = "runCmdInVCVarsContext_XXXXXX"; # trailing Xs are changed
	($fh, $tmpFileName) = tempfile( $template, SUFFIX => ".bat");
	print $fh '@echo off' . "\r\n";
	local $result = GetString2InsertIntoBatchFileToInitCompiles_($ARCH, $VSDIR);
	print $fh $result;
	print $fh $cmd2Run . "\r\n";
	close $fh;
	local $result = `cmd /C $tmpFileName`;
	unlink ($tmpFileName);
	return $result;
}


sub GetEnvironmentVariablesForConfiguration_
{
	local $ARCH = shift;
	my $VSDIR = shift;
	my $resultStr = RunBackTickWithVCVarsSetInEnvironment_($ARCH, "set");
	my %result =     ();
	foreach $line (split /[\r\n]/, $resultStr) {
		my @splitLine = split (/=/, $line);
		my $envVar = @splitLine[0];
		my $envVarValue = @splitLine[1];
		$result{$envVar} .= $envVarValue;
	}
	#print "GOT (ACTIVECONFIG=$activeConfig) PATH=" . %result{"WindowsSdkVerBinPath"} . "\n";
	#print "GOT (ACTIVECONFIG=$activeConfig) PATH=" . %result{"PATH"} . "\n";
	return %result;
}


#
# Return a dictionary with all the configuration for the given argument ARCH string
#
sub GetAugmentedEnvironmentVariablesForConfiguration
{
	local $ARCH = shift;
	local $VSDIR = shift;
	my %resEnv = GetEnvironmentVariablesForConfiguration_ ($ARCH, $VSDIR);

	my $cwVSDIR = toCygPath_ ($VSDIR);

	#my $HOSTSTR="HostX86";	-- I doubt anyone develops on 32bit anymore, but if they do we can easily detect and fix this
	my $HOSTSTR="HostX64";
	if ($ARCH eq "x86") {
		my @exe32Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/$HOSTSTR/x86");
		my $exe32Dir = fromCygPath_ (@exe32Dirs[0]);
		$resEnv{"AS"} = toExternallyUsedPath_ ($exe32Dir . "\\ml");
		$resEnv{"CC"} = toExternallyUsedPath_ ($exe32Dir . "\\cl");
		$resEnv{"LD"} = toExternallyUsedPath_ ($exe32Dir . "\\link");
		$resEnv{"AR"} = toExternallyUsedPath_ ($exe32Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}
	elsif ($ARCH eq "x86_64") {
		my @exe64Dirs = bsd_glob ("$cwVSDIR/VC/Tools/MSVC/*/bin/$HOSTSTR/x64");
		my $exe64Dir = fromCygPath_ (@exe64Dirs[0]);
		$resEnv{"AS"} = toExternallyUsedPath_ ($exe64Dir . "\\ml64");
		$resEnv{"CC"} = toExternallyUsedPath_ ($exe64Dir . "\\cl");
		$resEnv{"LD"} = toExternallyUsedPath_ ($exe64Dir . "\\link");
		$resEnv{"AR"} = toExternallyUsedPath_ ($exe64Dir . "\\lib");		# 'AR' is what unix uses to create libraries
	}
	else {
		die ("unrecongized ARCH")
	}

	{
		my $sdkPath = %resEnv{'WindowsSdkVerBinPath'};
		$sdkPath = toCygPath_($sdkPath);
		$resEnv{"WindowsSdkVerBinPath"} = toExternallyUsedPath_ ($WindowsSdkVerBinPath);
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
	#todo must so similar but be careful about how to treat colon in paths.
	$resEnv{"TOOLS_PATH_ADDITIONS"} = $param_string;
	#$resEnv{"TOOLS_PATH_ADDITIONS"} =`cygpath --unix --path \"$param_string\"`;

	return %resEnv;
}

1