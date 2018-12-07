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

my $testNumber = $ARGV[0];

require "$thisScriptDir/TestsList.pl";

my $testName = GetTestName ($testNumber);

print "$testName"
