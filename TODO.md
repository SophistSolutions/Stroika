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
      O(n^2) there. Also: reserve () turned out to LOSE for this - see the comment on the disabled
      branch in Iterable<T>::As<> () for the measurements and why.
- PeekContiguousStorage - STAGE 1 DONE (read-only side). Iterable<T>::_IRep::PeekContiguousStorage ()
  exists (defaults to nullopt), Sequence_Array and Sequence_stdvector override it, and
  Iterable<T>::As<vector<T>> () uses it. Measured (Release, N=1000, 'Test52 --show'):
  As<vector<int>> on a contiguous backend went 170 -> 1.07 vs a plain vector copy, and
  Sequence<int>::OrderBy () fell 2.58 -> 1.02 vs std::stable_sort as a consequence.
  Remaining, roughly in priority order:
    1. Measure a NON-TRIVIAL T (eg Sequence_Array<String>::As<vector<String>>) before spreading this.
       The 170x was memcpy-vs-virtual-iteration on a 4-byte type; where the per-element copy dominates
       the win should shrink a lot (cf the OrderBy probe, where type erasure cost 3.4x for int but
       ~1.0x for String). That number decides whether item 2 is worth doing at all.
    2. The other 8 contiguous backends: Set_Array, Collection_Array, Mapping_Array, Association_Array,
       KeyedCollection_Array, MultiSet_Array, Queue_Array, DenseDataHyperRectangle_Vector. Overrides are
       ~5 lines and mechanical, BUT: check DenseDataHyperRectangle_Vector's cell iteration order against
       its linear storage order, and confirm span<const KeyValuePair<K,V>> is what callers of the keyed
       ones actually want. Sequence_ChunkedArray must stay nullopt. Nothing else hides As () - Sequence
       was the only one - so these get the fast path as soon as their reps override.
    3. STAGE 2 - the mutable span<T> on Sequence<T>::_IRep behind _GetWriteableRep (), which is what
       lets OrderBy () sort backend storage in place (measured 1.85x for int, 1.13x for String - see
       DESIGN DIRECTION above). Note OrderBy () is now at ~1.02 vs raw stable_sort, so the remaining
       headroom is smaller than when that was measured; re-measure before building it.
    4. Other consumers named below (Contains/Find/IndexOf, SequentialEquals, Min/Max/Sum/Median, Top).
  Original notes, still relevant:
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

- ask if anything else reasonable todo on bidi iterator support or at least if this is good breaking point.
- test HearHE
- deal with failed/lost bugs from JIRA
