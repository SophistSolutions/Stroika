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

my @kConfigurations = (
					"Configuration=Debug-U-32,Platform=Win32",
					"Configuration=Debug-U-64,Platform=x64",
					"Configuration=Release-U-32,Platform=Win32",
					"Configuration=Release-U-64,Platform=x64",
					"Configuration=Release-Logging-U-32,Platform=Win32",
					"Configuration=Release-Logging-U-64,Platform=x64",
					"Configuration=Release-DbgMemLeaks-U-32,Platform=Win32"
					);


sub getCFGStr
{
	foreach (@kConfigurations) {
		my $curConfig	=	$_;
		if (index($curConfig, $ENV{'CONFIGURATION'}) != -1) {
			return $curConfig;
		}
	}
	die ("unrecognized config");
}

print (getCFGStr ($configuratioName));