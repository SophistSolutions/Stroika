# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- ROTATE the personal access token stored in plaintext in a local (NOT in-repo) git config on the
  Windows dev box - github.com/settings/tokens, revoke and reissue. Noticed 2026-08-14. It is in a
  world-readable file, and it was read into an AI session transcript, which is enough on its own -
  a token that has left the machine should be treated as compromised whether or not it was misused.
  Prefer a credential helper (Git Credential Manager) over a config entry when reissuing, so the
  replacement is not sitting in cleartext too.

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
    - WHAT WOULD JUSTIFY BUILDING IT: putting a real heuristic inside the no-policy overloads (see the
      OrderBy item above). Choosing a policy by size needs a CHEAP size, and nullopt has an obvious
      right answer there (use eSeq). Until something concrete needs it, do not add it.

- test HearHE
- do a performance compare with checked in data

- function vs movable_function etc winging on internet

+ Consider adding Mapping_stdflatmap? Is there such a thing? Maybe fast for small sizes?

