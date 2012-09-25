#!/usr/bin/perl

print ("Checking Samples...\n");
print ("  ActiveLedIt...   "); system ("cd ActiveLedIt; perl checkall.pl");
print ("  Ledit...         "); system ("cd LedIt; perl checkall.pl");
print ("  SimpleLedTest... "); system ("cd SimpleLedTest; perl checkall.pl");
print ("  WebServer...     "); system ("cd WebServer; perl checkall.pl");
