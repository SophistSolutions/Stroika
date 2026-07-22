# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- [ ] `RandomAccessIterator.inl`: free `operator-` (line ~151) calls `lhs.Distance (rhs)`, but no
      `Distance` method exists — the real method is `Difference`. Won't compile.
      See [RandomAccessIterator.inl:151](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.inl#L151),
      [RandomAccessIterator.h:101](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.h#L101).

- [ ] `RandomAccessIterator.h`/`.inl`: `operator+=` and `operator-=` are declared/defined `const`
      (both the header decls and `.inl` bodies), but they mutate the iterator via
      `GetRep ().Advance (i)` (which itself requires a non-const `GetRep()`) and return
      `RandomAccessIterator&`. The `const` is simply wrong and should be removed.
      See [RandomAccessIterator.h:143-151](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.h#L143-L151),
      [RandomAccessIterator.inl:102-112](Library/Sources/Stroika/Foundation/Traversal/RandomAccessIterator.inl#L102-L112).


- move scriptslib to Build folder
- Move makedefaultconfigs from makefile into script in that folder
- add more g++16 c++23 configs
- test WTF 
- test IPAM - etc...
- test HearHE
- Finish CONTAINER SEQUNNCE BIDI ITERATOR STUFF
