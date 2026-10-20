# Stroika Release Process

Rough steps I go through do do each release (rough draft)

## Release notes

Built purely from the commits since the last release (its tag - the same as `v3-Release...HEAD` while v3-Release is that release):

```bash
git log --reverse --date=short --format='=== %h %ad %s%n%b' 3.0d24..HEAD > a
```

Then summarize them into Release-Notes.md, laid out as the previous release's section is:

- The header says `### 3.0dNNx {DATEx} {[diff](../../compare/3.0dMM...3.0dNN)}` - the x's stay until the release is tagged.
- **Upgrade Notes** come from the commits' `UPGRADE NOTE:` blocks (`UpgradeNotes:` in older ones) - grep them out of `a`, group
  them by area, and merge duplicates. Then check each against the FINAL tree, not the commit: a later commit can restore, move or
  rename what an earlier note says is gone (in 3.0d25: sanitizer macros removed, then restored as deprecated; lock helpers moved
  to another header; "writing needs an XML provider", undone a week later).
- Then look for upgrade impact that no note mentions:
    - removed or renamed headers - `git diff --name-status 3.0d24 HEAD -- Library/Sources | grep '^[DR]'`
    - the `qCompilerAndStdLib_*_Buggy` defines in `Common/Private/CompilerAndStdLib_.h` at the tag against now
    - changed serialized names (an ObjectVariantMapper's `{"Name"sv, &T::fField}`) - data saved by the old version reads differently
    - a field added in the MIDDLE of a struct (positional initialization shifts; one added at the end breaks nothing)
- **TLDR** and **Change Details** come from the commit subjects, grouped by area. Skip todo/cosmetic commits, and a fix
  plus its follow-up fixes is one line.
- **Release-Validation** comes from `Documentation/SupportedPlatformsAndCompilers.md`, the docker images' Visual Studio versions,
  and - once they finish - the regression runs (its known issues).

## Update STROIKA\-VERSION file

Just delete the 'x' in the version#, and save/checkin/commit.

## Docker tag the latest containers, and push

Rebuild and publish latest containers using the github action 'build-v3-Docker-Containers' (and blow away and reload docker containers after build on local systems doing builds or rebuild locally).

## Do most builds on medusa-windows-dev VM

see <Regression-Tests.md> for steps to run most regression tests

(more notes to add here)
(some must be run on non-dev vm - )
run all builds on medusa-windows-dev except:
    >   vs2k22 builds
    >   docker/wsl windows builds
(those get run on my laptop)

## Review all the build logs

If anything looks iffy, fix before release (repeat much of above). But when all tests pass adequately (or documneted issues in release notes) - then checkin/commit the release note files (see instructions in <Regression-Tests.md>)


## Git tag and push

## Update STROIKA\_-_VERSION file

Change to next version, including the suffix 'x' on version name.


## Checkin results

```bash
VER=`cat STROIKA_VERSION` sh -c 'mv Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-*$VER*.txt Tests/HistoricalPerformanceRegressionTestResults/3.0/ && git add Tests/HistoricalPerformanceRegressionTestResults/3.0/PerformanceDump-*$VER.txt'
VER=`cat STROIKA_VERSION` sh -c 'mv Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/HistoricalRegressionTestResults/3.0/ && git add Tests/HistoricalRegressionTestResults/3.0/REGRESSION-TESTS-*-$VER-OUT.txt'
```


## github Draft Release

For late cycle releases (relase candidates or final or point releases) - do a github 'draft release' so easier for those versions to get downloaded.
