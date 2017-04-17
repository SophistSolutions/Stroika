#!/usr/bin/perl 

use File::Temp qw(tempfile);

BEGIN{ @INC = ( "./", @INC ); }

sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}
my	$thisScriptDir	=	GetThisScriptDir ();

my $activeConfig = $ARGV[0];
my $cmd = $ARGV[1];

require "$thisScriptDir/ConfigurationReader.pl";

my $useProjectDir= "$thisScriptDir/../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}



sub RunAndPrint
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
	my $tmpFileName = "";
	$template = "runCmdInVCVarsContext_XXXXXX"; # trailing Xs are changed
	($fh, $tmpFileName) = tempfile( $template, SUFFIX => ".bat");
	##	open(my $fh, '>', $tmpFileName);
	#tmphack - need better way to select
	if (index($activeConfig, "32") != -1) {
		print $fh GetString2InsertIntoBatchFileToInit32BitCompiles();
	}
	elsif (index($activeConfig, "64") != -1) {
		print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
	}
	else {
		die ("hardwried/to fix logic about mapping config names to 32/64")
	}
	print $fh "$cmd2Run\r\n";
	close $fh;
	print ("      cmd /C $tmpFileName\r\n");
	my $result = system ("cmd /C $tmpFileName");
	if ($result != 0) {
		print "Run result = $result\r\n";
		exit ($result);
	}
}

#RunAndPrint (join(' ',@ARGV));
RunAndPrint ($cmd);
