#!/usr/bin/perl
#use strict;
#use warnings;

####Stroika 2.1d14

BEGIN{ @INC = ( "./", @INC ); }


# Declare the subroutines
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

print STDERR "StringUtils.pl DEPRECATED Stroika 2.1d14\n";

1