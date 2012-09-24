#!/usr/bin/perl

print ("Checking Samples...\n");
system ("cd ActiveLedit; perl checkall.pl");
system ("cd Ledit; perl checkall.pl");
system ("cd SimpleLedTest; perl checkall.pl");
system ("cd WebServer; perl checkall.pl");
