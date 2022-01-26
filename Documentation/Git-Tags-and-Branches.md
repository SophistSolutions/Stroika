# **_Git Tags and Branches_**

## Major Release Versions

- v2.1\* - requires C++17 or later; stable, and in maintainance
- v2.0\* - requires C++14 or later; stable, and in maintainance

## Branches</h3>

- v2.1-Release

  Roughly once per week (to once per-month) - a release is merged from the v2.1-Dev branch. When we do a release
  we run and record all the regression tests, including performance tests. Nearly every time we do a release, all regression tests pass fully (or mostly - sometimes exceptions).

- v2.1-Dev

  The development branch may not be stable, and represents work in progress.

- v2.0-Dev

  The development branch may not be stable, and represents work in progress.

- v2.0-Release

  Roughly once per week (to once per-month) - a release is merged from the V2.0-Dev branch. When we do a release we run and record all the regression tests, including performance tests. Nearly every time we do a release, all regression tests pass fully.

- v2.0-Stable

  This is based on V2.0-Release, but lags behind by typically weeks or months, and represents a snapshot thats been used in other products
  and appears to be working well.

## Tags

Each v2.x release is taged vITSVERSION, for example, v2.0a205
