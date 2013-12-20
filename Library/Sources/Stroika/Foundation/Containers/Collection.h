/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_h_
#define _Stroika_Foundation_Containers_Collection_h_   1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   Collection_DefaultTraits {
                /**
                 *  @todo DOCUMENT WHAT GOES HERE. CAN A LAMBDA GO HERE? HOW TODO WITH LAMBDAS?
                 *          MAYBE to use a lamnda - use soemthing LIKE? Not quite - maybe ???.... THINK OUT
                 *          typedef std::function<int(T,T)> CompareFunctionType;
                 *
                 *          (working on this as of 2013-07-03 in Comparer.h code - though alot todo here as well)
                 *
                 *  NB: NOT USED YET - JUST PLAYING WITH HOW WE WANT TODO THIS...
                 *
                 *  (TODO - DOC WHAT THIS MEANS - CUZ IT APPEARS TO BE ACTUALLY REQUIRED - cuz virtual methods. Maybe shouldbt but hard to
                 *      AVOID FOR this class. Maybe should have more abstract thing - or maybe should ahve this be such - so
                 *      these things not implementated virtually - and collection can eb something that doesnt require operator== or any comparer???
                 *      @todo - think about above!!! And clarify these docs
                 *      @todo DID GOOD JOB OF THIS WITH SEQUENCE. DECIDE IF BAG ALWAYS REQUIRES EQUALS!!!
                 *
                 *  Note - this comparer is NOT required - and not actually used except for certain specific methods:
                 *      o   Bag<T, TRAITS>::Contains (T)
                 *      o   Bag<T, TRAITS>::Remove (T)
                 *      o   Bag<T, TRAITS>::UniqueElements ()
                 *      o   Bag<T, TRAITS>::TallyOf()
                 *      o   Bag<T, TRAITS>::Equals()
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
            };


            /**
             *  \brief  A Collection<T, TRAITS> is a container to manage an un-ordered collection of items.
             *
             *  A Collection<T, TRAITS> is a container pattern to manage an un-ordered collection of items.
             *  This is both an abstract interface, but the Collection<T, TRAITS> class it actually concrete because
             *  it automatically binds to a default implementation.
             *
             *  A Collection<T, TRAITS> is the simplest kind of collection. It allows addition and
             *  removal of elements, but makes no guarantees about element ordering. Two
             *  collections are considered equal if they contain the same items, even if iteration
             *  order is different.
             *
             *  \em Performance
             *      Collections are typically designed to optimize item addition and iteration.
             *      They are fairly slow at item access (as they have no keys). Removing items
             *      is usually slow, except in the context of an Iterator<T>, where it is usually
             *      very fast. Collection comparison (operator==) is often very slow in the worst
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
             *  @TODO BELOW REWUIRMENT WRONG - BUT CLARIFY!!!
             * \req RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, T);
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename T, typename TRAITS = Collection_DefaultTraits<T>>
            class   Collection : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Collection<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;

            public:
                Collection ();
                Collection (const Collection<T, TRAITS>& b);
                Collection (const std::initializer_list<T>& b);
                template <typename CONTAINER_OF_T>
                explicit Collection (const CONTAINER_OF_T& b);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Collection (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Collection (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Collection<T, TRAITS>& operator= (const Collection<T, TRAITS>& rhs);

            public:
                /**
                 * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 * Add the given item(s) to this Collection<T,TRAITS>. Note - if the given items are already present, another
                 * copy will be added. No promises are made about where the added value will appear in iteration.
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
                 *
                 *      @todo DOCUMENT SEMANTICS MORE CLEARLY - THINKING THROUGH SUBCLASS SORTEDCollection - DEFINE SO STILL MAKES SENSE THERE!!!
                 *      LIKE EQUIV TO REMOVE(i) and ADD newValue - but more efficient? No - maybe just does remove(i) and add (newValue?))
                 *      and document promised semantics about if you will encounter newvalue again when you continue iterating!
                 *
                 *      MAYBE best answer is to LOSE this Update() method for bag<> - useful for Sequence<> - but maybe not here!
                 */
                nonvirtual  void    Update (const Iterator<T>& i, T newValue);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Collection<T, TRAITS> after the remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
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
                 * Collection<T, TRAITS> after the remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
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
                 *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T, TRAITS> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Collection<T, TRAITS>& operator+= (T item);
                nonvirtual  Collection<T, TRAITS>& operator+= (const Collection<T, TRAITS>& items);


            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Collection<T,TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Collection<T,TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Collection<T, TRAITS>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void    Add (T item)                                =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)   =   0;
                virtual void    Remove (T item)                             =   0;
                virtual void    Remove (const Iterator<T>& i)               =   0;
                virtual void    RemoveAll ()                                =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Collection.inl"

#endif  /*_Stroika_Foundation_Containers_Collection_h_ */
