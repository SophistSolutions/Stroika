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

### NEARLY DEPRECATED but not quite - still used to run MSBuild scripts - LGP 2018-12-12

require "$thisScriptDir/ConfigurationReader.pl";

my $useProjectDir= "$thisScriptDir/../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}

#my $result = RunSystemWithVCVarsSetInEnvironment ($activeConfig, $cmd);
my $result = RunSystemWithVCVarsSetInEnvironment (GetConfigurationParameter($activeConfig, "ARCH"), $cmd);
# exit int ($result) doesn't work - for reasons I don't understand, but since abanodoming perl, no point in debugging - just workaround for now --LGP 2017-10-29
if ($result == 0) {
	exit 0;
}
else {
	exit 1;
}
