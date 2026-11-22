# ArchiveUtility sample

ArchiveUtility is a simple command-line tool to create/update/list/etc archives, like zip files, or 7z files. This demonstrates use of the Stroika [DataExchange/Archive](../../Library/Sources/Stroika/Foundation/DataExchange/Archive/ReadMe.md) classes

[TestData](TestData) holds a small zip and 7z archive to try it on - which the release regression runs list (Build/Scripts/RegressionTests):

    ArchiveUtility --list Samples/ArchiveUtility/TestData/Sample.7z