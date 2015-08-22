#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;


require "../../../ScriptsLib/ConfigurationReader.pl";


my $projectPlatformSubdir = GetProjectPlatformSubdir ();

#CRUDDY IMPL - but avoid runnign 64 bit binaries on 32-bit OS
sub bitter {
     my $bit;
     if ($ENV{'ProgramFiles(x86)'} eq '') {
         $bit = "32";
     }
     else {
         $bit = "64";
     }
     return $bit;
}

sub PrepareOutputDir
{
	use File::Path;
	local $outDir = 'Output';
	rmtree("$outDir");
	mkdir "$outDir", 0777;
	return $outDir;
}

sub DoRunSimpleTestArgv
{
	local $dbgOrRel = $_[0];
	local $tstName = $_[1];
	local $dirPrefix = $_[2];
	local $suffixFromTrgDir = $_[3];

	if ($dbgOrRel eq "") {
		$dbgOrRel = 'All';
	}

	local $outDir = PrepareOutputDir();
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
#TMPHACK...til we move stuff and get right targets list
$dbgOrRel = '';
}
	if ($dbgOrRel eq 'All') {
		DoRunSimpleTest ('Debug-U-32', $outDir, $tstName, $dirPrefix, $suffixFromTrgDir);
		DoRunSimpleTest ('Release-U-32', $outDir, $tstName, $dirPrefix, $suffixFromTrgDir);
		if (bitter() eq 64) {
			DoRunSimpleTest ('Debug-U-64', $outDir, $tstName, $dirPrefix, $suffixFromTrgDir);
			DoRunSimpleTest ('Release-U-64', $outDir, $tstName, $dirPrefix, $suffixFromTrgDir);
		}
	}
	else {
		DoRunSimpleTest ($dbgOrRel, $outDir, $tstName, $dirPrefix, $suffixFromTrgDir);
	}
}

sub DoRunSimpleTest
{
	local $dbgOrRel = $_[0];
	local $outDir = $_[1];
	local $tstName = $_[2];
	local $dirPrefix = $_[3];
	local $suffixFromTrgDir = $_[4];
	local $extraRunArgs = $_[5];
	
	local $r = DoRunSimpleTest_R ($dbgOrRel, $outDir, $tstName, $dirPrefix, $suffixFromTrgDir, $extraRunArgs);
	print $r;
}

#RunEXE
sub DoRunSimpleTest_EXE_R
{
	local $dbgOrRel = $_[0];
	local $outDir = $_[1];
	local $tstName = $_[2];
	local $extraRunArgs = $_[3];
	local $EXE = $_[4];
	
	local $startTime = time();

	#my $STDOUT_LOGS='Estdout_GDB_'.time();
	#my $ERR_LOGS='Error_GDB_'.time();
	my $STDOUT_LOGS=$outDir . '/StdOut-' . $dbgOrRel;
	my $ERR_LOGS=$outDir . '/StdErr-' . $dbgOrRel;

	local *CATCHOUT = new IO::File "> $STDOUT_LOGS";
	local *CATCHERR = new IO::File "> $ERR_LOGS";


	use IPC::Open3;
	use Symbol qw(gensym);
	use IO::File;
	#print "RUNNING: ", "$EXE $extraRunArgs", "\r\n";
	my $pid = open3(gensym, ">&CATCHOUT", ">&CATCHERR", "$EXE $extraRunArgs");
	waitpid($pid, 0);
	
	local $diffTime = time() - $startTime;
	my $MyStatus = $?;
	if ($MyStatus == 0) {
		return "[SUCCEEDED]\t$dbgOrRel\t($diffTime seconds)\t$tstName ($EXE $extraRunArgs)\r\n";
	}
	else {
		return "[FAILED]\t$dbgOrRel\t($diffTime seconds)\t$tstName ($EXE $extraRunArgs)\r\n";
	}
}

sub DoRunSimpleTest_R
{
	local $dbgOrRel = $_[0];
	local $outDir = $_[1];
	local $tstName = $_[2];
	local $dirPrefix = $_[3];
	local $suffixFromTrgDir = $_[4];
	local $extraRunArgs = $_[5];

	return DoRunSimpleTest_EXE_R ($dbgOrRel, $outDir, $tstName, $extraRunArgs, "$dirPrefix$dbgOrRel/$suffixFromTrgDir");
}


sub RunDiff
{
	local $f1 = $_[0];
	local $f2 = $_[1];
	local $maxDiffs = $_[2];
	
	# figure  out how todo overloading like this - check for NULL,""???
	if ($maxDiffs eq '') {
		$maxDiffs = 0;
	}
	return RunDiff3 ($f1, $f2, $maxDiffs);
}

sub RunDiff3
{
	local $f1 = $_[0];
	local $f2 = $_[1];
	local $maxDiffs = $_[2];

	local $val =  `diff -c --text -b $f1 $f2 | egrep -c \"^[!+-]\"`;
	chomp ($val);
	if ($val <= $maxDiffs) {
		if ($maxDiffs == 0) {
			print "[SUCCEEDED]\t\t\tdiff -b $f1 $f2\r\n";
		}
		else {
			print "[SUCCEEDED]\t\t\tdiff -b $f1 $f2 (count = $val, max=$maxDiffs)\r\n";
		}
	}
	else {
		print "[FAILED]\t\t\tdiff -b $f1 $f2 (count = $val, max=$maxDiffs)\r\n";
	}
}

1
