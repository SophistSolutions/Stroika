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
    1. DONE - the non-trivial-T question is answered. Per-element cost BEFORE the hook vs AFTER
       ('Test52 --show --orderby-probe', Release, N=1000, same contiguous source both sides):
                                        int                 String
           As<vector<T>>          11.7 -> 0.06 ns      40.0 -> 13.0 ns
           plain walk, no copy     6.2 -> 0.13 ns      12.1 ->  0.9 ns
       The hook removes a roughly FIXED per-element cost - the virtual iteration, ~6ns/element for int
       and ~12ns for String. What is left afterwards is whatever the operation actually does per
       element: for As<vector<String>> that is the ~13ns copy (a refcount bump plus the vector's own
       growth), which is why that row improves least in RELATIVE terms while still saving the same ~27ns
       in absolute terms.
       (The old As<> paid TWO virtual walks - distance () then copy - which is why 2*12+13 ~ the 40ns
       measured. The model is consistent.)
    2. Consumers. SequentialEquals () DONE - engages when BOTH sides are contiguous, where "contiguous"
       means either an Iterable<T> whose rep offers a span OR any other contiguous_range of T (so a
       vector<T> or initializer_list<T> RHS qualifies). Per-element cost BEFORE vs AFTER, comparing two
       equal 1000-element Sequence_Arrays:
           int      13.6 -> 0.13 ns    (memcmp - the comparer is DROPPED when it is the default one,
                                        which is what licenses the library's memcmp specialization)
           String   42.4 -> 22.1 ns    (still a String compare per element either way)
       SIZING THE NEXT CONSUMER: do not predict from the "plain walk" row in item 1. Its per-element
       work was Magnitude_ (a size () call), far cheaper than a real comparison, so it flatters any
       consumer that does actual work. Predict instead from
           saved     = the iteration removed, ~6ns/element/side for int and ~12ns for String
           remaining = what the operation itself costs per element
       String equality costs ~22ns, which swamps the ~20ns saved across two sides - hence 1.9x. So
       expect ~2x, not 13x, for a non-trivial T on: Contains ()/Find ()/IndexOf (), then
       Min ()/Max ()/Sum (). Median () sorts, so it is dominated by the sort - probably not worth doing.
    2b. DROP SequentialEquals ()'s useIterableSize parameter (LGP's call - do not build a mechanism to
       make it automatic). Comparing sizes first only saves work when the lengths DIFFER and the shorter
       is a PREFIX of the longer; any other mismatch is already found at the first differing element in
       O(1), and when the lengths are equal - the common case - the size check can never pay off. So the
       entire prize is prefix-vs-longer comparisons on NON-contiguous backends (contiguous ones get it
       free from the span), which is mostly a test-suite shape, not a hot path.
       Not worth a PeekSize () virtual plus an override on every backend, which was the earlier idea.
       Supporting evidence for removing rather than just leaving it: repo-wide - Library/, Samples/,
       Tests/ - NOTHING passes true. That is structural: the bool asks the CALLER whether size () is
       cheap on both operands, which means knowing both backends, which is what Iterable exists to hide.
       It also carries a precondition the caller can silently violate (size () must not change during
       the comparison; debug builds assert it) in exchange for an optimization that usually does nothing.
       And "just always use it" is not an option either: _IRep::size () defaults to Apply ()-ing over
       every element and counting (Iterable.inl:37) with no generator rep overriding it, so every
       Where ()/Map ()/Select ()/Repeat () pipeline has O(n) size () no matter what LinkedList does -
       and on a lazy pipeline that runs the generator once, then SequentialEquals runs it AGAIN.
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
    6. The permanent 'Sequence_Array<int>::As<vector<int>> () vs plain vector copy' entry WILL flap, and
       the reason is measurement CONTEXT, not machine load:
           run standalone ('Test52 --show'):   0.76, 1.06, 1.09, 1.11, 1.19, 1.25, 1.40
           run in-suite ('make run-tests'):    1.69, 1.77
       ie ~2x higher in the run that actually gates. Test52 runs after 30+ other test binaries in one
       make run-tests invocation, so heap/cache state is nothing like a fresh process. Its threshold
       (1.5) was tuned from standalone numbers - the code comment claims 0.97-1.07 over 4 runs and calls
       1.5 "loose enough not to flap" - so it is calibrated against the wrong distribution. Both sides
       are only ~15-25ms, which makes it that much easier to move.
       Fix by raising the run count until in-suite readings stabilize, or by widening the threshold to
       cover the in-suite range. Do NOT re-tune it from a standalone run.

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
- should SequentialEquals() take 'use size' parameter? probbaly not
