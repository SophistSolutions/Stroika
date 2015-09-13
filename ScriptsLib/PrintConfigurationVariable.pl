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

my $configuratioName=$ARGV[0];
my $envVarName=$ARGV[1];
if ($envVarName eq "DefaultConfiguration") {
	print "DefaultConfiguration";
}
else {
	print GetConfigurationParameter($envVarName);
}
