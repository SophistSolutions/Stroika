setlocal
rm -rf DocsOut
mkdir DocsOut
cd DocsOut
..\Release\AutoDoc -date 2002-09-04 ..\..\..\Led\Headers\*.h ..\..\..\Led\Sources\*.cpp
