#!/usr/bin/perl

print ("Checking...\n");
system ("cd Library; perl checkall.pl");
system ("cd Tools; perl checkall.pl");
system ("cd Tests; perl checkall.pl");
