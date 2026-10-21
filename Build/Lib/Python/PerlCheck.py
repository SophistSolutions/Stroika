"""
While Stroika's configure and ApplyConfiguration move from Perl to Python
(https://github.com/SophistSolutions/Stroika/issues/1240), each Python script computes its own result, then runs the old
Perl one (kept beside it as NAME.pl) into a scratch directory and compares every file the two write. When they differ, the
PERL result is the one used - with a loud warning, and the diff kept under IntermediateFiles/configure-mismatch/ - so a
Python mistake can never change a build. When every release run and CI job agrees, the Perl goes, and this with it.

STROIKA_SKIP_PERL_CHECK=1 skips the Perl run (for speed); with no perl on PATH it is skipped too, and the run says so.
"""

import difflib
import os
import shutil
import subprocess
import sys
import tempfile

MISMATCH_TAG = "PYTHON/PERL CONFIGURATION MISMATCH"  # what the regression-results scan looks for
MISMATCH_DIR = "IntermediateFiles/configure-mismatch"


def PerlCheckWanted():
    if os.environ.get("STROIKA_SKIP_PERL_CHECK", "") not in ("", "0"):
        return False
    if shutil.which("perl") is None:
        sys.stderr.write("NOTE: no perl on PATH - using the Python result unchecked (#1240)\n")
        return False
    return True


_perlIsPOSIX = None


def PerlPath(path):
    """
    path, in the form the perl on PATH opens: the POSIX one (/c/...) for MSYS2's or Cygwin's perl, given a Windows path
    (C:/...), as a native Windows Python (MSYS2's mingw64 one) has. Whether a POSIX perl takes a drive-letter path as
    absolute varies: MSYS2's in the Windows-MSYS docker image takes it as relative, so configure.pl looked for its own
    Perl libraries under /c/Stroika/c:/Stroika/.
    """
    global _perlIsPOSIX
    if sys.platform != "win32":
        return path
    if _perlIsPOSIX is None:
        p = subprocess.run(["perl", "-e", "print $^O"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        _perlIsPOSIX = p.stdout.decode("latin-1") in ("msys", "cygwin")
    if not _perlIsPOSIX:
        return path
    p = subprocess.run(["cygpath", "--unix", path], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    converted = p.stdout.decode("latin-1").strip()
    return converted if p.returncode == 0 and converted else path


def RunPerlInScratch(perlScript, args, seedFiles):
    """
    Runs perl perlScript args with a scratch directory as its current directory, after copying each of seedFiles
    (paths relative to the current directory) into it, if present. Returns (scratchDir, exitStatus, combinedOutput).
    The caller reads the results from scratchDir, and removes it.
    """
    scratch = tempfile.mkdtemp(prefix="stroika-perl-check-")
    for rel in seedFiles:
        if os.path.exists(rel):
            dst = os.path.join(scratch, rel)
            os.makedirs(os.path.dirname(dst) or ".", exist_ok=True)
            shutil.copyfile(rel, dst)
    p = subprocess.run(["perl", PerlPath(perlScript)] + list(args), cwd=scratch, stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT)
    return scratch, p.returncode, p.stdout.decode("latin-1")


def Choose(scriptName, what, label, pythonBytes, perlBytes, same=None):
    """
    The bytes to write for one output file: pythonBytes if they match perlBytes (by same (a, b) if given, else exactly) -
    otherwise perlBytes, after a loud warning on stderr and the diff kept under MISMATCH_DIR. perlBytes None means no Perl
    result to check against: pythonBytes, unchecked.
    """
    if perlBytes is None or (same(pythonBytes, perlBytes) if same else pythonBytes == perlBytes):
        return pythonBytes
    os.makedirs(MISMATCH_DIR, exist_ok=True)
    diffPath = os.path.join(MISMATCH_DIR, "%s-%s-%s.diff" % (scriptName, label, os.path.basename(what)))
    diff = difflib.unified_diff(perlBytes.decode("latin-1").splitlines(True), pythonBytes.decode("latin-1").splitlines(True),
                                fromfile="perl: " + what, tofile="python: " + what)
    with open(diffPath, "w", encoding="latin-1", newline="") as f:
        f.writelines(diff)
    sys.stderr.write("\n" + "*" * 120 + "\n")
    sys.stderr.write("WARNING: %s: %s %s: %s differs - USING THE PERL RESULT. Diff: %s\n" % (MISMATCH_TAG, scriptName, label, what, diffPath))
    sys.stderr.write("         Please report it (https://github.com/SophistSolutions/Stroika/issues/1240).\n")
    sys.stderr.write("*" * 120 + "\n\n")
    return perlBytes


def Decline(scriptName, label, why):
    """For an input the Python does not reproduce exactly: say so - the caller then uses the Perl result"""
    sys.stderr.write("NOTE: %s %s: the Python cannot reproduce the Perl here (%s) - using the Perl result (#1240)\n" % (scriptName, label, why))
