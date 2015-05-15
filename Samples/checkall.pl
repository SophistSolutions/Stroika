#!/usr/bin/perl

print ("Checking Samples...\n");
print ("   ActiveLedIt...             "); system ("cd ActiveLedIt; perl checkall.pl");
print ("   LedIt...                   "); system ("cd LedIt; perl checkall.pl");
print ("   LedLineIt...               "); system ("cd LedLineIt; perl checkall.pl");
print ("   SimpleLedTest...           "); system ("cd SimpleLedTest; perl checkall.pl");
print ("   SSDPClient...              "); system ("cd SSDPClient; perl checkall.pl");
print ("   SSDPServer...              "); system ("cd SSDPServer; perl checkall.pl");
print ("   SimpleService...           "); system ("cd SimpleService; perl checkall.pl");
print ("   SystemPerformanceClient... "); system ("cd SystemPerformanceClient; perl checkall.pl");
print ("   WebServer...               "); system ("cd WebServer; perl checkall.pl");
