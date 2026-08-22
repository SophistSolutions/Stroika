# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- Iterable<T>::PeekSize () -> optional<size_t> ("do you know your size cheaply?"). STILL WANTED, not
  built. The full analysis - why it cannot be folded into size (), the two shapes that were rejected
  and why, and the shape to actually build (a protected bool on _IRep read by a NON-virtual
  PeekSize (), so zero vtable slots and none of the 41 size () overriders touched) - now lives as a
  design note on Iterable<T>::size () in Iterable.h, next to where anyone would look for it. Kept here
  because it is an open item, not a closed question.
  What would justify doing it: a real heuristic in the no-policy overloads (the "@todo measure the
  crossover ... eSeq is a placeholder, not a decision" notes in Iterable.inl / Sequence.inl) - choosing
  a policy by size needs a cheap size, and nullopt has an obvious answer there (use eSeq).

- InlineBuffer and default_initializable<T>: require it on the methods that actually need it, and need
  it in fewer places. Today the class requires nothing up front, but:
    - resize (n) - and so InlineBuffer (size_t n), which is a resize - genuinely needs it: the growing
      branch does uninitialized_fill (end, newEnd, T{}) (InlineBuffer.inl ~L239). Put a
      requires default_initializable<T> on those, so the failure names the real constraint instead of
      erupting deep inside the .inl.
    - clear () needs it only BY ACCIDENT: it is resize (0), which can only ever SHRINK, but instantiating
      resize compiles the grow branch's T{} anyway. Fix by having clear () do what resize's shrink path
      does - DestroyElts_ (begin (), end ()) and fSize_ = 0 - after which any copy-constructible T works.
    - ctor (eUninitialized, n), push_back () and reserve () do NOT need it (uninitialized_copy / realloc),
      so a CLASS-level requirement would be too strong and would forbid exactly the scratch-buffer usage
      pattern.
  Payoff beyond tidiness: Sequence<T>::AppendAll ()/InsertAll () and Collection<T>::AddAll () currently
  gate their chunked path on default_initializable<T> SOLELY because of clear (), so a T that cannot be
  default constructed silently falls back to element-at-a-time. Fixing clear () lets that gate go and the
  chunking apply universally. Real in-tree example of such a T: the
  pair<ConnectionOrientedMasterSocket::Ptr, Set<...>> used by Execution::WaitForIOReady.

- test HearHE
