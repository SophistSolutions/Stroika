# Stroika Release Process

Rough steps I go through do do each release (rough draft)

## Release notes

Prepare them by doing

```bash
git log --reverse v3-Release...HEAD > a
```

Then summarize them into Release-Notes.md

## Update STROIKA\-VERSION file

Just delete the 'x' in the version#, and save/checkin/commit.

## Docker tag the latest containers, and push

Rebuild and publish latest containers using the github action 'build-v3-Docker-Containers' (and blow away and reload docker containers after build on local systems doing builds or rebuild locally).

## Do most builds on hercules-dev VM

see <Regression-Tests.md> for steps to run most regression tests

(more notes to add here)
(some must be run on non-dev vm - )
run all builds on hercules-dev except:
    >   vs2k22 builds
    >   docker windows builds
(those get run on my laptop)

## Review all the build logs

If anything looks iffy, fix before release (repeat much of above). But when all tests pass adequately (or documneted issues in release notes) - then checkin/commit the release note files (see instructions in <Regression-Tests.md>)


## Git tag and push

## Update STROIKA\_-_VERSION file

Change to next version, including the suffix 'x' on version name.


## Checkin results

```bash
VER=3.0d5 sh -c 'mv Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-*$VER*.txt Tests/HistoricalPerformanceRegressionTestResults/3.0/ && git add Tests/HistoricalPerformanceRegressionTestResults/3.0/PerformanceDump-*$VER.txt'
VER=3.0d5 sh -c 'mv Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/HistoricalRegressionTestResults/3.0/ && git add Tests/HistoricalRegressionTestResults/3.0/REGRESSION-TESTS-*-$VER-OUT.txt'
```


## github Draft Release

For late cycle releases (relase candidates or final or point releases) - do a github 'draft release' so easier for those versions to get downloaded.
