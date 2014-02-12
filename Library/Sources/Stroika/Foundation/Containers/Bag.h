/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_h_
#define _Stroika_Foundation_Containers_Bag_h_   1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"



/**
 *  \file
 *
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             */
            template    <typename T>
            class   Bag : public Iterable<T> {
            private:
                using   inherited   =   Iterable<T>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   Bag<T>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep              =   shared_ptr<_IRep>;

            public:
                Bag ();
                Bag (const Bag<T>& src);
                Bag (const std::initializer_list<T>& src);
                template <typename CONTAINER_OF_T>
                explicit Bag (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Bag (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Bag (const _SharedPtrIRep& rep);

#if     qDebug
            public:
                ~Bag ();
#endif
            public:
                nonvirtual  Bag<T>& operator= (const Bag<T>& rhs) = default;

            public:
                /**
                 * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 * Add the given item(s) to this Bag<T>. Note - if the given items are already present, another
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
                 *      @todo DOCUMENT SEMANTICS MORE CLEARLY - THINKING THROUGH SUBCLASS SORTEDBag - DEFINE SO STILL MAKES SENSE THERE!!!
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
                 * Bag<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
                 *
                 * SECOND OVERLOAD:
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Bag<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
                 *
                 *  The no-argument verison Produces an empty bag.
                 */
                nonvirtual  void    RemoveAll ();
                template    <typename COPY_FROM_ITERATOR_OF_T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& c);

            public:
                /**
                 *  \em EXPERIMENTAL API (2014-01-08)
                 *
                 *  Apply the function funciton to each element, and return all the ones for which it was true.
                 */
                nonvirtual  Bag<T>    EachWith (const function<bool(const T& item)>& doToElement) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 * \brief STL-ish alias for Remove ().
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  void    erase (T item);
                nonvirtual  void    erase (const Iterator<T>& i);

            public:
                /**
                 *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Bag<T>& operator+= (T item);
                nonvirtual  Bag<T>& operator+= (const Iterable<T>& items);

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Bag<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Bag<T> container API.
             */
            template    <typename T>
            class   Bag<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void    Add (T item)                                                        =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)                           =   0;
                virtual void    Remove (const Iterator<T>& i)                                       =   0;
                virtual void    RemoveAll ()                                                        =   0;
#if     qDebug
                virtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)     =   0;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Bag.inl"

#endif  /*_Stroika_Foundation_Containers_Bag_h_ */
