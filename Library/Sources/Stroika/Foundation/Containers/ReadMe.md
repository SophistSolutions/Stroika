Overview
--------
  This folder contains all the Stroika Library Foundation::Containers source code.

  Containers are objects that collect together a related (parameterized) set of objects.
  For example, a Stack<T>, or Set<T>, or Sequence<T>.


Quick Start
-----------
	#include "Stroika/Foundation/Containers/Seqeunce.h"

	using namespace Stroika::Foundation::Containers;

	Sequence<int> x = { 1, 3, 4 };
	Assert (x.length () == 3);
	x.push_back (3);
	Assert (x.length () == 4);
	Assert (x[1] == 3);
	x = Sequence_LinkedList<int> (x);      // convert x to use a linked-list representation
	Assert (x[1] == 3);                    // all data same as before
	x = Sequence_stdvector<int> (x);       // convert x to use a std::vector representation
	Assert (x.length () == 4);             // all data same as before
	std::list<int> y = x.As<list<int>> (); // convert to STL types


Rejected Ideas
--------------
   *   Make KEYs a UNIFYING concept for containers
      *   Think about somehow making KEYs a UNIFYING concept for containers? 
          Special case of unsticky keys -
          like array indexes? In that sense, a KEY is almost like an ITERATOR. In fact, 
          an interator is somewhat better than a key. 
      *   We may want Find() to (optionally) return an iterator positioned at the first entry
          (unclear what ++ would do).
      *   We may want a[i] or some rough analog for sequnces to get a sequence - offset by I.
      *   **Reason Rejected**:
          This really only applies to randomly accessed containers (so not stack, deque etc).
          Though its possible to define for them, not usefully. Its probably better to just
          keep key as a 'key' concept for Map<>, and use 'index' - which is analagous - but different -
          for sequence (important difference is stickiness of assocation when container is modified).

   *   No Compact() methods
      *   Stroika 1.0 had Compact() methods - that could be used to generically request that a container
          be compacted.
             
          We decided against that for Stroika v2 because
             1.	   We COULD always add it back.
             2.    Its just a class of optimziations which makes sense for some backends. But other 
				   optimizations make snese for other classes of backends. You can always retain a smartptr
				   with OUR type of backend! So really can be added just for _Array<> impls. Note - this
				   rationale doesn't work perfectly due to copy-by-values semantics with 'casts' but still
				   OK.
