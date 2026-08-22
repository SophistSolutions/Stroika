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

- test HearHE
