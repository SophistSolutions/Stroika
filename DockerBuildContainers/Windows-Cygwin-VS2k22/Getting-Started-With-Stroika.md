# Steps to build

## Git clone

git clone https://github.com/SophistSolutions/Stroika.git

## Optionally enable VS2k tools (not needed for stroika)

"c:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"

## Build default configurations

make default-configurations

## Make one configuration

make CONFIGURATION=Debug all -j8

## Make all configurations

make all -j8
