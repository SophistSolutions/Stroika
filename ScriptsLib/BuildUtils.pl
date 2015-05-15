#!/usr/bin/perl 

sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}
my	$thisScriptDir	=	GetThisScriptDir ();

require "$thisScriptDir/ConfigurationReader.pl";


my $useProjectDir= "$thisScriptDir/../Library/Projects/" . GetProjectPlatformSubdirIfAny ();
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}




sub	RunAndStopOnFailure
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		die ("Run result = $result\r\n");
	}
}



sub	GetMSBuildArgs {
	my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";
	return $EXTRA_MSBUILD_ARGS;
}



sub	NormalizeBuildArg {
	my $useBld =	$_[0];
	if ($useBld eq '') {
		$useBld = 'Build';
	}
	if (lc ($useBld) eq "clobber") {
		$useBld = "Clean";
	}
	return $useBld;
}


sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}

1
