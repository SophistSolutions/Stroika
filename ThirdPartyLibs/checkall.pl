#!/usr/bin/perl

system ("cd Xerces; perl checkall.pl");
system ("cd doxygen; perl checkall.pl");
system ("cd astyle; perl checkall.pl");
