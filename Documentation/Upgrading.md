# Upgrading from one verison of Stroika to another

Stroika makes lots of changes in its APIs over time. This the only way to improve. This is imposes a cost on developers using Stroika, but also provides significant benefits (better deisgn, better features)

To mitigate the downsides, Stroika generally follows this pattern:

- Within a major-minor-stage of development (e.g. 2.1d), Stroika will endeavor to backwardly support all previous APIs through classes or methods marked as deprecated.
- those deprecation warnings, generally document what API to use INSTEAD
- breaking changes are generally called out as such in the release notes
- Between Major/Minor stages, Stroika may (will typically) change its dependent version of C++ (e.g. v2.0 requires C++14 or later, and v2.1 requires C++17 or later)

## Recommended upgrade process

Thus the obvious upgrade process from using one verison of Stroika to another would be:

- For each major-minor-stage version of Stroika between the one you currently use, and the target version (you wish to upgrade to)

  - Upgrade to the last version of that stage
  - Address any warnings about use of deprecated features
  - IF NEEDED, review release notes for that range of releases (hopefully such review will be rare)

- Upgrading within a major-minor-stage (typically about a year of development) should be relatively simple, just following instructions in deprecation warnings (with occasional bigger challenges when APIs change alot or something subtle slips through the cracks)

### Example

So for exmaple, if upgrading from use of 2.0b23 to v2.1 (final release)

- Upgrade to v2.0b7
- Upgrade to v2.1d27
- Upgrade to v2.1a5
- Upgrade to v2.1b15
- Upgrade to v2.1

Each stage of upgrade should come with fairly clear instructions (deprecation warnings) about what API changes to make, or just recompile with no problems.

## Writing code that works with multiple versions of Stroika

~~~c++
#if kStroika_Version_FullVersion <= Stroika_Make_FULL_VERSION (2, 1, kStroika_Version_Stage_ReleaseCandidate, 2, 1)
  old_way();
#else
  new_way();
#endif
~~~