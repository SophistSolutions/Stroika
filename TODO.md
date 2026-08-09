# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- OrderBy () - remaining work. Measurements: run 'Test52 --show --orderby-probe' (Release, N=1000).

    1. OrderBy () always returns a Sequence_stdvector<T>-backed result, unlike Where ()/Map () which
       CloneEmpty () to retain the rep type. Probably fine/desirable for a sort, but undocumented.
    2. STK-972 ("optimize case where 'iterable' is already sortable") is still open on
       Iterable<T>::OrderBy ().
    - DESIGN DIRECTION (LGP): make OrderBy a virtual on Sequence<T>::_IRep so Sequence_Array /
      Sequence_stdvector can sort their own storage in place. The blocking question was whether the
      comparer's type erasure (virtuals can't be templates) costs more than the copy it saves.
      MEASURED - and it does, decisively:
        * Virtual taking a type-erased function<> comparer: type-erasure tax 3.4x for int, making it
          ~1.8x SLOWER than today. Only affordable where comparison is expensive relative to the
          indirect call - for String the tax was ~1.0x (free).
        * Virtual exposing mutable storage (span<T>) with stable_sort left in the outer template
          (comparer stays inlined): 1.85x faster than today for int, 1.13x for String. Never
          regressed in any case measured. => this is the shape to build.
      So: storage-exposing virtual + inlined comparer, NOT a comparer-taking virtual. A type-erased
      virtual is still defensible as a *fallback* hook for backends with a genuinely different
      strategy (eg DoublyLinkedList merge sort by relinking) - just not on the array-backed path.
      CAVEAT: N=1000 on one machine. The ePar-vs-eSeq crossover is unmeasured - parallel should win at
      some larger N, so the eSeq default (now on both Sequence and Iterable) may be wrong for big
      sequences. A size sweep would settle it. At N=1000 ePar costs 2.08x on Sequence, 1.81x on
      Iterable, whose larger copy dilutes the sort's share.
    - Do NOT pre-size a copy via MakeRandomAccessIterator () unconditionally: Sequence_LinkedList and
      Sequence_DoublyLinkedList still return _MakeRandomAccessIterator_ViaGetAt () for random access
      (the doubly-linked one has native *bidirectional* only), so a vector range CTOR over it goes
      O(n^2) there. Also: reserve () measured 1.33x-1.55x SLOWER than just letting the range CTOR
      size the target itself, so it was not adopted (and that note is no longer in Iterable<T>::As<> ()
      - a negative result does not need a causal story attached to it). Do not reintroduce reserve ()
      without new evidence; the size () item below is the thing that would change the picture.
- PeekContiguousStorage - the READ-ONLY side is done and committed (f3f2ad7d1b, c6438b404d,
  5324d0eff0): the hook exists on Iterable<T>::_IRep defaulting to nullopt, 9 Array-backed reps
  override it, and Iterable<T>::As<> () takes the fast path for ANY target constructible from a
  pointer pair. Measured (Release, N=1000, 'Test52 --show'): As<vector<int>> over a contiguous backend
  went 170 -> ~1.1 vs a plain vector copy, and Sequence<int>::OrderBy () fell 2.58 -> ~0.95 vs
  std::stable_sort as a consequence. Still open, in priority order:
    1. Measure a NON-TRIVIAL T (eg Sequence_Array<String>::As<vector<String>>) BEFORE doing item 2.
       The 170x was memcpy-vs-virtual-iteration on a 4-byte type; where the per-element copy dominates
       the win should shrink a lot (cf the OrderBy probe, where type erasure cost 3.4x for int but
       ~1.0x for String). That number decides how much of item 2 is worth doing at all.
    2. The remaining consumers: SequentialEquals () first (memcmp-able for trivial T, the biggest one
       left), then Contains ()/Find ()/IndexOf (), then Min ()/Max ()/Sum ()/Median ().
    3. DenseDataHyperRectangle_Vector - the one contiguous backend still not overriding. Blocked on
       checking its cell iteration order against its linear storage order: an overrider MUST hand back
       elements in ITERATION order, so anything whose storage order differs has to stay nullopt (as
       Sequence_ChunkedArray does).
    4. STAGE 2 - the mutable span<T> on Sequence<T>::_IRep behind _GetWriteableRep (), which is what
       lets OrderBy () sort backend storage in place (measured 1.85x for int, 1.13x for String - see
       DESIGN DIRECTION above). RE-MEASURE FIRST: OrderBy () sits at ~0.95 vs raw stable_sort now, so
       the headroom is far smaller than when that 1.85x was taken.
       span<T> must NOT go on Iterable<T>::_IRep. Iterable is conceptually read-only, and handing out
       mutable storage from a const object reintroduces the COW hazard documented on
       Sequence<T>::operator[] (another thread copies the container, bumping the refcount, while you
       write through the span). _GetWriteableRep () is where sole ownership is assured.
    5. This supersedes Apply () for contiguous backends - see the note at Iterable.h ~547 explaining
       that Apply () exists to avoid per-element virtual iteration. Apply () still pays a
       std::function call per element; a span pays nothing per element.

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
- Build system error handling. One fix landed (c02ebaa1c6 - Tests/Makefile's all-configurations
  check/run-tests loops swallowed output and returned 0 on failure). Same class, still open, each
  needing a decision rather than just a fix:
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
- ask if anything else reasonable todo on bidi iterator support or at least if this is good breaking point.
- test HearHE
- deal with failed/lost bugs from JIRA
