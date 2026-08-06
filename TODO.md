# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- OrderBy () - remaining work. Measurements come from Test52 --orderby-probe (Release/MSVC, N=1000).

  DONE so far: Iterable<T>::OrderBy () default flipped ePar -> eSeq (parallel measured 1.54-1.70x
  SLOWER than sequential at N=1000 - flip confirmed correct).

  RESOLVED: a special-cased As<vector<T>> is NOT worth it - the branch is now disabled ('and false')
  and As<vector<T>> falls through to the generic range CTOR. Measured, 4 copy strategies within ONE
  run, same Iterable<int>, N=1000 (Test52 --orderby-probe, "Copy strategy" entries), as ratios vs the
  iterator-pair baseline:
      vector{begin,end}  (the generic branch)      1.00x   <-- fastest; distance () + copy, 2 walks
      assign (begin, end)                         ~0.97x   within noise of the above (same 2 walks)
      reserve (size ()) + range-for push_back     ~1.17x   consistently slowest of the three
      [attempt 1] reserve + Apply + std::function   1.33x   worse
      [attempt 2] reserve + assign                  1.55x   worse still - pays for the length 3x
                                                            (size (), then distance (), then copies)
  Mechanism: vector's range CTOR does distance () then a copy loop with NO per-element capacity
  check. reserve+push_back pays a capacity check per element AND size () is a virtual call that is
  O(n) for a generic rep - isolating just that one line cost ~0.53x of the whole copy.
  => NOTE the earlier "add reserve ()" advice does NOT generalize. It was measured on
     Sequence_Array<int>; on Iterable<int> reserve+walk LOSES to the plain range CTOR.
  Remaining nit: the disabled branch is dead code kept for reference - either keep it clearly marked
  or drop it.

    1. THE IMPROVEMENT DOES NOT REACH Sequence<T> USERS. Sequence<T>::OrderBy () (Sequence.inl) still
       has its own body doing 'vector<T> tmp{begin (), Iterator<T>{end ()}}' - no reserve, not going
       through As<vector<T>> () - and it HIDES Iterable<T>::OrderBy (), so Sequence callers get none
       of the above. Also still no SequencePolicy parameter on it (LGP: planned), and no
       'using inherited::OrderBy', so a policy cannot be requested on a Sequence at all.
       Its in-line @todo is misleading too - see the NOTE below.
    2. OrderBy () always returns a Sequence_stdvector<T>-backed result, unlike Where ()/Map () which
       CloneEmpty () to retain the rep type. Probably fine/desirable for a sort, but undocumented.
    3. Iterable.h's new doc note cites the option as '--order-by'; it is '--orderby-probe'. Now that
       Test52 calls cmdLine.Validate (), the wrong spelling hard-errors. Also typo 'asign' in the new
       As<> comment.
    4. The new same_as<CONTAINER_OF_T, vector<T>> branch in Iterable<T>::As<> () silently ignores
       CONTAINER_OF_T_CONSTRUCTOR_ARGS... args, which both other branches forward. No caller hits it
       today, but it fails silently rather than at compile time - constrain it with
       sizeof...(args) == 0 so such a call falls through to the general branch.
    5. STK-972 ("optimize case where 'iterable' is already sortable") is still open on
       Iterable<T>::OrderBy ().
    - NOTE the in-line @todo at the top of Sequence<T>::OrderBy () is misleading: better iterators
      cannot avoid the copy. stable_sort needs std::sortable/permutable, and Stroika iterators are
      read-only by design (Iterator::operator* and RandomAccessIterator::operator[] both return
      const T&) - deliberately, since handing out T& would break COW (see the operator[] note in
      Sequence.h). --LGP still to double-check this claim.
    - Do NOT pre-size via MakeRandomAccessIterator () *unconditionally*. Sequence_Array and
      Sequence_stdvector have native random-access reps so it is O(n) for them, but
      Sequence_LinkedList and Sequence_DoublyLinkedList still return
      _MakeRandomAccessIterator_ViaGetAt () (the doubly-linked one got native *bidirectional* only),
      so vector's range CTOR would go O(n^2) there. reserve (size ()) + fill is the backend-agnostic
      version - which is what As<vector<T>> () now does. (size () is itself O(n) for LinkedList.)
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
      CAVEAT: all of the above is N=1000 on one machine. The ePar-vs-eSeq crossover in particular is
      unmeasured - parallel should win at some larger N, so 'eSeq always' may be the wrong default
      for big sequences. A size sweep would settle it.
- IDEA (LGP): add a virtual to Iterable<T>::_IRep that hands back the backend's contiguous storage as
  a span - nullopt when the backend has none - so algorithms can take a fast path with a slow
  fallback. Generalizes the storage-exposing virtual proposed for OrderBy (see above) to everything.
  Worth doing. Notes before implementing:
    - SPLIT const from mutable. span<const T> belongs on Iterable<T>::_IRep. span<T> does NOT:
      Iterable is conceptually read-only, and handing out mutable storage from a const object
      reintroduces the COW hazard documented on Sequence<T>::operator[] (another thread copies the
      container, bumping the refcount, while you write through the span). The mutable variant belongs
      on Sequence<T>::_IRep, reachable only via _GetWriteableRep () where sole ownership is assured.
    - DON'T spell it As<optional<span<T>>>. As<CONTAINER_OF_T> today means "materialize an owning
      copy you can keep"; a span is a borrowed view invalidated by the next mutation. Same spelling,
      opposite ownership semantics. Prefer a Peek-style name (eg _PeekContiguousStorage ()), and keep
      it protected/_-prefixed - the span must never escape the algorithm that asked for it.
    - Give it a default implementation returning nullopt so it is purely additive (does not break
      out-of-tree backends).
    - Payoff, roughly in order: As<vector<T>> (biggest, and now feeds OrderBy ()/Top ()/Repeat ()),
      Contains ()/Find ()/IndexOf (), SequentialEquals () (memcmp-able for trivial T),
      Min ()/Max ()/Sum ()/Median (), OrderBy ()/Top (). Supported by Sequence_Array and
      Sequence_stdvector; nullopt for tree/hash/linked-list backends.
    - This supersedes Apply () for contiguous backends - see the note at Iterable.h ~547 explaining
      that Apply () exists to avoid per-element virtual iteration. Apply () still pays a
      std::function call per element; a span pays nothing per element.
    - OPEN QUESTION: is "contiguous AND in iteration order" always true where we would return a span?
      True for Sequence_Array/Sequence_stdvector. Must be false (nullopt) for anything whose storage
      order differs from its iteration order.

- REDO Iterable<T>::Top () - the overloads WITHOUT an 'n' argument should return optional<T>, not
  Iterable<T>. Returning a whole Iterable for "the single top element" is the wrong shape, and it
  forces the current implementation to do a full O(n log n) sort to answer an O(n) question.
  Reimplement in terms of max_element (), and consider giving it an Execution::SequencePolicy
  argument the way Iterable<T>::OrderBy () has (and Sequence<T>::OrderBy () is getting).
    - Note the current state: NONE of the four Top () overloads takes a SequencePolicy, and
      Iterable.inl hardcodes 'sort (std::execution::par, ...)' / 'partial_sort (std::execution::par,
      ...)'. So Top () unconditionally pays for parallelism that OrderBy () just measured as a
      1.70x LOSS at N=1000 - with no way for a caller to opt out. Fixing the return type and the
      policy argument together makes sense.
    - Top (n, ...) keeps returning Iterable<T> - only the no-'n' overloads change.

- ask if anything else reasonable todo on bidi iterator support or at least if this is good breaking point.
- test HearHE
- deal with failed/lost bugs from JIRA
