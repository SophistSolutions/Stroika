#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

my $useMakeBuild = $BLD_TRG;
if (lc ($useMakeBuild) eq "build") {
        $useMakeBuild = "all";
}

system ("cd doxygen; perl buildall.pl $BLD_TRG");
system ("make --directory openssl --no-print-directory $useMakeBuild");
system ("make --directory Xerces --no-print-directory $useMakeBuild");
system ("make --directory zlib --no-print-directory $useMakeBuild");
