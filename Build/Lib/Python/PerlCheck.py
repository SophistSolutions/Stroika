"""
While Stroika's configure and ApplyConfiguration move from Perl to Python
(https://github.com/SophistSolutions/Stroika/issues/1240), each Python script computes its own result, then runs the old
Perl one (kept beside it as NAME.pl) into a scratch directory and compares every file the two write. When they differ, the
PERL result is the one used - with a loud warning, and the diff kept under IntermediateFiles/configure-mismatch/ - so a
Python mistake can never change a build. But a Perl that could not run at all (perl could not open the script, or load a
file it requires) checks nothing: the Python result is used then, with a loud warning of its own - so the check cannot fail
a build the Python got right. When every release run and CI job agrees, the Perl goes, and this with it.

STROIKA_SKIP_PERL_CHECK=1 skips the Perl run (for speed); with no perl on PATH it is skipped too, and the run says so.
"""

import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile

MISMATCH_TAG = "PYTHON/PERL CONFIGURATION MISMATCH"  # what the regression-results scan looks for
NOT_CHECKED_TAG = "PYTHON/PERL CONFIGURATION NOT CHECKED"  # ... and this
MISMATCH_DIR = "IntermediateFiles/configure-mismatch"

# for each Perl run: a LANG or LC_* naming a locale that is not installed (common in a container reached over ssh) makes
# perl itself warn "Setting locale failed" on stderr at startup - which no Python run says, so every check would
# report a mismatch. This hides only that warning: perl still falls back to the "C" locale
PERL_QUIET_LOCALE_ENV = {"PERL_BADLANG": "0"}

# perl's own messages when it cannot load the script, or a file the script requires - never one of Stroika's diagnostics
_PERL_COULD_NOT_RUN = re.compile(r"^.*?(Can't open perl script .*|Can't locate \S+\.p[lm] .*|BEGIN failed--compilation aborted.*|"
                                 r"Compilation failed in require.*|Execution of .* aborted due to compilation errors.*)$", re.M)


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
    # set in this process's environment, which perl inherits - not env=, which on Windows would upper-case every name
    saved = {k: os.environ.get(k) for k in PERL_QUIET_LOCALE_ENV}
    os.environ.update(PERL_QUIET_LOCALE_ENV)
    try:
        p = subprocess.run(["perl", PerlPath(perlScript)] + list(args), cwd=scratch, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)
    finally:
        for k, v in saved.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
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


def PerlCouldNotRun(status, output):
    """
    Why the Perl could not run as a check at all - perl could not open the script, or load a file it requires - or None,
    when it ran (whatever it then reported). Such a failure is the environment's, and says nothing about the Python result.
    """
    if status == 0:
        return None
    m = _PERL_COULD_NOT_RUN.search(output)
    return m.group(1) if m else None


def NotChecked(scriptName, label, why):
    """When the Perl could not run as a check: say so, loudly - the caller then uses the Python result, unchecked"""
    sys.stderr.write("\n" + "*" * 120 + "\n")
    sys.stderr.write("WARNING: %s: %s %s: the Perl could not run (%s) - USING THE PYTHON RESULT, unchecked.\n" % (NOT_CHECKED_TAG, scriptName, label, why))
    sys.stderr.write("         Please report it (https://github.com/SophistSolutions/Stroika/issues/1240).\n")
    sys.stderr.write("*" * 120 + "\n\n")


def Decline(scriptName, label, why):
    """For an input the Python does not reproduce exactly: say so - the caller then uses the Perl result"""
    sys.stderr.write("NOTE: %s %s: the Python cannot reproduce the Perl here (%s) - using the Perl result (#1240)\n" % (scriptName, label, why))
