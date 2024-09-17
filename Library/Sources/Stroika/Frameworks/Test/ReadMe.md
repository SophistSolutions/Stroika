# Stroika::[Frameworks](../)::Service

This Folder contains the [Frameworks](../)::Test Framework source code.

## Overview

Before Stroika v3, we used our own private test framework code, but embedded in the 'Tests' folder, and not
re-usable outside of Stroika.

There are many popular test frameworks out there for C++, and none seemed particularly good or bad. But I didn't have
much to offer over those, so I just decided to build on top of google-test. No good reasons.

This test framework code - does however provide a few useful classes/infrastructure, at least for container testing (archtype classes).

## Modules

- [ArchtypeClasses.h](ArchtypeClasses.h)
- [TestHarness.h](TestHarness.h)
