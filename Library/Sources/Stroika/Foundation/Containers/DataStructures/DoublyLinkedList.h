/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *      @todo   Major changes to actually support double-links - on 2017-06-06 (old stroika had this not sure how
 *              I messed up in translation to new stroika).
 *
 *              Anyhow - must review the patching and other logic and test a bit. This should be consdiered almost totally untested!
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
 *      @todo   Could add iteartor subclass (or use traits to control) which tracks index internally, as with Stroika v1
 *              but this will do for and maybe best (depending on frequency of calls to CurrentIndex ()
 *
 * Notes:
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
        *      DoublyLinkedList<T> is a generic link (non-intrusive) list implementation.
        *  It keeps a length count so access to its length is rapid. We provide
        *  no public means to access the links themselves.
        *
        *      Since this class provides no patching support, it is not generally
        *  used - more likely you want to use DoublyLinkedList<T>. Use this if you
        *  will manage all patching, or know that none is necessary.
        */
    template <typename T>
    class DoublyLinkedList : public Debug::AssertExternallySynchronizedLock {
    public:
        using value_type = T;

    public:
        DoublyLinkedList ();
        DoublyLinkedList (const DoublyLinkedList<T>& from);
        ~DoublyLinkedList ();

    public:
        nonvirtual DoublyLinkedList<T>& operator= (const DoublyLinkedList<T>& list);

    public:
        class Link;

    public:
        nonvirtual bool IsEmpty () const;

    public:
        nonvirtual size_t GetLength () const;

    public:
        /**
         *  Efficient.
         *
         *  \req not IsEmpty ()
         */
        nonvirtual T GetFirst () const;

    public:
        /**
         *  Efficient:      ?? depedns on if I store last link?
         *
         *  \req not IsEmpty ()
         */
        nonvirtual T GetLast () const;

    public:
        /**
        *  Efficient.
        */
        nonvirtual void Prepend (ArgByValueType<T> item);

    public:
        /**
        *  Efficient.
        */
        nonvirtual void Append (ArgByValueType<T> item);

    public:
        /**
         *  Efficient.
         *
         *  \req not IsEmpty ()
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  Efficient.
         *
         *  \req not IsEmpty ()
         */
        nonvirtual void RemoveLast ();

    public:
        /*
         *  Performance:
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
        template <typename FUNCTION>
        nonvirtual Link* FindFirstThat (FUNCTION doToElement) const;

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Note - does nothing if item not found.
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  Performance:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        class ForwardIterator;

    public:
        /*
         *  Take iteartor 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iteartor from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom);

    public:
        /**
         *  Performance:
         *      Worst Case: O(1)
         */
        nonvirtual void RemoveAt (const ForwardIterator& i);

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
        nonvirtual void Invariant () const;

    protected:
        Link* _fHead{};
        Link* _fTail{};

#if qDebug
    protected:
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
        Link (ArgByValueType<T> item, Link* prev, Link* next);

    public:
        T     fItem;
        Link* fPrev;
        Link* fNext;
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
        ForwardIterator (const ForwardIterator& from);
        ForwardIterator (const DoublyLinkedList<T>* data);

    public:
        using Link = typename DoublyLinkedList<T>::Link;

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator& list);

    public:
        nonvirtual bool Done () const;
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);
        nonvirtual T Current () const;

    public:
        // Warning - intrinsically slow
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual void SetCurrentLink (Link* l);

    public:
        nonvirtual bool Equals (const typename DoublyLinkedList<T>::ForwardIterator& rhs) const;

    public:
        nonvirtual void Invariant () const;

    protected:
    public: /// TEMPORARILY MAKE PUBLIC SO ACCESSIBLE IN ``<> - until those cleaned up a bit
        const DoublyLinkedList<T>* _fData;
        const Link*                _fCurrent;
        bool                       _fSuppressMore; // Indicates if More should do anything, or if were already Mored...

#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif

    private:
        friend class DoublyLinkedList<T>;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_ */
