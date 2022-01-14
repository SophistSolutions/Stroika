# Stroika Release Process

Rough steps I go through do do each release (rough draft)

## Release notes

Prepare them by doing

```bash
git log --reverse v2.1-Dev > a
```

Then summarize them into Release-Notes.md

## Update STROIKA\_-_VERSION file

Just delete the 'x' in the version#, and save/checkin/commit.

## Docker tag the latest containers, and push

in DockerBuildContainers (both windows dev vm and Hercules)

```bash
make pull-base-images
make build-images
```

## Do most builds on hercules-dev VM

see <Regression-Tests.md> for steps to run most regression tests

(more notes to add here)
(some must be run on non-dev vm - )

## Review all the build logs

If anything looks iffy, fix before release (repeat much of above). But when all tests pass adequately (or documneted issues in release notes) - then checkin/commit the release note files (see instructions in <Regression-Tests.md>)

## Docker tag the latest containers, and push

in DockerBuildContainers (both windows dev vm and Hercules)

```bash
make tag-images
make push-images
```

## Git tag and push

## Update STROIKA\_-_VERSION file

Change to next version, including the suffix 'x' on version name.



## Checkin results

```bash
VER=2.1r1 sh -c 'mv Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-*$VER*.txt Tests/HistoricalPerformanceRegressionTestResults/2.1/ && git add Tests/HistoricalPerformanceRegressionTestResults/2.1/PerformanceDump-*$VER.txt'
VER=2.1r1 sh -c 'mv Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/HistoricalRegressionTestResults/2.1/ && git add Tests/HistoricalRegressionTestResults/2.1/REGRESSION-TESTS-*-$VER-OUT.txt'
```


## github Draft Release

For late cycle releases (relase candidates or final or point releases) - do a github 'draft release' so easier for those versions to get downloaded.