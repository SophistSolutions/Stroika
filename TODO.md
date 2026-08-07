# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- OrderBy () - remaining work. Measurements: run 'Test52 --show --orderby-probe' (Release, N=1000).

    1. Sequence<T>::OrderBy () has no SequencePolicy parameter, and since it HIDES
       Iterable<T>::OrderBy () (no 'using inherited::OrderBy') a policy cannot be requested on a
       Sequence at all. LGP: planned. Decide the default deliberately when adding it - Iterable's is
       now eSeq, and matching that keeps the two consistent.
    2. OrderBy () always returns a Sequence_stdvector<T>-backed result, unlike Where ()/Map () which
       CloneEmpty () to retain the rep type. Probably fine/desirable for a sort, but undocumented.
    3. STK-972 ("optimize case where 'iterable' is already sortable") is still open on
       Iterable<T>::OrderBy ().
    - NOTE the in-line @todo at the top of Sequence<T>::OrderBy () is misleading: better iterators
      cannot avoid the copy. stable_sort needs std::sortable/permutable, and Stroika iterators are
      read-only by design (Iterator::operator* and RandomAccessIterator::operator[] both return
      const T&) - deliberately, since handing out T& would break COW (see the operator[] note in
      Sequence.h). --LGP still to double-check this claim.
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
      some larger N, so 'eSeq always' may be the wrong default for big sequences. A size sweep would
      settle it.
    - Do NOT pre-size a copy via MakeRandomAccessIterator () unconditionally: Sequence_LinkedList and
      Sequence_DoublyLinkedList still return _MakeRandomAccessIterator_ViaGetAt () for random access
      (the doubly-linked one has native *bidirectional* only), so a vector range CTOR over it goes
      O(n^2) there. Also: reserve () turned out to LOSE for this - see the comment on the disabled
      branch in Iterable<T>::As<> () for the measurements and why.
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

- ask if anything else reasonable todo on bidi iterator support or at least if this is good breaking point.
- test HearHE
- deal with failed/lost bugs from JIRA
