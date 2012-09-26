#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

if ("$^O" eq "cygwin") {
	require "../../Library/Projects/VisualStudio.Net-2010/SetupBuildCommonVars.pl";
}
else {
	#require "../../Library/Projects/VisualStudio.Net-2010/SetupBuildCommonVars.pl";
}

my $EXTRA_MSBUILD_ARGS = "/nologo /v:quiet /clp:Summary";
sub	GetMSBuildArgs {
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
	print ("$cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		print "Run result = $result\r\n";
	}
}

1
