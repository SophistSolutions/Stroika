#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

BEGIN{ @INC = ( "./", @INC ); }

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $configurationFiles	=	GetThisScriptDir() . "/../ConfigurationFiles";
my $masterXMLConfigFile	=	"";

my @useExtraCDefines;
my @useExtraMakeDefines;
my @packageConfigLinkLines;

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

	#print ("ENTERING ReadConfigFile_ ($masterXMLConfigFile)\n");
	open (FILE, "$masterXMLConfigFile") or $fileErr = "Unable to open $masterXMLConfigFile";
	if ($fileErr ne '') {
		return;
	}
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my $pps = ReadValue_($line, "<Configure-Command-Line>");
		if (defined $pps) {
			$configuration {'Configure-Command-Line'} = $pps;
		}
		my $pps = ReadValue_($line, "<ProjectPlatformSubdir>");
		if (defined $pps) {
			$configuration {'ProjectPlatformSubdir'} = $pps;
		}
		my $pps = ReadValue_($line, "<TARGET_PLATFORMS>");
		if (defined $pps) {
			$configuration {'TARGET_PLATFORMS'} = $pps;
		}
		my $pps = ReadValue_($line, "<TOOLS_PATH_ADDITIONS>");
		if (defined $pps) {
			$configuration {'TOOLS_PATH_ADDITIONS'} = $pps;
		}
		my $pps = ReadValue_($line, "<TOOLS_PATH_ADDITIONS_BUGWORKAROUND>");
		if (defined $pps) {
			$configuration {'TOOLS_PATH_ADDITIONS_BUGWORKAROUND'} = $pps;
		}
		my $pps = ReadValue_($line, "<ARCH>");
		if (defined $pps) {
			$configuration {'ARCH'} = $pps;
		}
		my $pps = ReadValue_($line, "<ConfigTags>");
		if (defined $pps) {
			$configuration {'ConfigTags'} = $pps;
		}
		my $pps = ReadValue_($line, "<Linker>");
		if (defined $pps) {
			$configuration {'Linker'} = $pps;
		}
		my $pps = ReadValue_($line, "<AS>");
		if (defined $pps) {
			$configuration {'AS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CXX>");
		if (defined $pps) {
			$configuration {'CXX'} = $pps;
		}
		my $pps = ReadValue_($line, "<CC>");
		if (defined $pps) {
			$configuration {'CC'} = $pps;
		}
		my $pps = ReadValue_($line, "<HasMakefileBugWorkaround_lto_skipping_undefined_incompatible>");
		if (defined $pps) {
			$configuration {'HasMakefileBugWorkaround_lto_skipping_undefined_incompatible'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_boost>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_boost'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LibCurl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LibCurl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ActivePerl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ActivePerl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_OpenSSL>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_OpenSSL'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_OpenSSL_ExtraArgs>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_OpenSSL_ExtraArgs'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_Xerces>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_Xerces'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ZLib>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ZLib'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_sqlite>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_sqlite'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LZMA>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LZMA'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_WinHTTP>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_WinHTTP'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_WIX>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_WIX'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ATLMFC>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ATLMFC'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbolsInLibraries>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbolsInLibraries'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbolsInExecutables>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbolsInExecutables'} = $pps;
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
		my $pps = ReadValue_($line, "<CWARNING_FLAGS>");
		if (defined $pps) {
			$configuration {'CWARNING_FLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<AR>");
		if (defined $pps) {
			$configuration {'AR'} = $pps;
		}
		my $pps = ReadValue_($line, "<MIDL>");
		if (defined $pps) {
			$configuration {'MIDL'} = $pps;
		}
		my $pps = ReadValue_($line, "<RC>");
		if (defined $pps) {
			$configuration {'RC'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIBTOOL>");
		if (defined $pps) {
			$configuration {'LIBTOOL'} = $pps;
		}
		my $pps = ReadValue_($line, "<RANLIB>");
		if (defined $pps) {
			$configuration {'RANLIB'} = $pps;
		}
		my $pps = ReadValue_($line, "<STRIP>");
		if (defined $pps) {
			$configuration {'STRIP'} = $pps;
		}
		my $pps = ReadValue_($line, "<CPPFLAGS>");
		if (defined $pps) {
			$configuration {'CPPFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CFLAGS>");
		if (defined $pps) {
			$configuration {'CFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CXXFLAGS>");
		if (defined $pps) {
			$configuration {'CXXFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_COMPILER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_COMPILER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<INCLUDES_PATH>");
		if (defined $pps) {
			$configuration {'INCLUDES_PATH'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIBS_PATH>");
		if (defined $pps) {
			$configuration {'LIBS_PATH'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIB_DEPENDENCIES>");
		if (defined $pps) {
			$configuration {'LIB_DEPENDENCIES'} = $pps;
		}
		my $pps = ReadValue_($line, "<CrossCompiling>");
		if (defined $pps) {
			$configuration {'CrossCompiling'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_PREFIX_LINKER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_PREFIX_LINKER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_SUFFIX_LINKER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_SUFFIX_LINKER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<RUN_PREFIX>");
		if (defined $pps) {
			$configuration {'RUN_PREFIX'} = $pps;
		}
		my $pps = ReadValue_($line, "<CDefine>");
		if (defined $pps) {
			$useExtraCDefines[@useExtraCDefines] = $pps;
		}
		my $pps = ReadValue_($line, "<MakeDefine>");
		if (defined $pps) {
			push (@useExtraMakeDefines, $pps);
		}
		my $pps = ReadValue_($line, "<PkgConfigLinkLineAppendage>");
		if (defined $pps) {
			push (@packageConfigLinkLines, $pps);
		}
	print (OUT "    </PkgConfigLinkLineAppendages>\n");

	}
	$configuration {'ExtraCDefines'} = \@useExtraCDefines;
	$configuration {'ExtraMakeDefines'} = \@useExtraMakeDefines;
	$configuration {'PackageConfigLinkLines'} = \@packageConfigLinkLines;
}




### PUBLIC APIS
sub	GetProjectPlatformSubdir {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'ProjectPlatformSubdir');
}

sub	GetProjectPlatformSubdirIfAny {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'ProjectPlatformSubdir');
}

my $lastReadConfig	=	null;

sub	GetConfigurationParameter {
    my $configName =  $_[0];
	#print ("ENTERING GetConfigurationParameter (configName = $configName)\n");

	if ($fileErr ne "") {
		print ("Cannot GetConfigurationParameter: $fileErr");
		exit (1);
	}

	if ($lastReadConfig ne $configName) {
		@useExtraCDefines = ();
		@useExtraMakeDefines = ();
		@useExtraMakeDefines = ();
		@packageConfigLinkLines = ();
		%configuration = ();

		$masterXMLConfigFile	=	"$configurationFiles/$configName.xml";

		ReadConfigFile_ ();
		$lastReadConfig = $configName;
	}

	my $paramName = $_[1];
	
	# crazy hack cuz storing arrays into a hash screws them up in perl. What a terrible
	# language  choice!
	if ($paramName eq "ExtraCDefines") {
		return @useExtraCDefines;
	}
	if ($paramName eq "ExtraMakeDefines") {
		return @useExtraMakeDefines;
	}
	if ($paramName eq "PackageConfigLinkLines") {
		return @packageConfigLinkLines;
	}
	
	my $result = $configuration{$paramName};

	# aliases
	if ($result eq "") {
		if ($paramName eq "CompilerDriver-C") {
			## ALIAS DEPRECATED
			return GetConfigurationParameter ($configName, "CC");
		}
		if ($paramName eq "CompilerDriver-C++") {
			## ALIAS DEPRECATED
			return GetConfigurationParameter ($configName, "CXX");
		}
	}

	#default values
	if ($result eq "") {
		if ($paramName eq "LIB_SUFFIX") {
			my $PROJECTPLATFORMSUBDIR = GetConfigurationParameter($configName, "ProjectPlatformSubdir");
			if ($PROJECTPLATFORMSUBDIR eq "Unix") {
				return ".a";
			}
			elsif ($PROJECTPLATFORMSUBDIR =~ /^VisualStudio.Net/) {
				return ".lib";
			}
		}
		if ($paramName eq "OBJ_SUFFIX") {
			my $PROJECTPLATFORMSUBDIR = GetConfigurationParameter($configName, "ProjectPlatformSubdir");
			if ($PROJECTPLATFORMSUBDIR eq "Unix") {
				return ".o";
			}
			elsif ($PROJECTPLATFORMSUBDIR =~ /^VisualStudio.Net/) {
				return ".obj";
			}
		}
		if ($paramName eq "EXE_SUFFIX") {
			my $PROJECTPLATFORMSUBDIR = GetConfigurationParameter($configName, "ProjectPlatformSubdir");
			if ($PROJECTPLATFORMSUBDIR eq "Unix") {
				return "";
			}
			elsif ($PROJECTPLATFORMSUBDIR =~ /^VisualStudio.Net/) {
				return ".exe";
			}
		}
	}


	#print ("RETURNING paramname=$paramName: $configuration{$paramName}\n");
	return $result;
}

1