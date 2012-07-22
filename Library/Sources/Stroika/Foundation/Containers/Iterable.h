/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterable_h_
#define _Stroika_Foundation_Containers_Iterable_h_  1

/*
 *
 *
 *
 *  TODO:
 *
 *          (o)         CLARIFY AND ENFORCE ISNULL semantics. Either treat as EMPTY - or ILLEGAL - but either way make clear
 *                      and ENFORCE. See existing pattern with existing containers for hint of which way to go...
 *
 *          (o)         Consider adding class Mutable<T> to add reference to mutator stuff? Maybe no point?
 *
 *          (o)         Move APPLYISH APIs that can be into here (stuff purely based on iterating)
 *
 *          (o)         Stack<T> should NOT be ITERABLE, but other types like Bag<> and Tally<> sb iterable (but tally probably
 *                      Iterable<TallyEntry<T>>.
 *
 *          (o)         When this code matures, plan is to OBSOLETE/DELETE the Collection code...
 *
 *			(o)			Apply/ApplyUntilTrue() should also take overload with function object (STL). Also, consider
 *						providing a IREP version - to implement LOCKING logic promised in the API.
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            template    <typename T> class  Iterable {
            public:
                class  IRep {
                protected:
                    IRep ();

                public:
                    virtual ~IRep ();

                public:
                    virtual Iterator<T>     MakeIterator () const   = 0;
                    virtual size_t          GetLength () const  =   0;
                    virtual bool            IsEmpty () const    =   0;
                };

            public:
                explicit    Iterable (const Memory::SharedByValue<IRep, Memory::SharedByValue_CopyByFunction<IRep>>& rep);

            public:
                Iterable (const Iterable<T>& from);
                ~Iterable ();

                nonvirtual  Iterable<T>&    operator= (const Iterable<T>& rhs);

            public:
                nonvirtual Iterator<T>      MakeIterator () const;
                nonvirtual  size_t          GetLength () const;
                nonvirtual  bool            IsEmpty () const;

            public:
                // STL-ish alias for IsEmpty ()
                nonvirtual  bool    empty () const;
            public:
                // STL-ish alias for GetLength ()
                nonvirtual  size_t  length () const;
            public:
                // STL-ish alias for GetLength ()
                nonvirtual  size_t  size () const;

            public:
                // Support for ranged for, and stl syntax in general
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;

            public:
                /*
                    * Take the given function argument, and call it for each element of the Collection. This is equivilent to:
                    *      for (Iterator<T> i = begin (); i != end (); ++i) {
                    *          (doToElement) (item);
                    *      }
                    *
                    *  However, in threading scenarios, this maybe preferable, since it counts as an atomic operation that will happen to each element without other
                    *  threads intervening to modify the container.
					*
					*	Also, note that this function does NOT change any elements of the Iterable.
                    */
                nonvirtual  void    Apply (void (*doToElement) (T item)) const;

            public:
				/*
					* Take the given function argument, and call it for each element of the Collection. This is equivilent to:
					*      for (Iterator<T> i = begin (); i != end (); ++i) {
					*          if ((doToElement) (item)) {
					*              return it;
					*          }
					*      }
					*      return end();
					*
					*  However, in threading scenarios, this maybe preferable, since it counts as an atomic operation that will happen to each element without other
					*  threads intervening to modify the container.
					*
					*  This function returns an iteartor pointing to the element that triggered the abrupt loop end (for example the element you were searching for?).
					*  It returns the specail iterator end () to indicate no doToElemet() functions returned true.
					*
					*	Also, note that this function does NOT change any elements of the Iterable.
					*/
				nonvirtual  Iterator<T>    ApplyUntilTrue (bool (*doToElement) (T item)) const;

			
			protected:
                Memory::SharedByValue<IRep, Memory::SharedByValue_CopyByFunction<IRep>> _fRep;
            };

        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Iterable_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Iterable.inl"
