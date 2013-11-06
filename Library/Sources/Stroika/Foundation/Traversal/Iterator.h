/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_h_
#define _Stroika_Foundation_Traversal_Iterator_h_  1

#include    "../StroikaPreComp.h"

#include    <iterator>

#include    "../Configuration/Common.h"

#include    "../Memory/Optional.h"
#include    "../Memory/SharedByValue.h"



/**
 *
 *  \file
 *
 *  @todo   Consider adding a Refresh() method to iterator. Semantics would be
 *          to recheck the current item, and recheck the done state of the container.
 *
 *          Use case is with map/reduce. Consider one process doing the mapping, and
 *          another reading the output (if we have &param version of map). Then the
 *          reader could check if done, and wait a moment, and check again (wait? how that?)
 *
 *          The key is that with map reduce you have parallel mapping and processing the output.
 *          But if iterating over output, hard (not impossible) to know you are at the end (length).
 *
 *          Restartable iterators would be one way.
 *
 *  @todo   Consider having Iterator<T> have begin(), end() methods that do magic so
 *          you can also directly use an intertor in
 *              for (auto i : soemthingThatReturnsIterator()) {
 *              }
 *          I think easy and useful, and biggest concern is the potential for subtle
 *          overload confusion generation.
 *
 *          But PROBABLY NOT!!! - That is what Iterable is for. Instead - just have Traversal::mkIterable(Iterator<T>) - and
 *          then that iterable will be a trivail, short-lived private impl iterable that just indirects
 *          all iteration calls to that iterator!
 *
 *  @todo   Merge Current virtual call into More() call? Trouble is constructing
 *          T. We could make fields char fCurrent_[sizeof(T)] but that poses problems
 *          for copying iterators. On balance, probably best to bag it!!!
 *
 *  @todo   CLARIFY (decide on?) the behavior if you lose all references to a container? Does the
 *          iterator remain valid? Does it effectively retain a reference to teh contianer?
 *          Or does it instantly go 'empty'?
 *
 *          Not sure it matters much which but we must document this clearly.
 *
 *  @todo   See if we can replace Rep_Cloner_ stuff with lambda[] inline in type declaration? See if that
 *          has any negative impacts on performacnce/size etc (test win/gcc). Really more an issue
 *          for SharedByValue<> template.
 *
 *  @todo   CONSIDER using enabled_shared_from_this on ireps? Document why we chose to or not to
 *          (maybe test space). From preliminary testing (VC++2012) - it appears to just make things
 *          bigger, not smaller. Must test other compilers too, and maybe disassemble.
 *
 *  @todo   Speed tweeks
 *
 *          The Major Design limitaiton of this approach to iterators is that it requires a non-inlinable
 *          function call per iteration (roughly). Basically - if you pass a callback into an iterator rep
 *          then to APPLY that function on each iteration requires a non-inlinable (indirect through pointer)
 *          function call, or if you do the reverse, and directly use the iteraotr so no you can inline
 *          apply the function, you must call a virtual function for each iteration to bump the next pointer.
 *
 *          Fundementally - you have multiple polypmorphism (on representation of container and thing to apply
 *          at each iteration).
 *
 *          Two tricks:
 *              (1)     Read a bunch at a time. This is tricky to implement and doesn't affect overall computational
 *                      complexity (because it reduces number of virtual calls by a constant factor). But if that
 *                      constant factor is big enough - 10-100-1000? - that still could be relevant pragmatically.
 *
 *                      The biggest challenge is preserving the existing stafety and patch semantics generically,
 *                      in light of update during iteration, and making sure for uses where that doesnt help its
 *                      not allowed to hurt.
 *
 *              (2)     special case common combinations. For example - at the point in the code where you have:
 *                      Iterator<T> i;
 *                      for (; i != e.end (); ++i) {
 *                          do_this();
 *                      }
 *
 *                      One could put in special purpose code for the Iterator<T>::operator++ that said:
 *                      if (dynamic_cast<Sequence_Array::_IRep*> (myIRep) != nullptr) {
 *                          then peek and cheat...
 *                      }
 *                      Maybe when the iterator is constructed - it checks for a couple importnat types
 *                      and sets a flag, so the only cost when this doesnt work is checking that bool flag.
 *                      And the benefit in teh more common case is you avoid the virtual function call! so the it++ can be
 *                      inlined (a big win oftne times).
 *
 *  @todo   Consider as speed tweek
 *          // we allow fIterator to equal nullptr, as a sentinal value during iteration, signaling that iteration is Done
 *          #define qIteratorUsingNullRepAsSentinalValue    1
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  \brief
             *      An Iterator<T> is a copyable object which allows traversing the contents of some container.
             *
             *  An Iterator<T> is typically associated with some container (that is being iterated over)
             *  and which allows traversal from start to finish.
             *  (The iterator itself essentially provides a notion of *start* to *finish*).
             *
             *  There need not actually be a 'container' object. Other 'iterators' can be created, so long as
             *  they act like an iterator.
             *
             *  An Iterator<T> is a copyable object which can safely be used to capture (copy) the state of
             *  iteration and continue iterating from that spot. If the underlying container is
             *  modified, the iterator will be automatically updated to logically account for that update.
             *
             *  Iterators are robust in the presence of changes to their underlying container. Adding or
             *  removing items from a container will not invalidate the iteration.
             *
             *  Different kinds of containers can make further guarantees about the behavior of iterators
             *  in the presence of container modifications. For example a SequenceIterator will always
             *  traverse any items added after the current traversal index, and will never traverse items
             *  added with an index before the current traversal index.
             *
             *  For example:
             *
             *          for (Iterator<T> i = container.MakeIterator (); not i.Done (); i.Next ())  {
             *              f (i.Current ());
             *          }
             *
             *  or:
             *
             *          for (Iterator<T> i = container.begin (); i != container.end (); ++i)  {
             *              f (*i);
             *          }
             *
             *  or:
             *
             *          for (i : container)  {
             *              f (i);
             *          }
             *
             *  Key Differences between Stroika Iterators and STL Iterators:
             *
             *      1.      Stroika iterators are threadsafe. Updates to the underlying container
             *              never interfere with iteration, and need no explicit synchonization.
             *
             *      2.      Stroika iterators continue to work correctly when the underlying
             *              container is modified.
             *
             *      3.      Stroika iterators carry around their 'done' state all in one object.
             *              For compatability with existing C++ idiom, and some C++11 language features
             *              Stroika iterators inherit from std::iterator<> and allow use of end(),
             *              and i != end() to check for if an iterator is done. But internally,
             *              Stroika just checks i.Done(), and so can users of Stroika iterators.
             *
             *      4.      Stroika iterators are not 'random access'. They just go forwards, one step at a
             *              time. In STL, some kinds of iterators act more like pointers where you can do
             *              address arithmatic.
             *
             *
             * Some Rules about Iterators:
             *
             *      1.      What is Done() cannot be UnDone()
             *              That is, once an iterator is done, it cannot be restarted.
             *
             *      2.      Iterators can be copied. They always refer to the same
             *              place they did before the copy, and the old iterator is unaffected
             *              by iteration in the new (though it can be affected indirectly
             *              thru mutations).
             *
             *      3.      Whether or not you encounter items added after an addition is
             *              undefined (by Iterator<T> but often defined in specifically for
             *              particular container subtypes).
             *
             *      4.      A consequence of the above, is that items added after you are done,
             *              are not encountered.
             *
             *
             *  Interesting Design Notes:
             *
             *      -   We considered a desgin for Current() - where it would dynamically
             *          grab the current value, as opposed to being defined to be frozen/copied
             *          at the time of iteration.
             *
             *          The advantage of the path not taken is that if you iterated several times without
             *          examining the current value, and if the cost of copying was relatively large, this
             *          definition would have worked out better.
             *
             *          However, because I think its far more frequent that the copies are cheap, the
             *          user will want to look at each value, and the cost in terms of thread locking
             *          and probabe virtual function calls, the current approach of freezing / copying
             *          on iteration seemed better.
             *
             *          Iterator<T> can never 'be null' and so there is no need to check for a null
             *          Iterator 'Rep'.
             *
             *  @see Iterable<T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class  Iterator : public std::iterator<input_iterator_tag, T> {
            private:
                typedef typename    std::iterator<input_iterator_tag, T>    inherited;

            public:
                /**
                 *      \brief  ElementType is just a handly copy of the *T* template type which this
                 *              Iterator<T> parameterizes access to.
                 */
                typedef T   ElementType;

            public:
                class   IRep;
                typedef shared_ptr<IRep>    SharedIRepPtr;

            private:
                struct  Rep_Cloner_ {
                    static  SharedIRepPtr  Copy (const IRep& t);
                };

            public:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored
                 *          by users).
                 */
                typedef Memory::SharedByValue<Memory::SharedByValue_Traits<IRep, SharedIRepPtr, Rep_Cloner_>>   SharedByValueRepType;

            public:
                /**
                 *  \brief
                 *      This overload is usually not called directly. Instead, iterators are
                 *      usually created from a container (eg. Bag<T>::begin()).
                 *
                 *  \req RequireNotNull (rep.get ())
                 */
                explicit Iterator (const SharedIRepPtr& rep);
                Iterator () = delete;

            public:
                /**
                 *  \brief  Iterators are safely copyable, preserving their current position.
                 */
                Iterator (const Iterator<T>& from);

            public:
                /**
                 *  \brief  Iterators are safely copyable, preserving their current position.
                 */
                nonvirtual  Iterator<T>&    operator= (const Iterator<T>& rhs);

            public:
                /**
                 *  \brief
                 *      Return the Current value pointed to by the Iterator<T> (same as Current())
                 *
                 *  Support for range-based-for, and STL style iteration in general (containers must also
                 *  support begin, end).
                 *
                 *  This function is a synonym for @ref Current();
                 */
                nonvirtual  T       operator* () const;

            public:
                /**
                 *  \brief
                 *      Return a pointer to the current value pointed to by the Iterator<T> (like Current())
                 *
                 *  This function allows you to write i->b, where i is an iterator and b is a member of the type
                 *  iterated over by i.
                 *
                 *  Note - the lifetime of this pointer is short - only until the next operation on the wrapper
                 *  class instance Iterator<T>.
                 */
                nonvirtual  T*          operator-> ();
                nonvirtual  const T*    operator-> () const;

            public:
                /**
                 *  \brief
                 *      Advance iterator; support for range-based-for, and STL style iteration in
                 *      general (containers must also support begin, end).
                 *
                 *  Advance iterator; support for range-based-for, and STL style iteration in general
                 *  (containers must also support begin, end).
                 *
                 *  operator++ can be called anytime as long as Done () is not true (must be called
                 *  prior to operator++). It then it iterates to the  item in the container (i.e. it
                 *  changes the value returned by Current).
                 *
                 *  Note - the value return by Current() is frozen (until the next operator++() call)
                 *  when this method is called. Its legal to update the underlying container, but those
                 *  values won't be seen until the next iteration.
                 */
                nonvirtual  void    operator++ ();
                /**
                 *  \brief
                 *      Advance iterator; support for range-based-for, and STL style iteration in general
                 *      (containers must also support begin, end).
                 *
                 *  This function is identical to @ref operator++();
                 *
                 *  @see operator++()
                 */
                nonvirtual  void    operator++ (int);

            public:
                //tmphack to test - LGP 2013-06-06 - so can say "Iterator i = x; if (i) {...
                // - handy for stuff like ApplyUntilTrue ()...
                nonvirtual  operator bool () const {
                    return not Done ();
                }

            public:
                /**
                 *  \brief
                 *  Provides a limited notion of equality suitable for STL-style iteration and iterator comparison.
                 *
                 *  Two iterators are considered WeakEquals() if they are BOTH Done(). If one is done, but the
                 *  other not, they are not equal. If they are both not done, they are both equal if they are the
                 *  exact same rep.
                 *
                 *  Note - for WeakEquals(). The following assertion will fail:
                 *
                 *          Iterator<T> x = getIterator();
                 *          Iterator<T> y = x;
                 *          x++;
                 *          y++;
                 *          Assert (WeakEquals (x, y));    // This MAY succeed or MAY fail! - it will succeed IFF x.Done()
                 *
                 *  When there are two copies of an iterator, and one copy is modified, this breaks the connection between
                 *  the iterators, so they can never be equal again.
                 *
                 *  This definition was chosen to be sufficient to provide for efficient implementation of STL-style
                 *  iteration (note that operator==() is an alias for WeakEquals()).
                 *
                 *      Iterator<T> i   =   getIterator();
                 *      Iterator<T> e   =   end ();
                 *
                 *      for (; i != e; ++i) {
                 *      }
                 *
                 *  This style works because e.Done () is always true, (and the Rep for e is always different than
                 *  the rep for i). and so the only way for the iterators to become equal is for i.Done () to be true.
                 *
                 *  Note that WeakEquals() is *commutative*.
                 *
                 *  @see StrongEquals().
                 */
                nonvirtual  bool    WeakEquals (const Iterator& rhs) const;

            public:
                /**
                 *  \brief
                 *  StrongEquals () is a more restrictive, more logically coherent, but more expensive to compute
                 *  definition of Iterator<T> equality.
                 *
                 *  StrongEquals () is a more restrictive, more logically coherent, but potentially more expensive
                 *  to compute definition of Iterator<T> equality.
                 *
                 *  \em NB: StrongEquals () is the same notion of equality as used by STL iterators.
                 *
                 *  Very roughly, the idea is that to be 'equal' - two iterators must be iterating over the same source,
                 *  and be up to the same position. The slight exception to this is that any two iterators that are Done()
                 *  are considered StrongEquals (). This is mainly because we use a different representation for 'done'
                 *  iterators. They are kind-of-fake iterator objects.
                 *
                 *  NB:
                 *
                 *      if (a.StrongEquals (b)) {
                 *          Assert (a.WeakEquals (b));
                 *      }
                 *
                 *  HOWEVER:
                 *
                 *      if (a.WeakEquals (b)) {
                 *          Assert (a.StrongEquals (b) OR not a.StrongEquals (b));  // all bets are off
                 *      }
                 *
                 *  Note - for StrongEquals. The following assertion will succeed:
                 *
                 *          Iterator<T> x = getIterator();
                 *          Iterator<T> y = x;
                 *          x++;
                 *          y++;
                 *          Assert (StrongEquals (x, y));    // will always succeed (x++ and y++ may fail if iterator already at end)
                 *                                           // See WeakEquals ()
                 *
                 *  Note that StrongEquals is *commutative*.
                 *
                 *  @see WeakEquals ().
                 */
                nonvirtual  bool    StrongEquals (const Iterator& rhs) const;

            public:
                /**
                 *  \brief  WeakEquals(): this is normally used to compare two iterators already known to be from the same source.
                 *
                 *  operator==() just maps trivially to WeakEquals().
                 *
                 *  In fact, the definition of WeakEquals() was chosen to most efficiently, but adequately make STL-style
                 *  iterator usage work properly.
                 */
                nonvirtual  bool    operator== (const Iterator& rhs) const;

            public:
                /**
                 *  \brief  Returns not (operator==())
                 */
                nonvirtual  bool    operator!= (const Iterator& rhs) const;

            public:
                /**
                 *  \brief
                 *  Returns the value of the current item visited by the Iterator<T>, and is illegal to call if Done()
                 *
                 *  Current() returns the value of the current item visited by the Iterator<T>.
                 *
                 *  Two things can change the current value of Current():
                 *      *   operator++()
                 *      *   Done()  (or any similar functions, NotDone() operator==(), operator!=(), or any of the
                 *          range-based-for code which implicitly calls these methods.
                 *
                 *  Because of this, two subsequent calls to it.Current () *cannot* return different values with no
                 *  intervening calls on the iterator, even if the underlying container changes.
                 *
                 *  So even in multithread scenarios, its always safe to say:
                 *
                 *      if (not it.Done ()) {
                 *          T v = it.Current ();
                 *      }
                 *
                 *  and moreover, even in multithreaded scenarios, where another thread is randomly and rapidly modifying
                 *  a container:
                 *
                 *      if (not it.Done ()) {
                 *          T v1 = it.Current ();
                 *          sleep(1);
                 *          T v2 = it.Current ();
                 *          Assert (v1 == v2);      // they refer to the same value - this COULD fail
                 *                                  // if you defined a queer operator== that had operator==(x,x) return false)
                 *      }
                 *
                 *  The value of Current is undefined (Assertion error) if called when Done().
                 *
                 *  operator*() is a common synonym for Current().
                 *
                 *  @see operator*()
                 *  @see operator++()
                 *
                 */
                nonvirtual  T       Current () const;

            public:
                /**
                 *  \brief
                 *      Done () means there is nothing left in this iterator (a synonym for (it == container.end ()).
                 *
                 *  Done () means there is nothing left to visit in this iterator.
                 *
                 *  Once an iterator is Done(), it can never transition to 'not Done()'.
                 *
                 *  When an iterator is Done(), it is illegal to call Current().
                 *
                 *  Calling Done() *may* change (initialize) the value which would be returned by the next
                 *  call to Current().
                 *
                 *      NB: There are *no* modifications to an underlying container which will directly change
                 *      the value of Done(). This value only changes the next time the cursor is advanced
                 *      via a call to operator++();
                 *
                 *      if it comes from container, then (it == container.end ()) is true iff it.Done()
                 */
                nonvirtual  bool    Done () const;

            public:
                /**
                 *  \brief
                 *  Same as *somecontainer*::end ()
                 *
                 *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
                 *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
                 */
                static  Iterator<T>     GetEmptyIterator ();

            public:
                /**
                 *  \brief
                 *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
                 *      mainly intended for implementors.
                 *
                 *  Get a reference to the IRep owned by the iterator.
                 *  This is an implementation detail, mainly intended for implementors.
                 */
                nonvirtual  IRep&               GetRep ();
                nonvirtual  const IRep&         GetRep () const;

            private:
                SharedByValueRepType    fIterator_;

            private:
                Memory::Optional<T>     fCurrent_;      // SSW 9/19/2011: naive impementation that requires a no-arg constructor for T and has to build a T before being asked for current

            private:
                static  SharedIRepPtr    Clone_ (const IRep& rep);
            };


            /**
             *
             *  \brief  Implementation detail for iterator implementors.
             *
             *  IRep is a support class used to implement the @ref Iterator<T> pattern.
             *
             *  Subclassed by front-end container writers.
             *  Most of the work is done in More, which does a lot of work because it is the
             *  only virtual function called during iteration, and will need to lock its
             *  container when doing "safe" iteration. More does the following:
             *  iterate to the next container value if advance is true
             *  (then) copy the current value into current, if current is not null
             *  return true if iteration can continue (not done iterating)
             *
             *  typical uses:
             *
             *          it++ -> More (null, true)
             *          *it -> More (&v, false); return v;
             *          Done -> More (null, false)
             *
             *          (note that for performance and safety reasons the iterator envelope actually
             *           passes fCurrent_ into More when implenenting ++it
             */
            template    <typename T>
            class  Iterator<T>::IRep {
            protected:
                IRep ();

            public:
                virtual ~IRep ();

            public:
                typedef typename Iterator<T>::SharedIRepPtr SharedIRepPtr;

            public:
                /**
                 * Clone() makes a copy of the state of this iterator, which can separately be tracked with StrongEquals ()
                 * and/or More() to get values and move forward through the iteration.
                 */
                virtual SharedIRepPtr   Clone () const                      = 0;
                /**
                 *  More () is dual function - depending on its arguments. If advance is true, it moves the
                 *  iterator to the next legal position.
                 *
                 *  If current is not nullptr, then after that advance (if any) - the current value is copied back out.
                 *
                 *  It IS legal to call More () with advance true even when already at the end of iteration.
                 *  This design choice was made to be multi-threading friendly.
                 *
                 *  This function returns true iff the iterator is positioned at a valid position, and if (advance
                 *  is true, then More() returns true for exactly the cases where a valid value is copied out.
                 *
                 *  @todo FIX THIS DOCS FOR NEW ARGS TO MORE
                 */
                virtual void    More (Memory::Optional<T>* result, bool advance)     = 0;
                /**
                 * \brief two iterators must be iterating over the same source, and be up to the same position.
                 *
                 *  @see Iterator<T>::StrongEquals
                 */
                virtual bool    StrongEquals (const IRep* rhs) const            = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Iterator.inl"

#endif  /*_Stroika_Foundation_Traversal_Iterator_h_ */
