#!/usr/bin/perl 

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

require "$thisScriptDir/ConfigurationReader.pl";

$activeConfig = $ARGV[0];
my $useProjectDir= "$thisScriptDir/../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}

print STDERR "PRintVarFromCommonBuildVars.pl DEPRECATED - USE GetConfigurationParameter - or call GetAugmentedEnvironmentVariablesForConfiguration internally as configure does\n";

my $envVarName=$ARGV[1];
my %e = GetAugmentedEnvironmentVariablesForConfiguration ($activeConfig);
print "$e{$envVarName}"
