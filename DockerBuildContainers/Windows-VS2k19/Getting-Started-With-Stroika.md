# Steps to build

## Git clone

git clone https://github.com/SophistSolutions/Stroika.git

## Optionally enable VS2k tools (not needed for stroika)

"c:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"

## Build default configurations

make default-configurations

OR FOR NOW
    perl configure Debug-U-32 --config-tag Windows --config-tag 32 --arch x86 --apply-default-debug-flags --Xerces no
    perl configure Debug-U-64 --config-tag Windows --config-tag 64 --arch x86_64 --apply-default-debug-flags --Xerces no
