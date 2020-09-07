# Stroika Release Process

(rough draft)

## Release notes

Prepare them by doing

```bash
git log --reverse v2.1-Dev > a
```

Then summarize them into Release-Notes.md

## Update STROIKA\_-_VERSION file

Just delete the 'x' in the version#, and save/checkin/commit

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
