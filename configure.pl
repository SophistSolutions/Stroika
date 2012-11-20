#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

use constant false => 0;
use constant true  => 1;


my @genConfigureArgs;
my @applyConfigureArgs;


sub	DoHelp_
{
	print("Usage:\n");
	print("	configure.pl OPTIONS where options can be:\n");
	print("	    --force                      /* forces rebuild of the given configuration */\n");
	print("	    --only-if-unconfigured       /* Opposite of --force - only rebuilds the configfiles if absent */\n");
	print("	    --help                       /* Show this message */\n");
	print(" ... Other parameters passed through to GenerateConfiguration.pl\n");
	exit (0);
}

sub	ParseCommandLine_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if (lc ($var) eq "-force" or lc ($var) eq "--force") {
			$applyConfigureArgs[@applyConfigureArgs] = $var;
			$genConfigureArgs[@genConfigureArgs] = $var;
		}
		elsif ((lc ($var) eq "-only-if-unconfigured") or (lc ($var) eq "--only-if-unconfigured")) {
			$applyConfigureArgs[@applyConfigureArgs] = $var;
			$genConfigureArgs[@genConfigureArgs] = $var;
		}
		elsif ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
		else {
			$genConfigureArgs[@genConfigureArgs] = $var;
		}
	}
}

sub	DoRun_
{
	$newArgs = "perl GenerateConfiguration.pl ";
	foreach $var (@genConfigureArgs)
	{
	 $newArgs = $newArgs . " " . $var;
	}
	system ($newArgs);


	$newArgs = "perl ApplyConfiguration.pl ";
	foreach $var (@applyConfigureArgs)
	{
	 $newArgs = $newArgs . " " . $var;
	}
	system ($newArgs);
}



ParseCommandLine_ ();
DoRun_();

