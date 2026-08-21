# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- AppendAll () batching, the parts NOT done in c384915a32. That commit made Sequence<T>::AppendAll ()
  hand a CONTIGUOUS source to _IRep::Insert () as one span (int construction 0.171s -> 0.0042s, ~40x;
  Sequence<String> now costs exactly a vector<String> copy). Still element-at-a-time:
    - a STROIKA source. Iterator<T> is not a contiguous_iterator, so Sequence{someIterable} and
      As<Sequence<T>> () still pay one virtual dispatch per element. Wants PeekContiguousStorage (),
      the same hook Iterable<T>::As () already uses.
    - a non-contiguous source (list, generator). Could chunk through a small stack buffer - 256
      elements per Insert () call is still ~256x fewer dispatches than now.
    - the OTHER containers. Set/MultiSet/KeyedCollection/... still have the per-element AddAll ()
      shape. COLLECTION IS DONE: _IRep::Add () takes span<const value_type> (incompatible change to
      the rep interface rather than a second virtual, so no extra vtable slot), all 6 backends
      converted, contiguous fast path in AddAll (). Read that before doing Set/MultiSet, because the
      interesting result is that batching only pays where the BACKEND has a bulk op. Measured by the
      Tests/52 probes that feed AddAll () the same range from a std::list (per-element branch) versus a
      vector (span branch), which isolates the batching and nothing else: Collection_Array 5.93ns ->
      0.19ns per element (~30x), but Collection<int> (sorted multiset) only 0.508s -> 0.482s (5%),
      because tree insertion dominates. So check the data structure for a bulk insert FIRST; without
      one, batching buys only the dispatch and the change-count bump.
      NB the DEFAULT Collection<T> for an ordered T is the sorted multiset, so the default gets the 5%.
  Guarded by the Tests/52 entry "Build Sequence<int> from vector<int>" (threshold 3.3): if the fast
  path stops firing that score returns to ~60.

- ANSWERED, do not re-litigate: SortedCollection_stdmultiset IS the right default for Collection<T>
  when T is totally_ordered. Measured 3.0d24, Windows x86_64 release, 500 elements, probes now in
  Tests/52 ("Contains () each PRESENT/ABSENT, Collection_Array<...> vs sorted default"):
      Contains () hit   sorted ~10x faster than Collection_Array's linear scan, int AND String
      Contains () miss  sorted 23x (int) / 34x (String) faster
  The guess that a linear scan over 500 contiguous ints would beat the tree was WRONG - it loses at
  both element types. Arithmetic that settles it: the sorted rep costs ~21us MORE to build 500
  elements, and saves ~130us per 500-lookup sweep, so ONE lookup pass repays the whole construction
  penalty ~6x. Only a Collection that is essentially never searched would prefer the array - which is
  what fHints_OptimizeForLookupSpeedOverUpdateSpeed is for, and that branch now honors it.
  Related, also measured, also no change wanted: for non-ordered T the "optimize for updates" branch
  picks Collection_LinkedList, and that is right for per-element adds (1.9x faster than
  Collection_Array<String> - one node alloc versus push_back with realloc and String moves) even
  though Collection_Array is 2.9x faster for a batched AddAll (). Workload-dependent, not backwards.

- Perf thresholds could be ~2x tighter once there is PINNED history to calibrate from. The 3.0d24 pass
  used 1.20x margin against a 7% unpinned noise floor; pinned (which
  Build/Scripts/RunPerformanceRegressionTests now does) the floor is 2.9%, so ~1.10 would catch roughly
  half-size regressions. Needs a few pinned releases in the archive first.

- Do NOT pre-size a copy via MakeRandomAccessIterator () unconditionally: Sequence_LinkedList and
  Sequence_DoublyLinkedList still return _MakeRandomAccessIterator_ViaGetAt () for random access
  (the doubly-linked one has native *bidirectional* only), so a vector range CTOR over it goes
  O(n^2) there. Also: reserve () measured 1.33x-1.55x SLOWER than just letting the range CTOR
  size the target itself, so it was not adopted (and that note is no longer in Iterable<T>::As<> ()
  - a negative result does not need a causal story attached to it). Do not reintroduce reserve ()
  without new evidence; the size () item below is the thing that would change the picture.
  
- Iterable<T>::PeekSize () -> optional<size_t> - a way to ask "do you know your size cheaply?".
  Not built. Discussed at length 2026-08-13/14; recording the conclusions so it is not re-derived.
    - The DataStructure/container half of this IS done: both linked lists now cache their length, so
      every DataStructures class and therefore every Stroika container has O(1) size (). Documented in
      Containers/DataStructures/ReadMe.md, and the counterpart note on Iterable<T>::size () says why
      the guarantee stops there.
    - It CANNOT be pushed up to Iterable<T>::size (). An Iterable may be a generator, or a lazy
      pipeline - Where () returning Iterable<T> is lazy (Iterable.inl ~L556), so its count is not
      knowable without running the predicate over every element, and running it may not be repeatable
      (socket, file). Not an implementation gap; caching cannot cache what was never computed.
    - REJECTED: returning numeric_limits<size_t>::max () for "unknown". It makes an unknown look like
      a number, and every existing caller does arithmetic on size (): Median () computes size ()/2,
      reserve (size ()) throws, size () - 1 wraps, 'i < size ()' becomes an infinite loop. Fails
      silently and late. It also collapses three different states - known-and-cheap, knowable-but-
      expensive, unbounded - into one value.
    - REJECTED: overloading the existing virtual (size (SizeQuery) with a sentinel) to avoid adding a
      second one. There are 41 in-tree overrides of 'size () const override' plus out-of-tree backends,
      all of which would have to change, to save one vtable slot. Also default args on virtuals bind to
      the STATIC type, which is a trap.
    - THE SHAPE TO BUILD, if it is ever wanted: NOT a virtual at all. "Is my size () cheap?" is a
      per-TYPE constant, so it needs no dynamic dispatch - put a protected bool on Iterable<T>::_IRep
      (default false = "I make no promise"), set by backends that guarantee it, and make PeekSize ()
      a NON-virtual on Iterable<T> that reads it. Zero vtable slots (which was LGP's objection to a
      PeekSize () virtual - a virtual's body cannot be stripped by the linker, and _IRep is a template
      so it multiplies by every T), one bool per rep object, and none of the 41 overriders touched.
      Weakness: an unchecked promise - a backend could set it and lie.
    - WHAT WOULD JUSTIFY BUILDING IT: putting a real heuristic inside the no-policy overloads - the
      "@todo measure the crossover and auto-choose the policy here - eSeq is a placeholder, not a
      decision" notes in Iterable.inl / Sequence.inl. Choosing a policy by size needs a CHEAP size, and
      nullopt has an obvious right answer there (use eSeq). Until something concrete needs it, do not
      add it.

- test HearHE
