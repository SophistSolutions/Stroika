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
    - the OTHER containers. Collection/Set/MultiSet/... have the same per-element AddAll () shape;
      worth checking whether their reps take spans too. Not looked at.
  Guarded by the Tests/52 entry "Build Sequence<int> from vector<int>" (threshold 2.3): if the fast
  path stops firing that score returns to ~60.

- Perf-suite gap: warnings can only fire on Windows x86_64 release. See
  kPrintOutIfFailsToMeetPerformanceExpectations_ in Tests/52 - it needs _MSC_VER, no assertions, block
  allocation, and sizeof (void*) == 8. So the four Linux CI jobs run the whole perf suite and throw the
  verdict away. Probably cheap to widen, and it is the biggest structural weakness left in that suite.

- Perf thresholds could be ~2x tighter once there is PINNED history to calibrate from. The 3.0d24 pass
  used 1.20x margin against a 7% unpinned noise floor; pinned (which
  Build/Scripts/RunPerformanceRegressionTests now does) the floor is 2.9%, so ~1.10 would catch roughly
  half-size regressions. Needs a few pinned releases in the archive first.

- Two AGENTS.md notes, both cost real time to rediscover:
    - clang-format version. AGENTS.md gives the VS2022 path as the example, but the tree is formatted
      with the VS2026 one (installs as "Microsoft Visual Studio\18\", clang-format 22.1.3). Running
      VS2022's 19.1.5 silently reformats ~65 unrelated files backwards. Also worth noting
      Build/Scripts/FormatCode takes a directory + filenames, so you can format just what you changed
      instead of the ~20 minute whole-tree sweep.
    - a new Foundation .cpp needs IntermediateFiles/$(CONFIGURATION)/Library/Foundation/cached-list-objs
      deleted, or it compiles but never enters the library archive - and the failure surfaces as an
      unresolved external when a TEST links, not when the library builds.

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

