/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_LinkedList_h_

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/TypeHints.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Memory/BlockAllocated.h"

#include "../Common.h"

/**
 *
 *
 *      LinkedList<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *
 * TODO:
 *
 *      @todo   Lose protected stuff, and if practical, make Link private
 *
 *      @todo   In ForwardIterator object - maintain cached prev - so as we navigate - we can often
 *              avoid the back nav. Maybe make this a configurable class option? Anyhow - mostly include
 *              and use as cahce. ALready there mostly - but commented out (fPrev)
 *
 *      @todo   Include Performance numbers for each operation (done for many).
 *
 * Notes:
 *
 *      Slightly unusual behaviour for LinkedListMutator_Patch<T>::AddBefore () -
 *  allow it to be called when we are Done() - otherwise there is NO
 *  WAY TO APPEND TO A LINK LIST!!!
 *
 *
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
     *      LinkedList<T,TRAITS> is a generic link (non-intrusive) list implementation.
     *  It keeps a length count so access to its length is rapid. We provide
     *  no public means to access the links themselves.
     *
     *      Since this class provides no patching support, it is not generally
     *  used - more likely you want to use PatchingDataStructures::LinkedList<T>. Use this if you
     *  will manage all patching, or know that none is necessary.
     */
    template <typename T>
    class LinkedList : public Debug::AssertExternallySynchronizedLock {
    public:
        using value_type = T;

    public:
        LinkedList ();
        LinkedList (const LinkedList& from);
        ~LinkedList ();

    public:
        nonvirtual LinkedList<T>& operator= (const LinkedList<T>& list);

    public:
        class ForwardIterator;

    public:
        class Link;

    public:
        /*
         *  Take iteartor 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iteartor from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList* movedFrom);

    public:
        /**
         *  \note Performance:
         *      Always: O(1)
         */
        nonvirtual bool IsEmpty () const;

    public:
        /**
         *  \note Performance:
         *      Always: O(N)
         */
        nonvirtual size_t GetLength () const;

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         *      Average Case: O(1)
         */
        nonvirtual T GetFirst () const;

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         *      Average Case: O(1)
         */
        nonvirtual void Prepend (ArgByValueType<T> item);

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         *      Average Case: O(1)
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Returns pointer to T (or nullptr if not found). Lifetime of T* only til next call on this.
         *
         *  Contains(T item) == Lookup (item) != nullptr;
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual const T* Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {}) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual T* Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});

    public:
        /**
         */
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION doToElement) const;

    public:
        template <typename FUNCTION>
        nonvirtual Link* FindFirstThat (FUNCTION doToElement) const;

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         * 
         *  Returns iterator pointing at next item.
         */
        nonvirtual ForwardIterator RemoveAt (const ForwardIterator& i);

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         *
         *  NB: Can be called if done
         */
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  Note - does nothing if item not found.
         */
        template <typename EQUALS_COMPARER>
        nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer);

    public:
        nonvirtual void RemoveAll ();

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual void Append (ArgByValueType<T> item);

    public:
        /*
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /*
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual void SetAt (T item, size_t i);

    public:
        nonvirtual void Invariant () const;

    private:
        Link* _fHead{nullptr};

#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif

    private:
        friend class ForwardIterator;
    };

    template <typename T>
    class LinkedList<T>::Link : public Memory::UseBlockAllocationIfAppropriate<Link> {
    public:
        Link () = delete;
        Link (T item, Link* next);
        Link (const Link&) = delete;

    public:
        T     fItem;
        Link* fNext{nullptr};
    };

    /*
     *      ForwardIterator<T> allows you to iterate over a LinkedList<T,TRAITS>. Its API
     *  is designed to make easy implementations of subclasses of IteratorRep<T>.
     *  It is unpatched - use LinkedListIterator_Patch<T> or LinkedListMutator_Patch<T>
     *  for that.
     */
    template <typename T>
    class LinkedList<T>::ForwardIterator {
    public:
        ForwardIterator (const ForwardIterator& from) = default;
        ForwardIterator (const LinkedList* data);

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator& it);

    public:
        nonvirtual bool Done () const;
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        nonvirtual T Current () const;

    public:
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual const Link* GetCurrentLink () const;

    public:
        nonvirtual void SetCurrentLink (const Link* l);

    public:
        nonvirtual bool Equals (const typename LinkedList<T>::ForwardIterator& rhs) const;

    public:
        nonvirtual void PatchBeforeRemove (const ForwardIterator* adjustmentAt);

    public:
        nonvirtual void Invariant () const;

    protected:
        const LinkedList<T>*                _fData;
        const typename LinkedList<T>::Link* _fCurrent;

    protected:
#if qDebug
        virtual void Invariant_ () const;
#endif

    private:
        friend class LinkedList<T>;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_LinkedList_h_ */
