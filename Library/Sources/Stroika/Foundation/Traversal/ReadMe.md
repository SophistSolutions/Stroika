# Stroika::[Foundation](../ReadMe.md)::Traversal

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::Traversal source code.

## Overview

Stroika Traversal classes are those related to traversing data strucutres. Things like Iterators, and Ranges are defined here.

- [DiscreteRange.h](DiscreteRange.h) - range of integer like things
- [DisjointRange.h](DisjointRange.h) - not a range - but a sequence of ranges
- [DisjointDiscreteRange.h](DisjointDiscreteRange.h)
- [FunctionalApplication.h](FunctionalApplication.h)
- [Generator.h](Generator.h)
- [Iterable.h](Iterable.h) - like C# IEnumerable\<T>
- [IterableFromIterator.h](IterableFromIterator.h)
- [Iterator.h](Iterator.h)
- [RandomAccessIterator.h](RandomAccessIterator.h)
- [Range.h](Range.h) - more like Ruby Range, not much like c++20 range

---

DRAFT FUTURE NOTES - INCOMPLETE BELOW...

GENERATOR:

```c++
loop (yielder)
{
  while (1) {
    yielder(val);
  }
}
```

    // the question is - is there any way todo this without threads? I could run a look on a thread and yield would 'send a message' back to the
    // iterator? That maybe how it needs to happen.
    	class generator : iterable<T> {
    		public:

    			generator (std:function<void(std::function<void(T)> yielder)>);
    	};

    	Plus - we need a better way to handle size () - like 'not supported' - or maybe lose it as a common method of iterable?
    	we could just iterate (make new useless iterator). That maybe OK. But not for things that genrate  infinite sequne.
