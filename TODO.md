# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- OrderBy () - remaining work. Measurements: run 'Test52 --show --orderby-probe' (Release, N=1000).

    1. STK-972 ("optimize case where 'iterable' is already sortable") is still open on
       Iterable<T>::OrderBy ().
    2. The ePar-vs-eSeq crossover is unmeasured. Parallel should win at some larger N, so the eSeq
       default (now on both Sequence and Iterable) may be wrong for big sequences - a size sweep would
       settle it. At N=1000 ePar costs 2.08x on Sequence and 1.81x on Iterable, whose larger copy
       dilutes the sort's share.
       (The in-place-sort design direction that used to be recorded here is decided and closed - see
       the DESIGN NOTE on Sequence<T>::_IRep in Sequence.h for the measurements and the reasoning.)

       Change default to NOT be a value, but algorithm (overload) that picks. Say which you want. Or get the best default (ask Claude if makes sense? Probaly yes once we gaurnatee size() is O(1) - and DOCUMENT that ratioale - WHY we guarnatee size O(1))
    - Do NOT pre-size a copy via MakeRandomAccessIterator () unconditionally: Sequence_LinkedList and
      Sequence_DoublyLinkedList still return _MakeRandomAccessIterator_ViaGetAt () for random access
      (the doubly-linked one has native *bidirectional* only), so a vector range CTOR over it goes
      O(n^2) there. Also: reserve () measured 1.33x-1.55x SLOWER than just letting the range CTOR
      size the target itself, so it was not adopted (and that note is no longer in Iterable<T>::As<> ()
      - a negative result does not need a causal story attached to it). Do not reintroduce reserve ()
      without new evidence; the size () item below is the thing that would change the picture.
- LinkedList::size () / DoublyLinkedList::size () are O(n) - cache the length instead. Both walk from
  fHead_ counting. Every other DataStructure in the stack is already O(1): Array and SkipList keep
  fLength_, HashTable keeps fCachedSize_, STLContainerWrapper inherits the wrapped container's. And
  Sequence_DoublyLinkedList.inl:40 already says so - "// NOTE: O(N), but could easily be made faster
  caching the length".
    - Cost is one size_t per list OBJECT (not per node), plus an increment/decrement in the mutators.
    - The tradeoff that made std::list's O(1) size () contentious - ranged splice degrading to O(n) -
      does NOT apply here: neither class has any splice/steal/relink-from-another-list operation.
      (For reference, the standard requires O(1) size () on every container that has one, including
      set/map and the unordered_* family; forward_list is the lone holdout and resolves the tension
      by having no size () at all rather than a slow one.)
    - Worth more than tidiness: size () is what Nth (), Top (n), Median () and any prospective
      pre-sizing path consult. Making it O(1) is the precondition that would make the reserve ()
      question noted under OrderBy () above worth re-testing for linked-list backends - ie it is what
      would generate the evidence we currently do not have.
      (document/mentioned above - use this in various optimizations - like ePar)
- Test52's permanent 'Sequence_Array<int>::As<vector<int>> () vs plain vector copy' entry WILL flap,
  and the reason is measurement CONTEXT, not machine load:
      run standalone ('Test52 --show'):   0.76, 1.06, 1.09, 1.11, 1.19, 1.25, 1.40, 1.64
      run in-suite ('make run-tests'):    1.69, 1.77
  ie ~2x higher in the run that actually gates. Test52 runs after 30+ other test binaries in one
  'make run-tests' invocation, so heap/cache state is nothing like a fresh process. Its threshold (1.5)
  was tuned from standalone numbers - the code comment claims 0.97-1.07 over 4 runs and calls 1.5
  "loose enough not to flap" - so it is calibrated against the wrong distribution. Both sides are only
  ~15-25ms, which makes it that much easier to move.
  Fix by raising the run count until in-suite readings stabilize, or by widening the threshold to cover
  the in-suite range. Do NOT re-tune it from a standalone run.

- Build system error handling - same class as the two fixes that landed (c02ebaa1c6, 9caa6f69f9),
  still open:
    - Build/Shared/Skel-Templates/{Basic,HTMLUI}/Makefile: the generated app's all: loop over
      configurations has no '|| exit $$?', so a new Stroika app's 'make all' walks past a
      configuration that failed to BUILD and still exits 0. The root Makefile does it correctly for
      the same target, which suggests oversight - but fixing it changes behaviour for every
      Skel-generated app, so LGP's call.
    - Tests/Makefile ~191: 'tr -d "\r\n"' collapses each test's whole output onto ONE line, which is
      what makes run-tests logs so hard to read and grep. The comment there already says it is "not
      obvious we want the tr -d".
    - Tests/Makefile: [ "$$TEST_FAILURES_CAUSE_FAILED_MAKE" -ne 0 ] errors and then silently takes
      the suppress-failures branch if that variable is set but EMPTY. AGENTS.md documents setting it
      to 0, so empty is a plausible slip.
- CMAKE CONVERSION - scheduled for NEXT release (~Sept 2026), still tentative.
  DRIVER: LGP wants Stroika to be cheap for OTHER PEOPLE to consume. That is the whole argument -
  find_package/FetchContent/vcpkg/Conan, ie the normal ways a C++ project takes a dependency. Today,
  using Stroika means adopting Stroika's build system, and no amount of patching the hand-rolled make
  changes that. Decide this on THAT basis, not on any individual build bug.
  Secondary wins: header dependency tracking for free (see the DECIDED note below), native MSVC builds with no
  MSYS/Cygwin involved, IDE project generation and CTest, and the removal of a whole class of
  hand-rolled-build bug - the swallowed exit statuses and missing failure propagation fixed in
  c02ebaa1c6 / 9caa6f69f9 exist BECAUSE the build is bespoke.
  SCOPE - weeks, and most of it is NOT the compile rules. What has to be re-expressed:
    - the named-configuration system (ConfigurationFiles/*.xml + the perl 'configure' + config tags +
      per-feature flags) -> CMakePresets/cache vars. NB CMakePresets is a clumsier fit than what we
      have now; the tags model is genuinely good and some expressiveness will be lost.
    - ThirdPartyComponents' fetch/patch/build of boost/openssl/curl/xerces/mongo-cxx ->
      FetchContent/ExternalProject, per component. boost and openssl are the usual pain.
    - Skel + Skel-Templates - this is PUBLIC API for downstream apps, so every Skel-generated app
      breaks and needs a CMake equivalent.
    - HTMLViewCompiler/.swsp custom build steps; the REMOTE=/valgrind test paths; the docker build
      VMs; CI.
  DE-RISK FIRST: a CMake build covering ONLY the Foundation library + the test suite - no third-party
  fetching, no Skel, no configure - is a few days and answers most of the open questions (does the
  model fit, where is the friction, what will ThirdPartyComponents really cost) without committing to
  the whole thing.
  DECIDED 2026-08-12: do NOT do .d/header-dependency work in the hand-rolled build first. It would be
  ~1 day and 100% thrown away by this conversion. Live with the limitation until then: there is NO
  header dependency tracking (an object depends on its .cpp and nothing else), so editing a header
  rebuilds nothing and only 'library-clobber' gives a build worth trusting - AGENTS.md now says so
  (89a38cf2a2). A run-tests staleness warning was considered and rejected: it would cover tests only,
  not samples or any other use.
- ask if anything else reasonable todo on bidi iterator support or at least if this is good breaking point.
- test HearHE
- deal with failed/lost bugs from JIRA
- do a performance compare with checked in data

- functinal _movable_function etc  winging on internet

- issue of LOST JIRA TICKETS

  - for items where we have a default paraemter of eSeq or ePar, instead OVERLOAD and have
    unspecified version documented to make a good guess which to use, and leave ambiguous. Be specific if
    you care. Overload will probably generally look at s.size() - NOT fully considered but trial balloon plan.


  + Consider adding Mapping_stdflatmap? Is there such a thing? Maybe fast for small sizes?