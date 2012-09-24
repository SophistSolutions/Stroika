#!/usr/bin/perl

print ("Checking Samples...\n");
print ("  ActiveLedIt...   "); system ("cd ActiveLedit; perl checkall.pl");
print ("  Ledit...         "); system ("cd Ledit; perl checkall.pl");
print ("  SimpleLedTest... "); system ("cd SimpleLedTest; perl checkall.pl");
print ("  WebServer...     "); system ("cd WebServer; perl checkall.pl");
