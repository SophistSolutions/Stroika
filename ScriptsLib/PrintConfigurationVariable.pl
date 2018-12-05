#!/usr/bin/perl 

#####DEPRECATED#### AS OF STROIKA v2.1d10 - use GetConfigurationParameter

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

my $configuratioName=$ARGV[0];
my $envVarName=$ARGV[1];
print GetConfigurationParameter($configuratioName, $envVarName);
print STDERR "PrintConfigurationVariable.pl DEPRECATED - USE GetConfigurationParameter\n";
