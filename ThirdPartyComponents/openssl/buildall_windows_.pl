#!/usr/bin/perl 

#@todo FIX SO BUILDS ASM VERISON FOR WINDOWS!


require ("../../ScriptsLib/StringUtils.pl");

#print "PATH=";
#print (" $ENV{'PATH'};");

sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}
my	$thisScriptDir	=	GetThisScriptDir ();

require "$thisScriptDir/../../ScriptsLib/ConfigurationReader.pl";

$activeConfig = $ENV{'CONFIGURATION'};
my $useProjectDir= "$thisScriptDir/../../Library/Projects/" . GetProjectPlatformSubdirIfAny ($activeConfig);
#if (-e "$useProjectDir/SetupBuildCommonVars.pl") {
	require "$useProjectDir/SetupBuildCommonVars.pl";
#}

my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};
delete $ENV{'MAKELEVEL'};
delete $ENV{'MAKE_TERMERR'};
delete $ENV{'MAKE_TERMOUT'};
delete $ENV{'SHLVL'};
delete $ENV{'MFLAGS'};


sub	RunAndStopOnFailure
{
	my $cmd2Run = $_[0];
	print ("      $cmd2Run...\n");
	my $result = system ($cmd2Run);
	if ($result != 0) {
		die ("Run result = $result\r\n");
	}
}

sub	CopyBuilds2Out
{
	my $trgDir = $_[0];
	my $bldOutDir = $_[1];
	my $tmpSymOutDir = $_[2];
	system ("mkdir -p CURRENT/Builds/$trgDir");
	system ("cp -r CURRENT/inc32/ CURRENT/Builds/$trgDir/Includes");
	system ("cp CURRENT/$bldOutDir/libeay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/ssleay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$tmpSymOutDir/lib.pdb CURRENT/$tmpSymOutDir/app.pdb CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/openssl.* CURRENT/Builds/$trgDir/");
}

system ("rm -rf CURRENT/Builds");

#REM - only reconfigure if we just did the extract
if (index ($activeConfig, "U-32") != -1) {
	chdir ("CURRENT");
		print ("\n ...Configuring openssl 32-bit...\n");
		RunAndStopOnFailure ("perl Configure VC-WIN32 no-asm --prefix=c:/some/openssl/dir");
		#RunAndStopOnFailure ("perl Configure VC-WIN32 --prefix=c:/some/openssl/dir");
		## BASED ON ms\do_ms.bat
		system ("perl util/mkfiles.pl >MINFO");
		system ("perl util/mk1mf.pl no-asm VC-WIN32 >ms/nt.mak");
		system ("perl util/mk1mf.pl debug no-asm VC-WIN32 >ms/nt-DBG.mak");
		system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
		system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
		system ("rm -f NUL");
		system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends
	
		if (index ($activeConfig, "Release") != -1) {
			print (" ...Make Release...\n");
			#nb: lose -s to see each compile line
			#RunAndStopOnFailure ("(nmake /NOLOGO /S /f ms/nt.mak MAKEFLAGS= 2>&1) > NT.MAK.BUILD-Output.txt");
			open(my $fh, '>', 'doRun32NDB.bat');
			print $fh GetString2InsertIntoBatchFileToInit32BitCompiles();
			print $fh "echo nmake /NOLOGO /S /f ms\\nt.mak MAKEFLAGS=\n";
			print $fh "echo PATH=%PATH%\n";
			print $fh "nmake /NOLOGO /S /f ms/nt.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRun32NDB.bat) 2>&1 > NT32.MAK.BUILD-Output.txt");
		}
	
		if (index ($activeConfig , "Debug") != -1) {
			print (" ...Make Debug...\n");
			#nb: lose -s to see each compile line
			open(my $fh, '>', 'doRun32Dbg.bat');
			print $fh GetString2InsertIntoBatchFileToInit32BitCompiles();
			print $fh "nmake /NOLOGO /f ms/nt-DBG.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRun32Dbg.bat 2>&1) > NT32-DBG.MAK.BUILD-Output.txt");
		}
	
		if (index ($activeConfig, "Release") != -1) {
			print (" ...Running openssl tests (output TEST32-OUT.txt)...");
			system ("((nmake /NOLOGO /S /f ms/nt.mak test MAKEFLAGS=)  2>&1) > TEST32-OUT.txt");
			if (index (`grep "passed all tests" TEST32-OUT.txt`, 'passed all tests') == -1) {
				die ("tests failed");
			}
		}
		if (index ($activeConfig, "Debug") != -1) {
			print (" ...Running openssl tests (output TEST32-DBG-OUT.txt)...");
			system ("((nmake /NOLOGO /S /f ms/nt-DBG.mak test MAKEFLAGS=) 2>&1) > TEST32-DBG-OUT.txt");
			if (index (`grep "passed all tests" TEST32-DBG-OUT.txt`, 'passed all tests') == -1) {
				die ("tests failed");
			}
		}
		print ("done\n");
	chdir ("..");
	if (index ($activeConfig, "Release") != -1) {
		CopyBuilds2Out ("Release32", "out32", "tmp32");
	}
	if (index ($activeConfig, "Debug") != -1) {
		CopyBuilds2Out ("Debug32", "out32.dbg", "tmp32.dbg");
	}
}

if (index ($activeConfig, "U-64") != -1) {
	chdir ("CURRENT");
		print ("\n ...Configuring openssl 64-bit...\n");

		###tmphack to test workaround bug with this 64-bit make code. Probably most of this is not needed, but
		###not sure what is!!!
		###	-- LGP 2015-11-29
		RunAndStopOnFailure ("perl Configure VC-WIN32 no-asm --prefix=c:/some/openssl/dir");
		#RunAndStopOnFailure ("perl Configure VC-WIN32 --prefix=c:/some/openssl/dir");
		## BASED ON ms\do_ms.bat
		system ("perl util/mkfiles.pl >MINFO");
		system ("perl util/mk1mf.pl no-asm VC-WIN32 >ms/nt.mak");
		system ("perl util/mk1mf.pl debug no-asm VC-WIN32 >ms/nt-DBG.mak");
		system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
		system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
		system ("rm -f NUL");
		system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends


		RunAndStopOnFailure ("perl Configure no-asm VC-WIN64A");
			
		open(my $fh, '>', 'doRun64Configure.bat');
		print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
		print $fh "ms\\do_win64a.bat\n";
		#close $fh;

		system ("cmd /c \"ms\\do_win64a.bat\"");
		#I THINK OBSOLETE/UNNEEDED - BAD COPY FROM 32-bit days...system ("perl util/mk1mf.pl debug no-asm VC-WIN64A >ms/nt-DBG.mak");

		system ("rm -f NUL");
		system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends
	
		if (index ($activeConfig, "Release") != -1) {
			print (" ...Make Release64...\n");
			open(my $fh, '>', 'doRun64NDB.bat');
			print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
			print $fh "nmake /NOLOGO /S /f ms/nt.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRun64NDB.bat 2>&1) > NT64.MAK.BUILD-Output.txt");
		}
	
		if (index ($activeConfig, "Debug") != -1) {
			print (" ...Make Debug64...\n");
			open(my $fh, '>', 'doRun64Dbg.bat');
			print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
			print $fh "nmake /NOLOGO /f ms/nt-DBG.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRun64Dbg.bat 2>&1) > NT64-DBG.MAK.BUILD-Output.txt");
		}

		print (" ...Running openssl tests (output TEST64-OUT.txt, TEST64-DBG-OUT.txt)...");
		if (index ($activeConfig, "Release") != -1) {
			system ("((nmake /NOLOGO /S /f ms/nt.mak test MAKEFLAGS=)  2>&1) > TEST64-OUT.txt");
		}
		if (index ($activeConfig, "Debug") != -1) {
			system ("((nmake /NOLOGO /S /f ms/nt-DBG.mak test MAKEFLAGS=) 2>&1) > TEST64-DBG-OUT.txt");
		}
		print ("done\n");
	chdir ("..");
	
	if (index ($activeConfig, "Release") != -1) {
		CopyBuilds2Out ("Release64", "out32", "tmp32");
	}
	if (index ($activeConfig, "Debug") != -1) {
		CopyBuilds2Out ("Debug64", "out32.dbg", "tmp32.dbg");
	}
}
