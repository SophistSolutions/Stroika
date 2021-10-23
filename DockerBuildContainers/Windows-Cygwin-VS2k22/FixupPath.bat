@echo off
echo Applying hack to fix case of PATH (perl code assumes all uppercase and somehow docker creates mixed case)
set P=%PATH%
set PATH=
set PATH=%P%
set P=
