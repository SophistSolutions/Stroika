/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/TypeHints.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Memory/BlockAllocated.h"

#include "../Common.h"

/*
 *
 *  Description:
 *
 *      DoublyLinkedList<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *
 * TODO:
 *      @todo   Lose protected stuff, and if practical, make Link private
 *
 *      @todo   DataStructures::DoublyLinkedList::ForwardIterator has the 'suporesMode' in the
 *              datastrcutre code and we have it here in the patching code. Note SURE what is better
 *              probably patching code) - but make them consistent!
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Major cleanup needed - not doubly-linked list. Look at old Stroika code. Somehow the double link
 *              part got lost.
 *
 *  Long-Term TODO:
 *      @todo   Could add iterator subclass (or use traits to control) which tracks index internally, as with Stroika v1
 *              but this will do for and maybe best (depending on frequency of calls to CurrentIndex ()
 *
 * Notes:
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
     *      DoublyLinkedList<T> is a generic link (non-intrusive) list implementation.
     *  We provide no public means to access the links themselves.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class DoublyLinkedList : public Debug::AssertExternallySynchronizedLock {
    public:
        using value_type = T;

    public:
        DoublyLinkedList ();
        DoublyLinkedList (const DoublyLinkedList& from);
        ~DoublyLinkedList ();

    public:
        nonvirtual DoublyLinkedList& operator= (const DoublyLinkedList& list);

    public:
        class Link;

    public:
        nonvirtual bool IsEmpty () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        nonvirtual size_t GetLength () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         *
         *  \req not IsEmpty ()
         */
        nonvirtual T GetFirst () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         *
         *
         *  \req not IsEmpty ()
         */
        nonvirtual T GetLast () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         */
        nonvirtual void Prepend (ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         */
        nonvirtual void Append (ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         *
         *  \req not IsEmpty ()
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  \note Complexity:
         *      Always: O(0)
         *
         *  \req not IsEmpty ()
         */
        nonvirtual void RemoveLast ();

    public:
        /*
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Utility to search the list for the given item using EQUALS_COMPARER
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {}) const;

    public:
        /**
         */
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION doToElement) const;

    public:
        /**
         */
        template <typename FUNCTION>
        nonvirtual Link* FindFirstThat (FUNCTION doToElement) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Note - does nothing if item not found.
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        class ForwardIterator;

    public:
        /*
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom) const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         * 
         *  returns the next link
         */
        nonvirtual ForwardIterator RemoveAt (const ForwardIterator& i);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         *
         *  NB: Can be called if done
         */
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        nonvirtual void Invariant () const;

    private:
        Link* _fHead{};
        Link* _fTail{};

#if qDebug
    private:
        virtual void Invariant_ () const;
#endif

    private:
        friend class ForwardIterator;
    };

    /**
     *  Just an implementation detail. Don't use directly except in helper classes.
     */
    template <typename T>
    class DoublyLinkedList<T>::Link : public Memory::UseBlockAllocationIfAppropriate<Link> {
    public:
        Link ()            = delete;
        Link (const Link&) = delete;
        Link (ArgByValueType<T> item, Link* prev, Link* next);

    public:
        T     fItem;
        Link* fPrev{nullptr};
        Link* fNext{nullptr};
    };

    /**
     *      ForwardIterator<T> allows you to iterate over a DoublyLinkedList<T>. Its API
     *  is designed to make easy implementations of subclasses of IteratorRep<T>.
     *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListIterator_Patch<T>
     *  for that.
     */
    template <typename T>
    class DoublyLinkedList<T>::ForwardIterator {
    public:
        ForwardIterator (const ForwardIterator& from) = default;
        ForwardIterator (const DoublyLinkedList* data);

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator& list);

    public:
        nonvirtual bool Done () const;
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);
        nonvirtual T    Current () const;
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        // Warning - intrinsically slow
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual const Link* GetCurrentLink () const
        {
            return _fCurrent;
        }

    public:
        nonvirtual void SetCurrentLink (const Link* l);

    public:
        nonvirtual bool Equals (const typename DoublyLinkedList<T>::ForwardIterator& rhs) const;

    public:
        nonvirtual void PatchBeforeRemove (const ForwardIterator* adjustmentAt);

    public:
        nonvirtual void Invariant () const;

    private:
        const DoublyLinkedList* _fData{nullptr};
        const Link*             _fCurrent{nullptr};

#if qDebug
    private:
        virtual void Invariant_ () const;
#endif

    private:
        friend class DoublyLinkedList;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_ */
