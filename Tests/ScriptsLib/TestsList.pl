#!/usr/bin/env perl
#use strict;
#use warnings;

###
### NOT TO BE CALLED DIRECTLY - USED INTERNALLY IN PRINTTESTLIST and PRINTTESTNAME
###

BEGIN{ @INC = ( "./", @INC ); }

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


my @listAllTests = ();
my %mapTests2PrettyNames = ();

sub	ReadListOfTests_ {

	my $myDirName	=	GetThisScriptDir ();

	open (FILE, "$myDirName/../Tests-Description.txt") or die("Unable to open Tests-Description.txt");
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my @lineSplitByTab = split (/\t/, trim ($line));
		my $tstName = @lineSplitByTab[0];
		$tstName =~ s/\[//;
		$tstName =~ s/\]//;
		my $tstPrettyName = @lineSplitByTab[1];
		push (@listAllTests, $tstName);
		$mapTests2PrettyNames {$tstName} = $tstPrettyName;
	}
}
sub	GetTestName {
	my $tst = $_[0];
	return $mapTests2PrettyNames{$tst};
}

sub GetAllTests {
	if ($#listAllTests < 0) {
		ReadListOfTests_ ();
	}
	return @listAllTests;
}

sub Prepare_ () {
	if ($#listAllTests < 0) {
		ReadListOfTests_ ();
	}
	return 1;
}


sub RunSimpleTest {
	foreach $tst (GetAllTests ()) {
		print ("\tTstName=$tst, pretty: $mapTests2PrettyNames{$tst}\n");
	}
}


#RunSimpleTest();
Prepare_ ();
