# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

 - Sequence_stdvector still uses the generic fallback for both -
      needs STLContainerWrapper itself to grow Bidi/RandomAccess iterator support first (it's currently
      forward-only, unrelated pre-existing @todo in that file). Sequence_LinkedList (singly-linked)
      always uses the generic fallback - no efficient native alternative is possible.
      --LGP 2026-06-29 (updated 2026-08-05)

- Sequence<>::OrderBy (how todo now that we have better iterator support)

- test HearHE
- deal with failed/lost bugs from JIRA
