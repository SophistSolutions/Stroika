# Steps to build

## Git clone

git clone https://github.com/SophistSolutions/Stroika.git

## Optionally enable VS2k tools (not needed for stroika)

"c:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"

## Build default configurations

make default-configurations

## Make one configuration

make CONFIGURATION=Debug-U-32 all -j4

## Make all configurations

make all -j4

for now, not enuf disk space on windows docker containers, and broken resetting size
