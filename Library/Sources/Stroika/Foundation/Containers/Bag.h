/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_h_
#define _Stroika_Foundation_Containers_Bag_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
 *
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Seriously consider renaming this file to RandomizedCollection<>. Maybe can
 *              even SUBCLASS from Collection then.
 *
 *              Issue is the Bag name carries lots of 'baggage'. Unclear which bag meaning.
 *              Not a critical issue, but motivating.
 *          &&&&& TODO FIRST &&&&&&&&&
 *              I THINK NEW NAME IS BEST!!!
 *              --LGP 2015-07-16
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  The idea is to mimic that of a black bag (not like SmallTalk Bag<T> which Stroika Collection<> is closest to).
     *
     *  You put stuff in, you iterate, and you take stuff out not knowing about ordering. The ordering (of iteration)
     *  is sticky, but not known (like with a physical bag). So you can 'shake' and that changes the order, or you
     *  can COPY a bag (which preserves order).
     *
     *  This could have been called 'RandomizedCollection'.
     *
     *
            @todo CRITICAL - NEVER ADDED SHAKE METHOD - !!!! THATS THE POINT!
            <<< not su sure this is right. Yes random - but think out and document when the ordering is randomized. When you
            add, or maybe just when you 'shake'. When you add its ARBITRARY where things go (not documented but maybe predictable).
            Call SHAKE to force items to be re-randomized.

            * SOME DOCS ABOVE SAY FIRST APPROACH AND SOME SAY SECOND ... DECIDE

            o   See email 2013-12-27 email dioscission
            o   Maybe call this BlackBox
            o   Shake is what std::random_shuffle does. I'd usually prefer the no-argument version to the stl iterator one. Obviously there are performance issues with some backends,  but as long as you always return a new Collection (or other abstract type) with no promise about the backend it should be okay.

        *
        *  \note Note About Iterators
        *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
        *
        *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
        *          the iterators are automatically updated internally to behave sensibly.
        *
        */
    template <typename T>
    class Bag : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Bag<T>;

    protected:
        class _IRep;

    protected:
        using _SharedPtrIRep = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

    public:
        /**
         */
        Bag ();
        Bag (const Bag& src) = default;
        Bag (Bag&& src)      = default;
        Bag (const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE>
        explicit Bag (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        Bag (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit Bag (const _SharedPtrIRep& rep) noexcept;

#if qDebug
    public:
        ~Bag ();
#endif

    public:
        /**
         */
        nonvirtual Bag<T>& operator= (const Bag<T>& rhs) = default;
        nonvirtual Bag<T>& operator= (Bag<T>&& rhs) = default;

    public:
        /**
         * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (T item) const;

    public:
        /**
         * Add the given item(s) to this Bag<T>. Note - if the given items are already present, another
         * copy will be added. No promises are made about where the added value will appear in iteration.
         */
        nonvirtual void Add (T item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         */
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void AddAll (const CONTAINER_OF_ADDABLE& s);

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
        nonvirtual void Update (const Iterator<T>& i, T newValue);

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
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (T item);
        nonvirtual void Remove (const Iterator<T>& i);

    public:
        /**
         * It is legal to remove something that is not there. This function removes the first instance of item
         * (or each item for the 'items' overload), meaning that another instance of item could still be in the
         * Bag<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
         *
         *  The no-argument verison Produces an empty bag.
         */
        nonvirtual void RemoveAll ();
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& c);

    public:
        /**
         *  Apply the function funciton to each element, and return all the ones for which it was true.
         *
         *  @see Iterable<T>::Where
         */
        nonvirtual Set<T> Where (const function<bool(ArgByValueType<T>)>& doToElement) const;

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief STL-ish alias for Remove ().
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void erase (T item);
        nonvirtual void erase (const Iterator<T>& i);

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
        nonvirtual Bag<T>& operator+= (T item);
        nonvirtual Bag<T>& operator+= (const Iterable<T>& items);

    protected:
        nonvirtual const _IRep& _GetRep () const;
        nonvirtual _IRep& _GetRep ();
    };

    using Traversal::IteratorOwnerID;

    /**
     *  \brief  Implementation detail for Bag<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Bag<T> container API.
     */
    template <typename T>
    class Bag<T>::_IRep : public Iterable<T>::_IRep {
    protected:
        _IRep ();

    public:
        virtual ~_IRep ();

    public:
        virtual void Add (T item)                              = 0;
        virtual void Update (const Iterator<T>& i, T newValue) = 0;
        virtual void Remove (const Iterator<T>& i)             = 0;
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Bag.inl"

#endif /*_Stroika_Foundation_Containers_Bag_h_ */
