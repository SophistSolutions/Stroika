#!/usr/bin/perl

if ("$^O" eq "linux") {
    system ("cd Projects/Linux/1; perl checkall.pl");
    system ("cd Projects/Linux/2; perl checkall.pl");
    system ("cd Projects/Linux/3; perl checkall.pl");
    system ("cd Projects/Linux/4; perl checkall.pl");
    system ("cd Projects/Linux/5; perl checkall.pl");
}
else {
	system ("cd Projects/VisualStudio.Net-2010/1; perl checkall.pl");
	system ("cd Projects/VisualStudio.Net-2010/2; perl checkall.pl");
	system ("cd Projects/VisualStudio.Net-2010/3; perl checkall.pl");
	system ("cd Projects/VisualStudio.Net-2010/4; perl checkall.pl");
	system ("cd Projects/VisualStudio.Net-2010/5; perl checkall.pl");
}
