@setlocal
@echo off
time/t
del /Q /F Dictionaries\Dictionary-Compiled-US-English.inc
MungeDictionaries\Release\MungeDictionaries.exe Dictionaries\DictionarySource-US-English.xml -v -s -c Dictionaries\Dictionary-Compiled-US-English.inc
time/t
@endlocal
