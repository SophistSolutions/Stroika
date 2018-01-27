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

my $result = RunSystemWithVCVarsSetInEnvironment ($activeConfig, $cmd);
# exit int ($result) doesn't work - for reasons I dont udnerstnad, but since abanodoming perl, no point in debugging - just workaround for now --LGP 2017-10-29
if ($result == 0) {
	exit 0;
}
else {
	exit 1;
}
