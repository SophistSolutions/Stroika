#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $masterXMLConfigFile	=	"$intermediateFiles/Configuration.xml";



# Declare the subroutines
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}

sub	GetAllOfStringToEndTag($) {
	my $string = shift;
	my $eos = index($string, "<");
	if ($eos == -1) {
		$string;
	}
	else {
		return substr ($string, 0, $eos);
	}
}

my %configuration = ();

sub	ReadConfigFile_ {

	my $myDirName	=	GetThisScriptDir ();

	open (FILE, "$masterXMLConfigFile") or die("Unable to open $masterXMLConfigFile");
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my $startOff = index($line, "<ProjectPlatformSubdir>");
		if ($startOff != -1) {
			my $tmp = substr ($line, $startOff + length('<ProjectPlatformSubdir>'));
			$tmp = GetAllOfStringToEndTag ($tmp);
			$configuration {'ProjectPlatformSubdir'} = $tmp;
		}
	}
}




### PUBLIC APIS
sub	GetProjectPlatformSubdir {
	return $configuration{'ProjectPlatformSubdir'};
}








###PRIVATE
sub Prepare_ () {
	if ($#configuration < 0) {
		ReadConfigFile_ ();
	}
	#comment in to see values
	#print "CONFIGURE-ProjectPlatformSubdir: ", GetProjectPlatformSubdir(), "\n";
	return 1;
}



Prepare_ ();
