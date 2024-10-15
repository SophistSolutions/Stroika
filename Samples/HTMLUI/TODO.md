- Installer
  (working - do win and unix - and someday maybe macos)
  - windows do HTML, and TEST
  install clang_rt.asan_dynamic-x86_64.dll
  - assure right service names in installer and exe

      probably best bet is to do in MAKEFILE - check and pass in -d DEFINE for what DLL to use.
      OR use heat. Maybe heat better, since this is a clonable thing.... just like html dir

- Wrap output into docker container


Stroika-Sample-HTMLUI Service
  DependOnService RPCSS
DisplayName Stroika Sample HTMLUI Service
ErrorControl 1
ImagePath "C:\Program Files\Sophist Solutions, Inc\Stroika Sample HTMLUI Service\Stroika-SampleHTMLUI-Service.exe" --Run-As-Service
ObjectName LocalSystem
Start 3
Type 16