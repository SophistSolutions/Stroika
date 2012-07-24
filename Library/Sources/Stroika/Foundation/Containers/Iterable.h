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
 *      (o)         Consider adding class Mutable<T> to add reference to mutator stuff? Maybe no point?
 *
 *      (o)         Consider adding class ???<T> to add Iterable<T> like functions - where we can count on "T"
 *                  having TWithCompareEquals<T>
 *
 *      (o)         When this code matures, plan is to OBSOLETE/DELETE the Collection code...
 *
 *      (o)         Apply/ApplyUntilTrue() should also take overload with function object (STL). Also,
 *                  consider providing a _IRep version - to implement LOCKING logic promised in the API.
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

            /*
             *  Iterable<T> which supports the Iterator<T> API, and allows for the creation of an Iterator.
             *
             *  The Stroika iterators can be used either directly, or in the STL begin/end style - and this
             *  class supports both styles of usage.
             *
             *  Iterable<T> also supports read-only applicative operations on the contained data.
             *
             *  Iterable<T> is much like idea of 'abstract readonly container'.
             */
            template    <typename T>
            class  Iterable {
            protected:
                /*
                 * Abstract class used in subclasses which extend the idea of Iterable. Most abstract Containers in Stroika
                 * subclass of Iterable<T>.
                 */
                class  _IRep;

            protected:
                explicit Iterable (const Memory::SharedByValue<_IRep>& rep);

            public:
                Iterable (const Iterable<T>& from);
                ~Iterable ();
            public:
                nonvirtual  Iterable<T>&    operator= (const Iterable<T>& rhs);

            public:
                /*
                 * Create an iterator object which can be used to traverse the 'Iterable' - this object -
                 * and visit each element.
                 */
                nonvirtual Iterator<T>      MakeIterator () const;

            public:
                /*
                 * GetLength () returns the number of elements in this 'Iterable' object. Its defined to be
                 * the same number of elements you would visit if you created an iterator (MakeIterator())
                 * and visited all items. In practice, as the actual number might vary as the underlying
                 * iterable could change while being iterated over.
                 */
                nonvirtual  size_t          GetLength () const;

            public:
                /*
                 * Returns true iff GetLength () == 0
                 */
                nonvirtual  bool    IsEmpty () const;

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
                 *  Take the given function argument, and call it for each element of the Collection. This
                 *  is equivilent to:
                 *
                 *      for (Iterator<T> i = begin (); i != end (); ++i) {
                 *          (doToElement) (*i);
                 *      }
                 *
                 *  However, in threading scenarios, this maybe preferable, since it counts as an atomic
                 *  operation that will happen to each element without other
                 *  threads intervening to modify the container.
                 *
                 *  Also, note that this function does NOT change any elements of the Iterable.
                 */
                nonvirtual  void    Apply (void (*doToElement) (const T& item)) const;

            public:
                /*
                 *   Take the given function argument, and call it for each element of the Collection. This is
                 *   equivilent to:
                 *
                 *      for (Iterator<T> i = begin (); i != end (); ++i) {
                 *          if ((doToElement) (*i)) {
                 *              return it;
                 *          }
                 *      }
                 *      return end();
                 *
                 *   However, in threading scenarios, this maybe preferable, since it counts as an atomic
                 *   operation that will happen to each element without other
                 *   threads intervening to modify the container.
                 *
                 *   This function returns an iteartor pointing to the element that triggered the abrupt loop
                 *   end (for example the element you were searching for?). It returns the specail iterator
                 *   end () to indicate no doToElemet() functions returned true.
                 *
                 *   Also, note that this function does NOT change any elements of the Iterable.
                 */
                nonvirtual  Iterator<T>    ApplyUntilTrue (bool (*doToElement) (const T& item)) const;

            protected:
                Memory::SharedByValue<_IRep>     _fRep;
            };


            template    <typename T>
            class  Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual Iterator<T>     MakeIterator () const                                       =   0;
                virtual size_t          GetLength () const                                          =   0;
                virtual bool            IsEmpty () const                                            =   0;
                virtual  void           Apply (void (*doToElement) (const T& item)) const           =   0;
                virtual  Iterator<T>    ApplyUntilTrue (bool (*doToElement) (const T& item)) const  =   0;

            protected:
                /*
                 * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
                 * actual subclasses.
                 */
                nonvirtual bool         _IsEmpty () const;
                nonvirtual  void        _Apply (void (*doToElement) (const T& item)) const;
                nonvirtual  Iterator<T> _ApplyUntilTrue (bool (*doToElement) (const T& item)) const;
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
#include    "Iterable.inl"
