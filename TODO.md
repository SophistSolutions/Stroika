# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- AppendAll () batching, the parts NOT done in c384915a32. That commit made Sequence<T>::AppendAll ()
  hand a CONTIGUOUS source to _IRep::Insert () as one span (int construction 0.171s -> 0.0042s, ~40x;
  Sequence<String> now costs exactly a vector<String> copy). Still element-at-a-time:
    - a STROIKA source: DONE (b5b811058b) - asks the source rep for PeekContiguousStorage (), which
      already existed and simply was not being asked. int 93.7x, String 15.0x, and a Stroika source now
      scores BELOW 1.0 against a vector one. Same commit made InsertAll () stop being O (m*n).
    - a non-contiguous source (list, generator). Could chunk through a small stack buffer - 256
      elements per Insert () call is still ~256x fewer dispatches than now. CAVEAT, not yet measured:
      chunking forces source->buffer->container, ie TWO copies per element. Probably fine for a
      trivially copyable T and may well LOSE for String, so measure before building - the Collection
      result below is the precedent for batching not paying off where you assume it will.
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

- Perf thresholds could be ~2x tighter once there is PINNED history to calibrate from. The 3.0d24 pass
  used 1.20x margin against a 7% unpinned noise floor; pinned (which
  Build/Scripts/RunPerformanceRegressionTests now does) the floor is 2.9%, so ~1.10 would catch roughly
  half-size regressions. Needs a few pinned releases in the archive first.

- Iterable<T>::PeekSize () -> optional<size_t> ("do you know your size cheaply?"). STILL WANTED, not
  built. The full analysis - why it cannot be folded into size (), the two shapes that were rejected
  and why, and the shape to actually build (a protected bool on _IRep read by a NON-virtual
  PeekSize (), so zero vtable slots and none of the 41 size () overriders touched) - now lives as a
  design note on Iterable<T>::size () in Iterable.h, next to where anyone would look for it. Kept here
  because it is an open item, not a closed question.
  What would justify doing it: a real heuristic in the no-policy overloads (the "@todo measure the
  crossover ... eSeq is a placeholder, not a decision" notes in Iterable.inl / Sequence.inl) - choosing
  a policy by size needs a cheap size, and nullopt has an obvious answer there (use eSeq).

- test HearHE
