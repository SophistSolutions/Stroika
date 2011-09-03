#!/usr/bin/perl 

if ("$^O" eq "linux") {
	my $useBld = lc ($BLD_TRG);
	if ($useBld eq "build") {
		$useBld = "all";
	}
	system ("cd Projects/Linux/1; perl Run.pl");
	system ("cd Projects/Linux/2; perl Run.pl");
	system ("cd Projects/Linux/3; perl Run.pl");
	system ("cd Projects/Linux/4; perl Run.pl");
	system ("cd Projects/Linux/5; perl Run.pl");
}
else {
	system ("cd Projects/VisualStudio.Net-2010/1; perl Run.pl");
	system ("cd Projects/VisualStudio.Net-2010/2; perl Run.pl");
	system ("cd Projects/VisualStudio.Net-2010/3; perl Run.pl");
	system ("cd Projects/VisualStudio.Net-2010/4; perl Run.pl");
	system ("cd Projects/VisualStudio.Net-2010/5; perl Run.pl");
}

