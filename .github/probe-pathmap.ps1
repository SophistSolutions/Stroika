# TEMPORARY CI PROBE - removed with the fix for 5ae837d91c's Windows failures. MSVC's -pathmap can break __FILE__ (C2001,
# C1001, C1004, or cl hangs) where cl is given a path other than the file's final one - seen on a subst drive, depending
# on the paths' lengths. Here, in the Windows job's container: a directory shaped as the failing libxml2 build's, in
# c:\Stroika (mounted from the runner's workspace) and on the container's own disk - where Windows and Conan's Python say
# each really is, and how cl compiles there. Ends the job (exit 1): its build would fail anyway.
$ErrorActionPreference = 'Continue'
$mountRoot = 'C:\Stroika'
$ownRoot = 'C:\Stroikb'
$shape = '.conan-homX\p\b\stroid5aa8c3f750fa\b'   # the failing build's .conan-home\p\b\<id>\b, as long
$placeholder = '\StroikaDeployDirectory' + ('_' * (128 - '/StroikaDeployDirectory'.Length))
$places = [ordered]@{ 'mounted' = "$mountRoot\$shape\build\libxml2"; 'own-disk' = "$ownRoot\$shape\build\libxml2" }
# CI's libxml2 compile (vs2k22 msys x86_64 Release), but for its -I, -Fo, -Fd and the source
$ciFlags = @('-DLIBXML_STATIC', '/DWIN32', '/D_WINDOWS', '-D_UNICODE', '-DUNICODE', '-D_WINDOWS', '-DNDEBUG', '-O2', '-Oi', '-Oy', '-EHsc', '-nologo', '-GR', '-Gd', '-W4', '-Zc:inline', '-FC', '-bigobj', '-Gy', '-Oy', '-O2', '-GL', '-Oi', '-GF', '-MT', '-Z7', '-GL', '-wd4701', '-wd4702', '-wd4703', '-wd4706', '-wd4789', '-std:c11', '-MT', '-experimental:deterministic')

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32.SafeHandles;
public static class ProbeFinalPath {
    [DllImport ("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern SafeFileHandle CreateFileW (string name, uint access, uint share, IntPtr sa, uint disposition, uint flags, IntPtr template_);
    [DllImport ("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern uint GetFinalPathNameByHandleW (SafeFileHandle h, StringBuilder buffer, uint size, uint flags);
    public static string Get (string path, uint flags) {
        using (SafeFileHandle h = CreateFileW (path, 0, 7, IntPtr.Zero, 3, 0x02000000, IntPtr.Zero)) {
            if (h.IsInvalid) { return "(CreateFile error " + Marshal.GetLastWin32Error () + ")"; }
            StringBuilder b = new StringBuilder (2048);
            uint n = GetFinalPathNameByHandleW (h, b, 2048, flags);
            if (n == 0 || n >= 2048) { return "(error " + Marshal.GetLastWin32Error () + ")"; }
            return b.ToString ();
        }
    }
}
'@

# cl, its output checked for errors and the names it recorded (__FILE__ literal, /Z7 debug info); 60 s, then stopped
function Invoke-Cl ($cl, [string[]] $arguments, $obj, $log) {
    Remove-Item $obj, $log -ErrorAction Ignore
    $p = Start-Process -FilePath $cl -ArgumentList ($arguments | ForEach-Object { '"' + $_ + '"' }) -NoNewWindow -PassThru -RedirectStandardOutput $log
    $null = $p.Handle
    if (-not $p.WaitForExit(60000)) { Stop-Process -Id $p.Id -Force; return 'HUNG (stopped after 60 s)' }
    $p.WaitForExit()
    $r = "rc=$($p.ExitCode)"
    if (Test-Path $obj) {
        $text = [IO.File]::ReadAllText($obj, [Text.Encoding]::GetEncoding(28591))
        $names = [regex]::Matches($text, '[ -~]{4,}?(t2?\.cp?p?|pch\.h)\b') | ForEach-Object { $_.Value } | Where-Object { $_ -notmatch '-pathmap:' } | Select-Object -Unique -First 2
        $r += '  names: ' + (($names | ForEach-Object { $_ -replace '_{20,}', '_..._' }) -join ' ; ')
    }
    $e = (Get-Content $log -ErrorAction Ignore) | Where-Object { $_ -match 'error' } | Select-Object -First 1
    if ($e) { $r += '  ' + ($e -replace '^.*\): ', '') }
    return $r
}

"== PROBE: where each place really is"
foreach ($k in $places.Keys) {
    $d = $places[$k]
    New-Item -ItemType Directory -Force "$d\src" | Out-Null
    Set-Content -Encoding ascii "$d\src\t.c" 'void f (const char*, int); void g (void) { f (__FILE__, __LINE__); }'
    Set-Content -Encoding ascii "$d\src\t2.c" 'int g2 (int x) { return x + 1; }'
    Set-Content -Encoding ascii "$d\src\t.cpp" 'void f (const char*, int); void g () { f (__FILE__, __LINE__); }'
    Set-Content -Encoding ascii "$d\src\pch.h" 'inline const char* Where () { return __FILE__; }'
    Set-Content -Encoding ascii "$d\src\pch.cpp" '#include "pch.h"'
    "{0,-8} {1}" -f $k, $d
    "         final path, DOS:  {0}" -f [ProbeFinalPath]::Get($d, 0)
    "         final path, GUID: {0}" -f [ProbeFinalPath]::Get($d, 1)
    "         final path, NT:   {0}" -f [ProbeFinalPath]::Get($d, 2)
}
"$mountRoot's attributes: $((Get-Item $mountRoot).Attributes)"
fsutil reparsepoint query $mountRoot 2>&1 | Select-Object -First 3 | ForEach-Object { "  fsutil: $_" }

"== PROBE: Conan's Python"
$env:CONAN_HOME = "$ownRoot\probe-conan-home"
New-Item -ItemType Directory -Force "$ownRoot\probe-conan" | Out-Null
Set-Content -Encoding ascii -Path "$ownRoot\probe-conan\conanfile.py" -Value @"
import os, sys
for p in (r"$($places['mounted'])", r"$($places['own-disk'])"):
    r = os.path.realpath(p)
    print("PROBE python %s: realpath(%s) = %s - a directory: %s" % (sys.version.split()[0], p, r, os.path.isdir(r)))
from conan import ConanFile
class Probe(ConanFile):
    name = "probe"
"@
conan inspect "$ownRoot\probe-conan" 2>&1 | ForEach-Object { "$_" } | Where-Object { $_ -match 'PROBE|rror' }

"== PROBE: compiles (60 s each)"
$compilers = @(Get-ChildItem -Path 'C:\Program Files\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\*\cl.exe', 'C:\Program Files (x86)\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\*\cl.exe' -ErrorAction Ignore)
foreach ($cl in $compilers) {
    $ver = ($cl.FullName -split '\\MSVC\\')[1].Split('\')[0]
    $arch = $cl.Directory.Name
    foreach ($k in $places.Keys) {
        $d = $places[$k]
        $install = ($d -replace '\\build\\libxml2$', '\install') -replace '^C:', 'c:'
        $map = "-pathmap:$d=$placeholder\src\libxml2"
        $common = @('-nologo', '-c', '-FC', '-Z7', '-experimental:deterministic')
        $tag = "{0,-8} {1} {2,-3}" -f $k, $ver, $arch
        "$tag C, __FILE__, no -pathmap:   " + (Invoke-Cl $cl.FullName ($common + @('-std:c11', "$d\src\t.c", "-Fo$d\t.obj")) "$d\t.obj" "$d\cl.out")
        "$tag C, __FILE__:                " + (Invoke-Cl $cl.FullName ($common + @('-std:c11', $map, "$d\src\t.c", "-Fo$d\t.obj")) "$d\t.obj" "$d\cl.out")
        "$tag C, __FILE__, CI's flags:    " + (Invoke-Cl $cl.FullName ($ciFlags + @($map, "-pathmap:$install=$placeholder", '/FS', '-c', "$d\src\t.c", "-Fo$d\t.obj")) "$d\t.obj" "$d\cl.out")
        "$tag C, no __FILE__:             " + (Invoke-Cl $cl.FullName ($common + @('-std:c11', $map, "$d\src\t2.c", "-Fo$d\t2.obj")) "$d\t2.obj" "$d\cl.out")
        "$tag C++, __FILE__:              " + (Invoke-Cl $cl.FullName ($common + @('-EHsc', $map, "$d\src\t.cpp", "-Fo$d\tpp.obj")) "$d\tpp.obj" "$d\cl.out")
        "$tag C++ pch, __FILE__:          " + (Invoke-Cl $cl.FullName ($common + @('-EHsc', '-Ycpch.h', "-Fp$d\pch.pch", $map, "$d\src\pch.cpp", "-Fo$d\pch.obj")) "$d\pch.obj" "$d\cl.out")
        $final = [ProbeFinalPath]::Get($d, 0) -replace '^\\\\\?\\', ''
        if ($final -ne $d -and (Test-Path $final)) {
            "$tag C, __FILE__, CI's flags, by its final path $($final): " + (Invoke-Cl $cl.FullName ($ciFlags + @("-pathmap:$final=$placeholder\src\libxml2", '/FS', '-c', "$final\src\t.c", "-Fo$d\t.obj")) "$d\t.obj" "$d\cl.out")
        }
    }
}
"== PROBE done: this job stops here (temporary)"
exit 1
