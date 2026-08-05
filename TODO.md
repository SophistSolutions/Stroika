# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

 - MakeBidirectionalIterator ()/MakeRandomAccessIterator () (below) are now implemented generically,
      in terms of GetAt ()/size (), so they always work, for any backend - including Sequence_LinkedList,
      which cannot produce a native BidirectionalIterator reasonably. This resolves the design question
      raised below, but at the cost of O(GetAt ()) per step for backends without an efficient native
      random-access iterator.

      Sequence_Array now uses Private::RandomAccessIteratorImplHelper_ to back both
      MakeBidirectionalIterator ()/MakeRandomAccessIterator () with DataStructures::Array<T>::ForwardIterator's
      native random-access support (O(1) per step) rather than the generic GetAt ()-based fallback.
      Sequence_stdvector/Sequence_DoublyLinkedList (bidirectional case only, for the latter) still use the
      generic fallback - Sequence_stdvector needs STLContainerWrapper itself to grow Bidi/RandomAccess
      iterator support first (it's currently forward-only, unrelated pre-existing @todo in that file);
      Sequence_DoublyLinkedList could be wired to DoublyLinkedList<T>::BidirectionalIterator similarly to
      how Sequence_Array was done. Sequence_LinkedList (singly-linked) always uses the generic fallback -
      no efficient native alternative is possible.
      --LGP 2026-06-29 (updated 2026-08-05)

- Sequence<>::OrderBy (how todo now that we have better iterator support)


- test HearHE
- deal with failed/lost bugs from JIRA
