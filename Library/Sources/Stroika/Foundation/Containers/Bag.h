/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_h_
#define _Stroika_Foundation_Containers_Bag_h_   1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"
#include    "Iterator.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   ADD EQUALS_COMPARER .. and make Bag<T> depend on THAT! We ONLY require == for
 *              Bag.
 *              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!PRIORITY - NEXT !!!!!
 *              PUT RequireElementTraitsInClass etc in DEFUALT TRAITS OBJECT - NOT in class iteself (unless needed)
 *
 *
 *      @todo   Cleaup and actually use properly the TRAITS code. Not actually used yet.
 *
 *      @todo   Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *      @todo   Have Bag_Difference/Union/Interesection??? methods/?? Do research....
 *
 *      @todo   implement UniqueElements() and create test cases.
 *
 *      @todo   Add more efficent-for-tally implementation of bag (like multimap?). Low priority since you can
 *              always use a Tally<T>...
 *
 *      @todo   Consider adding RetainAll (Set<T>) API - like in Collection.h, and Java. Key diff is was force
 *              use of SET as arg - not another Bag? Or maybe overload with different container types as args?
 *              COULD do 2 versions - one with Iterable<T> and one with Set<T>. trick is to get definition
 *              to work without untoward dependencies between set and bag code? I think that means
 *              most of the check impl needs to be in the envelope to avoid always building it through vtables.
 *
 *      @todo   Consider adding smarter implementation with keys - some btree indexing impl - to make stuff that
 *              looks up by value quicker.
 *
 *      @todo   Add Shake() method, which MAY randomize the ordering of items. Note - since ordering is not
 *              defined, this may do nothing, but will often randomize order. Often handy as a testing tool.
 *
 *              NO - Probably not. One - this restricts use to backends capable of this randomizing of order (eg. not hashtables
 *              or trees), and is incompatible with the idea of subtypes like  SortedBag<T>. Instead - see if I can
 *              come up with a compromise - maybe a "Shake" method that produces a NEW BAG of a possibly different backend TYPE!
 *              THAT can work!
 *
 *      @todo   Document the performance characteristics of the various Tally<T>::Equals() implementations.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   Bag_DefaultTraits {
                /**
                 *  @todo DOCUMENT WHAT GOES HERE. CAN A LAMBDA GO HERE? HOW TODO WITH LAMBDAS?
                 *          MAYBE to use a lamnda - use soemthing LIKE? Not quite - maybe ???.... THINK OUT
                 *          typedef std::function<int(T,T)> CompareFunctionType;
                 *
                 *  NB: NOT USED YET - JUST PLAYING WITH HOW WE WANT TODO THIS...
                 *
                 *  Note - this comparer is NOT required - and not actually used except for certain specific methods:
                 *      o   Bag<T, BAG_TRAITS>::Contains (T)
                 *      o   Bag<T, BAG_TRAITS>::Remove (T)
                 *      o   Bag<T, BAG_TRAITS>::UniqueElements ()
                 *      o   Bag<T, BAG_TRAITS>::TallyOf()
                 *      o   Bag<T, BAG_TRAITS>::Equals()
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;
            };


            /**
             *  \brief  A Bag<T,BAG_TRAITS> is a container to manage an un-ordered collection of items.
             *
             *  A Bag<T,BAG_TRAITS> is a container pattern to manage an un-ordered collection of items.
             *  This is both an abstract interface, but the Bag<T,BAG_TRAITS> class it actually concrete because
             *  it automatically binds to a default implementation.
             *
             *  A Bag<T,BAG_TRAITS> is the simplest kind of collection. It allows addition and
             *  removal of elements, but makes no guarantees about element ordering. Two
             *  bags are considered equal if they contain the same items, even if iteration
             *  order is different.
             *
             *  \em Performance
             *      Bags are typically designed to optimize item addition and iteration.
             *      They are fairly slow at item access (as they have no keys). Removing items
             *      is usually slow, except in the context of an Iterator<T>, where it is usually
             *      very fast. Bag comparison (operator==) is often very slow in the worst
             *      case (n^2) and this worst case is the relatively common case of identical
             *      bags.
             *
             *  Although Bag has an TallyOf () method, it is nonvirtual, and therefore
             *  not optimized for the various backends. There is a separate class, Tally,
             *  for cases where you are primarily interested in keeping an summary count
             *  of the occurences of each item.
             *
             *  Bags allow calls to Remove with an item not contained within the bag.
             *
             *  As syntactic sugar, using either functional (Add, Remove) or
             *  operator (+,-) is allowed.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename T, typename BAG_TRAITS = Bag_DefaultTraits<T>>
            class   Bag : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                Bag ();
                Bag (const Bag<T, BAG_TRAITS>& b);
                template <typename CONTAINER_OF_T>
                explicit Bag (const CONTAINER_OF_T& b);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Bag (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Bag (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Bag<T, BAG_TRAITS>& operator= (const Bag<T, BAG_TRAITS>& rhs);

            public:
                /**
                 * \brief Compares items with BAG_TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 * Add the given item(s) to this Bag<T,BAG_TRAITS>. Note - if the given items are already present, another
                 * copy will be added.
                 */
                nonvirtual  void    Add (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
                 */
                nonvirtual  void    Update (const Iterator<T>& i, T newValue);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Bag<T,BAG_TRAITS> after the remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
                 *
                 * SECOND OVERLOAD:
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Bag<T,BAG_TRAITS> after the remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
                 *
                 *  The no-argument verison Produces an empty bag.
                 */
                nonvirtual  void    RemoveAll ();
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& c);

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();


            public:
                /**
                 * NYI - but this can be remove duplicates. So you can say
                 *      for (auto i : bag) {} OR
                 *      for (auto i : bag.UniqueElements ()) {}
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /*
                 *  Two Bags are considered equal if they contain the same elements (by comparing them with operator==) with the same count.
                 *  In short, they are equal if TallyOf() each item in the LHS equals the TallyOf() the same item in the RHS.
                 *
                 *  Equals is commutative().
                 */
                nonvirtual  bool    Equals (const Bag<T, BAG_TRAITS>& rhs) const;

            public:
                /**
                 *  Since items can appear more than once, this function traverses the bag and returns the
                 *  count of times the given item appears. Note that TallyOf (T) returns zero if the item
                 *  is not present. For TallyOf (T), the BAG_TRAITS::EqualsCompareFunctionType::Equals() function
                 *  is used to compare.
                 */
                nonvirtual  size_t  TallyOf (const Iterator<T>& i) const;
                nonvirtual  size_t  TallyOf (T item) const;

            public:
                /**
                 *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T,BAG_TRAITS> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Bag<T, BAG_TRAITS>& operator+= (T item);
                nonvirtual  Bag<T, BAG_TRAITS>& operator+= (const Bag<T, BAG_TRAITS>& items);

            public:
                /**
                 *  operator- is syntactic sugar on Remove() or RemoveAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T,BAG_TRAITS> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Bag<T, BAG_TRAITS>& operator-= (T item);
                nonvirtual  Bag<T, BAG_TRAITS>& operator-= (const Bag<T, BAG_TRAITS>& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Bag<T, BAG_TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Bag<T, BAG_TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Bag<T,BAG_TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Bag<T,BAG_TRAITS> container API.
             */
            template    <typename T, typename BAG_TRAITS>
            class   Bag<T, BAG_TRAITS>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool    Equals (const _IRep& rhs) const             =   0;
                virtual bool    Contains (T item) const                     =   0;
                virtual size_t  TallyOf (T item) const                      =   0;
                virtual void    Add (T item)                                =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)   =   0;
                virtual void    Remove (T item)                             =   0;
                virtual void    Remove (const Iterator<T>& i)               =   0;
                virtual void    RemoveAll ()                                =   0;

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            protected:
                nonvirtual bool    _Equals_Reference_Implementation (const _IRep& rhs) const;
                nonvirtual size_t  _TallyOf_Reference_Implementation (T item) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Bag.inl"

#endif  /*_Stroika_Foundation_Containers_Bag_h_ */
