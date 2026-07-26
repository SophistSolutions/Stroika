# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- error on ubuntu 26.04 - 
    /usr/include/c++/16/debug/safe_iterator.h:222:
    In function:
        constexpr gnu_debug::_Safe_iterator<_Iterator, _Sequence, 
        _Category>::_Safe_iterator(gnu_debug::_Safe_iterator<_Iterator, 
        _Sequence, _Category>&&) [with _Iterator = gnu_cxx::
        normal_iterator<const int*, std::vector<int, std::allocator<int> > >; 
        _Sequence = std::debug::vector<int>; _Category = 
        std::forward_iterator_tag]

    Error: attempt to copy-construct an iterator from a singular iterator.

    Objects involved in the operation:
        iterator "this" @ 0x7b8e9aa614c0 {
        type = gnu_cxx::normal_iterator<int const*, std::vector<int, std::allocator<int> > > (constant iterator);
        state = singular (value-initialized);
        }
        iterator "other" @ 0x7b8e9a9ccd70 {
        type = gnu_cxx::normal_iterator<int const*, std::vector<int, std::allocator<int> > > (constant iterator);
        state = singular;
        }
    Aborted


- add more g++16 c++23 configs
- Finish CONTAINER SEQUNNCE BIDI ITERATOR STUFF
- test WTF 
- test IPAM - etc...
- test HearHE
- deal with failed/lost bugs from JIRA
