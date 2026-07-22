# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- [ ] `RandomAccessIterator.h`/`.inl`: the three int⊕iterator friend operators (`operator+ (i, it)`,
      `operator- (i, it)`, and `operator- (lhs, rhs)` returning `difference_type`) are declared as
      hidden friends using the class's own template params, but *defined* out-of-line as independent
      function templates in the `.inl` — that doesn't bind to the friend declaration. GCC warns
      `declares a non-template function`, and the warning is silenced right next to the declarations
      (comment: "very tricky to avoid this — tried") instead of being fixed. Calling any of these
      three currently fails to link. Also, even setting linkage aside, `operator- (i, it)`'s body
      (`return -(it - i);`) applies unary `-` to a `RandomAccessIterator`, which has no such operator
      — so the body itself doesn't compile once instantiated either.
      See [RandomAccessIterator.h:172-191](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.h#L172-L191),
      [RandomAccessIterator.inl:136-146](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.inl#L136-L146).
      Likely fix: define these three inline in the class body (per-instantiation hidden friends),
      rather than trying to match them to an out-of-line template.

- move scriptslib to Build folder
- Move makedefaultconfigs from makefile into script in that folder
- add more g++16 c++23 configs
- test WTF 
- test IPAM - etc...
- test HearHE
- Finish CONTAINER SEQUNNCE BIDI ITERATOR STUFF
