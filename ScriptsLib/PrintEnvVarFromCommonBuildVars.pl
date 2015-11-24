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

#tmphack!!! 
$activeConfig = "DefaultConfiguration";
#$activeConfig = $ENV{'CONFIGURATION'};
#$activeConfig = $ARGV[0];
my $useProjectDir= "$thisScriptDir/../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
}


my $envVarName=$ARGV[1];
print "$ENV{$envVarName}"
