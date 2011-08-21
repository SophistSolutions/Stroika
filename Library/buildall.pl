#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

system ("perl configure.pl --only-if-unconfigured");

if ("$^O" eq "linux") {
    system ("cd IntermediateFiles/Platform_Linux; make $BLD_TRG");
}
else {
    system ("cd Projects/VisualStudio.Net-2010; perl buildall.pl $BLD_TRG");
}
