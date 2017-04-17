#!/usr/bin/perl
#use strict;
#use warnings;

BEGIN{ @INC = ( "./", @INC ); }


# Declare the subroutines
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}


1