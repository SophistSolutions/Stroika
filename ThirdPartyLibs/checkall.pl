#!/usr/bin/perl

system ("cd doxygen; perl checkall.pl");
system ("cd openssl; perl checkall.pl");
system ("cd Xerces; perl checkall.pl");
system ("make --directory zlib --no-print-directory check");
