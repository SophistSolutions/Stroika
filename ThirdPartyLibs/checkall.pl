#!/usr/bin/perl

system ("cd astyle; perl checkall.pl");
system ("cd doxygen; perl checkall.pl");
system ("cd openssl; perl buildall.pl $BLD_TRG");
system ("cd Xerces; perl checkall.pl");
