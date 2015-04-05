#!/usr/bin/perl 

#### NOTE - DONT CALL DIRECTLY - LEGACY CODE FROM OLD MAKE PROCESS TO BE MERGED
#### INTO MAKEFILE


require ("../../ScriptsLib/BuildUtils.pl");
require ("../../ScriptsLib/StringUtils.pl");

#print "PATH=";
#print (" $ENV{'PATH'};");

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};
delete $ENV{'MAKELEVEL'};
delete $ENV{'MAKE_TERMERR'};
delete $ENV{'MAKE_TERMOUT'};
delete $ENV{'SHLVL'};
delete $ENV{'MFLAGS'};


sub	CopyBuilds2Out
{
	my $trgDir = $_[0];
	my $bldOutDir = $_[1];
	system ("mkdir -p CURRENT/Builds/$trgDir");
	system ("cp -r CURRENT/inc32/ CURRENT/Builds/$trgDir/Includes");
	system ("cp CURRENT/$bldOutDir/libeay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/ssleay32.* CURRENT/Builds/$trgDir/");
	system ("cp CURRENT/$bldOutDir/openssl.* CURRENT/Builds/$trgDir/");
}


#REM - only reconfigure if we just did the extract
if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
	unless (-e "CURRENT/CONFIG.OUT") {
		print ("Creating CURRENT/CONFIG.OUT - new configuration\n");
		system ("cd CURRENT ; ./config -no-shared 2>&1  > CONFIG.OUT");
	}
	if (-e "CURRENT/libssl.a") {
		print (" ...Skipping build (already built)\n");
	}
	else {
		system ("make --directory CURRENT --no-print-directory -s all");
	}
	
	print (" ...Running openssl tests...");
	system ("make --directory CURRENT --no-print-directory -s test > CURRENT/TEST-OUT.txt 2>& 1");
	print ("\n");
}
else {if ("$^O" eq "cygwin") {
	if ((-e 'CURRENT/Builds/Debug32') && (-e 'CURRENT/Builds/Release32')) {
		print (" ...Skipping 32-bit build (already built)\n");
	}
	else {
		chdir ("CURRENT");
			RunAndStopOnFailure ("perl Configure VC-WIN32 no-asm --prefix=c:/some/openssl/dir");
			## BASED ON ms\do_ms.bat
			system ("perl util/mkfiles.pl >MINFO");
			system ("perl util/mk1mf.pl no-asm VC-WIN32 >ms/nt.mak");
			system ("perl util/mk1mf.pl debug no-asm VC-WIN32 >ms/nt-DBG.mak");
			system ("perl util/mkdef.pl 32 libeay > ms/libeay32.def");
			system ("perl util/mkdef.pl 32 ssleay > ms/ssleay32.def");
			system ("rm -f NUL");
			system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends
	
			print (" ...Make Release...\n");
			#nb: lose -s to see each compile line
			RunAndStopOnFailure ("(nmake /NOLOGO /S /f ms/nt.mak MAKEFLAGS= 2>&1) | tee -a NT.MAK.BUILD-Output.txt");
	
			print (" ...Make Debug...\n");
			#nb: lose -s to see each compile line
			system ("(nmake /NOLOGO /S /f ms/nt-DBG.mak MAKEFLAGS=  2>&1) | tee -a NT-DBG.MAK.BUILD-Output.txt");
	
			print (" ...Running openssl tests...");
			system ("(nmake /NOLOGO /S /f ms/nt.mak test MAKEFLAGS=  2>&1) > TEST-OUT.txt");
			system ("(nmake /NOLOGO /S /f ms/nt-DBG.mak test MAKEFLAGS= 2>&1) > TEST-DBG-OUT.txt");
			print ("\n");
		chdir ("..");
		CopyBuilds2Out ("Release32", "out32");
		CopyBuilds2Out ("Debug32", "out32.dbg");
	}


	if ((-e 'CURRENT/Builds/Debug64') && (-e 'CURRENT/Builds/Release64')) {
		print (" ...Skipping 64-bit build (already built)\n");
	}
	else {
		chdir ("CURRENT");
			#system ("perl Configure VC-WIN64A no-asm --prefix=c:/some/openssl/dir");
			system ("perl Configure no-asm VC-WIN64A");
			system ("cmd /c \"ms\\do_win64a.bat\"");
			system ("perl util/mk1mf.pl debug no-asm VC-WIN64A >ms/nt-DBG.mak");

			system ("rm -f NUL");
			system ("rm -rf tmp32 tmp32.dbg out32 out32.dbg");	# cuz changing proj files might not have right depends
	
			print (" ...Make Release64...\n");
			open(my $fh, '>', 'doRunNDB.bat');
			print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
			print $fh "nmake /NOLOGO /S /f ms/nt.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRunNDB.bat 2>&1) | tee -a NT.MAK.BUILD-Output.txt");
	
			print (" ...Make Debug64...\n");
			open(my $fh, '>', 'doRunDbg.bat');
			print $fh GetString2InsertIntoBatchFileToInit64BitCompiles();
			print $fh "nmake /NOLOGO /f ms/nt-DBG.mak MAKEFLAGS=\n";
			close $fh;
			RunAndStopOnFailure ("(cmd /c doRunDbg.bat 2>&1) | tee -a NT64-DBG.MAK.BUILD-Output.txt");

			print (" ...Running openssl tests...");
			system ("(nmake /NOLOGO /S /f ms/nt.mak test MAKEFLAGS=  2>&1) > TEST-OUT.txt");
			system ("(nmake /NOLOGO /S /f ms/nt-DBG.mak test MAKEFLAGS= 2>&1) > TEST-DBG-OUT.txt");
			print ("\n");
		chdir ("..");
	
		CopyBuilds2Out ("Release64", "out32");
		CopyBuilds2Out ("Debug64", "out32.dbg");
	}
}}
