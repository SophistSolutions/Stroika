/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterable_h_
#define _Stroika_Foundation_Containers_Iterable_h_  1

/**
 *  \file
 *
 *  TODO:
 *
 *
 *      @todo   since Iterator<T> now uses iterator<> traits stuff, so should Iteratable<T>
 *
 *      @todo   Consider renaming Iterable<T> to Collection<T>. Or at least document
 *              why not...
 *
 *      @todo   Crazy temphack cuz current code assumes you must call++ before starting iteration! Crazy!
 *              Issue is way we implemented the 'CURRENT' stuff with iterators - filling in after teh first
 *              More()...
 *
 *      @todo   Consider adding class TWithCompareEquals<T> to add Iterable<T> like functions - where we can count on "T".
 *              Perhaps implement with a Require (TWithCompareEquals<T>) in CTORs for class?
 *
 *      @todo   Apply/ApplyUntilTrue() should also take overload with function object (STL). Also,
 *              consider providing a _IRep version - to implement LOCKING logic promised in the API. Make sure
 *              this API works fully with lambdas - even bound...
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"


/**
 *  Empirically, std::function seems to use way more code space (and must evaluate rest)
 *  than the qAPPLY_IMPL_STRATEGY_COOKIE strategy, which is functionally equivilent from
 *  the point of view of the public use API.
 */
#define qAPPLY_IMPL_STRATEGY_STDFUNCTION    1
#define qAPPLY_IMPL_STRATEGY_COOKIE         2
#ifndef qAPPLY_IMPL_STRATEGY
#define qAPPLY_IMPL_STRATEGY                qAPPLY_IMPL_STRATEGY_COOKIE
#endif // !qAPPLY_IMPL_STRATEGY



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            /**
             *  \brief  Iterable<T> is a base class for containers which easily produce an Iterator<T>
             *          to traverse them.
             *
             *  Iterable<T> is a base class for containers which easily produce an Iterator<T> to traverse them.
             *
             *  The Stroika iterators can be used either directly, or in the STL begin/end style - and this
             *  class supports both styles of usage.
             *
             *  Iterable<T> also supports read-only applicative operations on the contained data.
             *
             *  Iterable<T> is much like idea of 'abstract readonly container', but which only supports an exceedingly simplistic pattern
             *  of access.
             *
             *  *Important Design Note*:
             *
             *      We have no:
             *          nonvirtual  void    _SetRep (_SharedByValueRepType rep);
             *
             *      because allowing a _SetRep() method would complicate the efforts of subclasses of Iterable<T>
             *      to assure that the underlying type is of the appropriate subtype.
             *
             *      For example - see Bag_Array<T>::GetRep_().
             */
            template    <typename T>
            class  Iterable {
            public:
                /**
                 *      \brief  ElementType is just a handly copy of the *T* template type which this
                 *              Iterable<T> parameterizes access to.
                 */
                typedef T   ElementType;

            protected:
                class  _IRep;

            private:
                struct  Rep_Cloner_ {
                    inline  static  shared_ptr<_IRep>  Copy (const _IRep& t) {
                        return Iterable<T>::Clone_ (t);
                    }
                };

            protected:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored by users).
                 */
                typedef Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, Rep_Cloner_>>   _SharedByValueRepType;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                Iterable (const Iterable<T>& from);

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit Iterable (const _SharedByValueRepType& rep);

            public:
                ~Iterable ();

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                nonvirtual  Iterable<T>&    operator= (const Iterable<T>& rhs);

            public:
                /**
                 * \brief Create an iterator object which can be used to traverse the 'Iterable'.
                 *
                 * Create an iterator object which can be used to traverse the 'Iterable' - this object -
                 * and visit each element.
                 */
                nonvirtual Iterator<T>      MakeIterator () const;

            public:
                /**
                 * \brief Returns the number of items contained.
                 *
                 * GetLength () returns the number of elements in this 'Iterable' object. Its defined to be
                 * the same number of elements you would visit if you created an iterator (MakeIterator())
                 * and visited all items. In practice, as the actual number might vary as the underlying
                 * iterable could change while being iterated over.
                 */
                nonvirtual  size_t          GetLength () const;

            public:
                /**
                 * \brief Returns true iff GetLength() == 0
                 */
                nonvirtual  bool    IsEmpty () const;

            public:
                /**
                 * \brief STL-ish alias for IsEmpty()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  length () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  size () const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 */
                nonvirtual  Iterator<T> begin () const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 */
                static Iterator<T> end ();

            public:
                /**
                 *  \brief  Run the argument function (or lambda) on each element of the container.
                 *
                 *  Take the given function argument, and call it for each element of the container. This
                 *  is equivalent to:
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
                 *
                 *  \pre    doToElement != nullptr
                 */
                nonvirtual  void    Apply (const std::function<void(const T& item)>& doToElement) const;
                nonvirtual  void    ApplyStatic (void (*doToElement) (const T& item)) const;

            public:
                /**
                 *  \brief  Run the argument bool-returning function (or lambda) on each element of the
                 *          container, and return an iterator pointing at the first element found true.
                 *
                 *  Take the given function argument, and call it for each element of the container. This is
                 *  equivalent to:
                 *
                 *      for (Iterator<T> i = begin (); i != end (); ++i) {
                 *          if ((doToElement) (*i)) {
                 *              return it;
                 *          }
                 *      }
                 *      return end();
                 *
                 *  However, in threading scenarios, this maybe preferable, since it counts as an atomic
                 *  operation that will happen to each element without other
                 *  threads intervening to modify the container.
                 *
                 *  This function returns an iteartor pointing to the element that triggered the abrupt loop
                 *  end (for example the element you were searching for?). It returns the specail iterator
                 *  end() to indicate no doToElement() functions returned true.
                 *
                 *   Also, note that this function does NOT change any elements of the Iterable.
                 */
                nonvirtual  Iterator<T>    ApplyUntilTrue (bool (*doToElement) (const T& item)) const;


            protected:
                nonvirtual  typename Iterable<T>::_IRep&         _GetRep ();
                nonvirtual  const typename Iterable<T>::_IRep&   _GetRep () const;

            private:
                static  shared_ptr<_IRep>  Clone_ (const _IRep& rep);

            private:
                _SharedByValueRepType    fRep_;
            };



            /**
             *  \brief  Implementation detail for iterator implementors.
             *
             * Abstract class used in subclasses which extend the idea of Iterable. Most abstract Containers in Stroika
             * subclass of Iterable<T>.
             */
            template    <typename T>
            class  Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                typedef const std::function<void(const T& item)>&   _APPLY_ARGTYPE;
                typedef const std::function<bool(const T& item)>&   _APPLYUNTIL_ARGTYPE;
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                typedef pair<const void*, void (*) (const void* cookie, const T& item)> _APPLY_ARGTYPE;
                typedef pair<const void*, bool (*) (const void* cookie, const T& item)> _APPLYUNTIL_ARGTYPE;
#endif

            public:
                virtual shared_ptr<_IRep>          Clone () const                                   =   0;
                virtual Iterator<T>     MakeIterator () const                                       =   0;
                virtual size_t          GetLength () const                                          =   0;
                virtual bool            IsEmpty () const                                            =   0;
                virtual void            Apply (_APPLY_ARGTYPE doToElement) const                    =   0;
                virtual Iterator<T>     ApplyUntilTrue (_APPLYUNTIL_ARGTYPE) const                  =   0;

            protected:
                /*
                 * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
                 * actual subclasses.
                 */
                nonvirtual  bool        _IsEmpty () const;
                nonvirtual  void        _Apply (_APPLY_ARGTYPE doToElement) const;
                nonvirtual  Iterator<T> _ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const;
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
