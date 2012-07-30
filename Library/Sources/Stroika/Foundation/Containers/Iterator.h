/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterator_h_
#define _Stroika_Foundation_Containers_Iterator_h_  1

/*
 *
 *      Iterators allow ordered traversal of a Collection. In general, the only
 *  guarantee about iteration order is that it will remain the same as long as
 *  the Collection has not been modified. Difference subclasses of Collection
 *  more narrowly specify the details of traversal order: for example a Stack
 *  will always iterate from the top-most to the bottom-most item.
 *
 *      Iterators are robust against changes to their collection. Adding or
 *  removing items from a collection will not invalidate the iteration. The
 *  only exception is that removing the item currently being iterated over
 *  will invalidate the results of the Current () method (until the next call
 *  to the Next () method). Subclasses of Collection can make further
 *  guarantees about the behavior of iterators in the presence of Collection
 *  modifications. For example a SequenceIterator will always traverse any
 *  items added after the current traversal index, and will never traverse
 *  items added with an index before the current traversal index.
 *
 *
 *      Note that we utilize a for-loop based approach to iteration. A
 *  somewhat popular alternative is modeled on Lisp usage: iterating over a
 *  passed in function. This style is sometimes refereed to as "passive"
 *  iteration. However, given C++ lack of support for lambda expressions
 *  (anonymous code blocks) the for-loop based approach seems superior,
 *  since it is always at least as convenient and at least as efficient,
 *  and often is slightly more convenient and slightly more convenient.
 *  DieHards can write ForEach style macros to support the passive style.
 *  For example:
 *      #define Apply(T,Init,F)\
            for (IteratorRep<T> it (Init); not it.Done (); it.Next ())  { (*F) (it.Current ()); }
 *  allows usages like Apply(int, fList, PrintInt);
 *
 *
 *
 * MORE RULES ABOUT ITERATORS (TO BE INTEGRATED INTO DOCS BETTER)
 *
 *      (1)     What is Done() cannot be UnDone()
 *              That is, once an iterator is done, it cannot be restarted.
 *      (2)     Iterators can be copied. They always refer to the same
 *              place they did before the copy, and the old iterator is unaffected
 *              by iteration in the new (though it can be affected indirectly
 *              thru mutations).
 *      (3)     Whether or not you encounter items added after an addition is
 *              undefined.
 *      (4)     A consequence of the above, is that items added after you are done,
 *              are not encountered. I'm not sure if this is currently true. We
 *              may want to rethink definitions, or somehow make this true.
 *
 *
 *  TODO:
 *
 *
 *      o   RETHINK RangedForIterator - I'm not sure why we need this. Make Tally<T> subclass from Iterable<TallyEntry<T>>?
 *
 *      o   FIX/LOSE qIteratorsRequireNoArgContructorForT stuff. Also related to quirk about REPS being constructed
 *          in the wrong state and requiring an initial ++.
 *          FIX THIS before supporting more typest that require Iterable<T> / Iterator<T>.
 *
 *      o   Merge Current virtual call into More() call? Trouble is constructing
 *          T. We could make fields char fCurrent_[sizeof(T)] but that poses problems
 *          for copying iterators. On balance, probably best to bag it!!!
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "../Memory/SharedByValue.h"


// SSW 9/19/2011: remove this restriction for more efficiency and flexibility
#define qIteratorsRequireNoArgContructorForT    1




/*
 *      Iterator are used primarily to get auto-destruction
 *  at the end of a scope where they are used. They can be used directly,
 *  or using ranged for syntax (currently imitated by For macro)
 *
 *  Current is a synonym for operator*. Done is a synonym for iterator != container.end ()
 *  Current can be called at anytime not Done. The value of
 *  Current is guaranteed to be valid if it was called before Done, even if that value
 *  was removed from the container at some point after Current was called.
 *
 *  The value of Current is undefined if called when Done.
 *
 *  Operator++ can be called anytime. If not done, then it iterates to the next
 *  item in the collection (i.e. it changes the value returned by Current).
 *
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             *      An Iterator<T> is an object which is associated with some container (that is being iterated over)
             *  and which allows traversal of that object from start to finish (the iterator itself essentially provides
             *  A notion of START to FINISH). An Iterator<T> is a copyable object which can safely be used to capture
             *  the state of iteration (copy) and continue iterating from that spot. If the underlying object is
             *  modified, the iterator will be automatically updated to logically account for that update (FILL IN DETAILS - SEE ABOVE COMMENTARY)
             */
            template    <typename T>
            class  Iterator {
            public:
                /*
                 * ElementType is just a handly copy of the "T" template type which parameterizes this Iterator<T>.
                 */
                typedef T   ElementType;

            public:
                class   IRep;

            private:
                NO_DEFAULT_CONSTRUCTOR (Iterator);
            public:
                explicit Iterator (IRep* it);
            public:
                Iterator (const Iterator<T>& from);
                ~Iterator ();

                nonvirtual  Iterator<T>&    operator= (const Iterator<T>& rhs);

                // support for Range based for, and stl style iteration in general (containers must also support begin, end)
            public:
                nonvirtual  T       operator* () const;

            public:
                nonvirtual  void    operator++ ();
                nonvirtual  void    operator++ (int);

            public:
                /*
                 *      Two iterators are considered EQUAL if they are BOTH Done (). If one is done, but the other not,
                 *  they are not equal. If they are both not done, they are both equal if they are the
                 *  exact same rep.
                 *
                 *      Note - this is a STRONG definition of equality. The following assertion will fail:
                 *
                 *          Iterator<T> x = getIterator();
                 *          Iterator<T> y = x;
                 *          x++;
                 *          y++;
                 *          Assert (x == y);    // This MAY succeed or MAY fail! - it will succeed IFF x.Done()
                 *
                 *
                 *  When there are two copies of an iterator, and one copy is modified, this breaks the connection between the
                 *  iterators, so they can never be equal again.
                 *
                 *  This definition was chosen to be sufficient to provide for efficient implementaiton of STL-style iteration.
                 *
                 *      Iterator<T> i   =   getIterator();
                 *      Iterator<T> e   =   end ();
                 *
                 *      for (; i != e; ++i) {
                 *      }
                 *
                 *      This style works because e.Done () is always true, (and the Rep for e is always different than the rep for i).
                 *      and so the only way for the iterators to become equal is for i.Done () to be true.
                 *
                 *  A Deeper notion of Iterator Equality might be achievable, by first checking that the LHS and RHS both were
                 *  iterating over the same container (came from the same source), and then calling a virtual method on one (since they
                 *  would be the same dynamic type) - and have that dynamically check for equality. But this form of equality testing
                 *  would be dramatically more performance costly, and would serve little practical purpose I can see
                 *  right now.
                 */
                nonvirtual  bool    operator== (const Iterator& rhs) const;

            public:
                /*
                 * See the definition of operator==
                 */
                nonvirtual  bool    operator!= (const Iterator& rhs) const;

            public:
                // Synonyms for above, sometimes making code more readable
                // Current -> operator*
                // Done -> (it != container.end ())
                nonvirtual  T       Current () const;
                nonvirtual  bool    Done () const;

            public:
                /*
                 *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
                 *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
                 */
                static  Iterator<T>     GetEmptyIterator ();

            protected:
                nonvirtual  IRep&               _GetRep ();
                nonvirtual  const IRep&         _GetRep () const;

            private:
                // probably should not need to use SharedByValue_CopyByFunction....
                Memory::SharedByValue<IRep, Memory::SharedByValue_CopyByFunction<IRep>>    fIterator_;

            private:
                T       fCurrent_;   // SSW 9/19/2011: naive impementation that requires a no-arg constructor for T and has to build a T before being asked for current

            private:
                static  IRep*    Clone_ (const IRep& rep);
            };


            /*
                Subclassed by front-end container writers.
                Most of the work is done in More, which does a lot of work because it is the
                only virtual function called during iteration, and will need to lock its
                container when doing "safe" iteration. More does the following:
                    iterate to the next container value if advance is true
                    (then) copy the current value into current, if current is not null
                    return true if iteration can continue (not done iterating)

                    typical uses:
                        it++ -> More (null, true)
                        *it -> More (&v, false); return v;
                        Done -> More (null, false)

                        (note that for performance and safety reasons the iterator envelope actually
                        passes fCurrent_ into More when implenenting ++it
            */
            template    <typename T>
            class  Iterator<T>::IRep {
            protected:
                IRep ();

            public:
                virtual ~IRep ();

            public:
                virtual IRep*   Clone () const                      = 0;
                virtual bool    More (T* current, bool advance)     = 0;

            public:
                nonvirtual bool Done () const;
            };



            /*
             For macro:
             This will be removed when compilers support ranged for. Just replace For with for, and the comma with a colon
             For (it, myBag) with for (it : myBag)
             */
#define For(_it,_Container)         for (auto _it = _Container.begin (); _it != _Container.end (); ++_it)

        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Iterator_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Iterator.inl"
