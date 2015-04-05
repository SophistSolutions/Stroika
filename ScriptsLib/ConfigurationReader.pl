#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $configurationFiles	=	GetThisScriptDir() . "/../ConfigurationFiles";
my $masterXMLConfigFile	=	"$configurationFiles/DefaultConfiguration.xml";

require (GetThisScriptDir () + "StringUtils.pl");

my @useExtraCDefines;
my @useExtraMakeDefines;


#
# BUILD iff LIBFEATUREFLAG_BuildOnly OR LIBFEATUREFLAG_UseStaticTPP
# HAS_FEATURE iff LIBFEATUREFLAG_UseStaticTPP OR LIBFEATUREFLAG_UseSystem
#
my $LIBFEATUREFLAG_BuildOnly = "build-only";
my $LIBFEATUREFLAG_UseStaticTPP = "use";
my $LIBFEATUREFLAG_UseSystem = "use-system";
my $LIBFEATUREFLAG_No = "no";

my $fileErr = "";

# Declare the subroutines
sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}

sub	GetAllOfStringToEndTag($) {
	my $string = shift;
	my $eos = index($string, "<");
	if ($eos == -1) {
		$string;
	}
	else {
		return substr ($string, 0, $eos);
	}
}

my %configuration = ();


sub	ReadValue_ {
	my $line = shift;
	my $tag = shift;
	my $startOff = index($line, $tag);
	if ($startOff != -1) {
		my $tmp = substr ($line, $startOff + length($tag));
		return GetAllOfStringToEndTag ($tmp);
	}
	return undef;
}

sub	ReadConfigFile_ {

	my $myDirName	=	GetThisScriptDir ();


	open (FILE, "$masterXMLConfigFile") or $fileErr = "Unable to open $masterXMLConfigFile";
	if ($fileErr ne '') {
		return;
	}
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my $pps = ReadValue_($line, "<ProjectPlatformSubdir>");
		if (defined $pps) {
			$configuration {'ProjectPlatformSubdir'} = $pps;
		}
		my $pps = ReadValue_($line, "<Platform>");
		if (defined $pps) {
			$configuration {'Platform'} = $pps;
		}
		my $pps = ReadValue_($line, "<Target>");
		if (defined $pps) {
			$configuration {'Target'} = $pps;
		}
		my $pps = ReadValue_($line, "<OptimizerFlag>");
		if (defined $pps) {
			$configuration {'OptimizerFlag'} = $pps;
		}
		my $pps = ReadValue_($line, "<CompilerDriver>");
		if (defined $pps) {
			$configuration {'CompilerDriver'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LibCurl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LibCurl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_OpenSSL>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_OpenSSL'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_Xerces>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_Xerces'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ZLib>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ZLib'} = $pps;
		}
		my $pps = ReadValue_($line, "<qHasFeature_WinHTTP>");
		if (defined $pps) {
			$configuration {'qHasFeature_WinHTTP'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbtolsInExecutables>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbtolsInExecutables'} = $pps;
		}
		my $pps = ReadValue_($line, "<STATIC_LINK_GCCRUNTIME>");
		if (defined $pps) {
			$configuration {'STATIC_LINK_GCCRUNTIME'} = $pps;
		}
		my $pps = ReadValue_($line, "<ENABLE_TRACE2FILE>");
		if (defined $pps) {
			$configuration {'ENABLE_TRACE2FILE'} = $pps;
		}
		my $pps = ReadValue_($line, "<ENABLE_ASSERTIONS>");
		if (defined $pps) {
			$configuration {'ENABLE_ASSERTIONS'} = $pps;
		}
		my $pps = ReadValue_($line, "<ENABLE_GLIBCXX_DEBUG>");
		if (defined $pps) {
			$configuration {'ENABLE_GLIBCXX_DEBUG'} = $pps;
		}
		my $pps = ReadValue_($line, "<CPPSTD_VERSION_FLAG>");
		if (defined $pps) {
			$configuration {'CPPSTD_VERSION_FLAG'} = $pps;
		}
		my $pps = ReadValue_($line, "<STATIC_LINK_GCCRUNTIME>");
		if (defined $pps) {
			$configuration {'STATIC_LINK_GCCRUNTIME'} = $pps;
		}
		my $pps = ReadValue_($line, "<CDefine>");
		if (defined $pps) {
			$useExtraCDefines[@useExtraCDefines] = $pps;
		}
		my $pps = ReadValue_($line, "<MakeDefine>");
		if (defined $pps) {
			push (@useExtraMakeDefines, $pps);
		}
	}
	$configuration {'ExtraCDefines'} = \@useExtraCDefines;
	$configuration {'ExtraMakeDefines'} = \@useExtraMakeDefines;
}




### PUBLIC APIS
sub	GetProjectPlatformSubdir {
	if ($fileErr ne "") {
		print ("Cannot GetProjectPlatformSubdir: $fileErr");
		exit (1);
	}
	if ($configuration{'ProjectPlatformSubdir'} eq "") {
		die ("GetProjectPlatformSubdir () EMPTY\n");
	}
	return $configuration{'ProjectPlatformSubdir'};
}

sub	GetProjectPlatformSubdirIfAny {
	if ($fileErr ne "") {
		return "";
	}
	if ($configuration{'ProjectPlatformSubdir'} eq "") {
		return "";
	}
	return $configuration{'ProjectPlatformSubdir'};
}

sub	GetConfigurationParameter {
	if ($fileErr ne "") {
		print ("Cannot GetConfigurationParameter: $fileErr");
		exit (1);
	}
	my $paramName = shift;
	
	# crazy hack cuz storing arrays into a hash screws them up in perl. What a terrible
	# language  choice!
	if ($paramName eq "ExtraCDefines") {
		return @useExtraCDefines;
	}
	if ($paramName eq "ExtraMakeDefines") {
		return @useExtraMakeDefines;
	}
	
	return $configuration{$paramName};
}


###tmphack - later enum configs and make above stuff 
sub	GetConfigurations {
	@names = ("DefaultConfiguration");
	return @names;
}

### we probably will run makefiles with 'all configs' or specific config. We 
### may set that current config into an environent var. But for now - just
### return magic value - config NAME
sub	GetActiveConfigurationName {
	return "DefaultConfiguration";
}




###PRIVATE
sub Prepare_ () {
	if ($#configuration < 0) {
		ReadConfigFile_ ();
	}
	#comment in to see values
	#print "CONFIGURE-ProjectPlatformSubdir: ", GetProjectPlatformSubdir(), "\n";
	return 1;
}



Prepare_ ();
