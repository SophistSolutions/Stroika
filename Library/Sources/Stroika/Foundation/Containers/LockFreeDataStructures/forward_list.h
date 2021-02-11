/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <cassert>
#include <list>
#include <memory>
#include <vector>

/**
 *  Initial implementation copied from (so full credit for algorithms)
 *      https://codereview.stackexchange.com/questions/167252/c11-lock-free-collection-similar-to-stdforward-list-follow-up-2
 *      APPENTLY written by https://codereview.stackexchange.com/users/62314/brent
 *      All said about license we MIT licence, which is what Stroika uses, so I assume will be OK.
 * 
 * 
NOTES FROM ORIGINAL POST:

This question is a new iteration of this question:

C++11 lock free collection similar to std::forward_list - follow-up

all methods are thread safe, baring the destructor
push, insert, emplace, pop, erase, and iterator increment/dereference are O(1)
locks that do occur are per-element spin_ locks
push, insert, emplace, clear, and separate operations are lock free.
pop and erase are not lock free.
begin, cbegin, and incrementing iterators are not lock free
dereferencing iterators is lock free
uses reference counting to preserve iterators (and values) of removed elements
iterators of removed elements will increment to end()
insert_after or emplace_after on a removed iterator will return end() to indicate failure.
In addition to the creative commons license for code on this site, you may also use it under the MIT license.

forward_list.hpp

 */

namespace Stroika::Foundation::Containers::LockFreeDataStructures {
    template <typename T>
    class forward_list;
}
namespace std {
    template <typename T>
    void swap (Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& a, Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& b) noexcept;
}


namespace Stroika::Foundation::Containers::LockFreeDataStructures {

    /**
     * similar to std::forward_list, but thread safe and partiallt lock free
     */
    template <typename T>
    class forward_list {
    private:
        class node_;

    public:
    private:
        template <class U>
        class ForwardIterator;

    public:
        typedef T                        value_type;
        typedef value_type&              reference;
        typedef const value_type&        const_reference;
        typedef value_type*              pointer;
        typedef value_type const*        const_pointer;
        typedef ForwardIterator<T>       iterator;
        typedef ForwardIterator<const T> const_iterator;

    public:
        /**
         */
        forward_list ();
        forward_list (const forward_list& src);
        forward_list (forward_list&& src) noexcept;

    public:
        ~forward_list ();

    public:
        /**
         *
         * lock free
         */
        nonvirtual bool empty () const;

    public:
        /**
         * The first node_ is removed before locking the other nodes.
         *  Push will not block.
         *  An iterator incremented externally may move to a value no longer in the list
         *
         *  \note UNLIKE https://en.cppreference.com/w/cpp/container/forward_list/clear, this method returns the number of elements deleted
         *
         * lock free
         */
        nonvirtual int clear ();

    public:
        /**
         *   **NOT A METHOD OF std::forward_list<>
        // NOT lock free
        // All nodes are locked before removing the first element.
        // Incrementing an iterator will block, and then result in the end() iterator
         */
        nonvirtual int locked_clear ();

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/front
        // <ORIGINAL DOCS SAID 'lock free' - but it calls begin () which orignal docs say NOT lock-free, so unclear but probably NOT lockfree>
         */
        nonvirtual reference       front ();
        nonvirtual const_reference front () const;

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/push_front
         *  but returns iterator pointing to element just added
         *
         * lock free
         */
        nonvirtual iterator push_front (const T& value);
        nonvirtual iterator push_front (T&& value);

    public:
        /**
         * https://en.cppreference.com/w/cpp/container/forward_list/emplace_front
        // lock free
         */
        template <class... U>
        nonvirtual reference emplace_front (U&&... params);

    public:
        /**
         @ see https://en.cppreference.com/w/cpp/container/forward_list/pop_front
         UNLIKE std version, this returns true iff it removed something 
         * **not** lock free
         */
        nonvirtual bool pop_front (T* value);

    public:
        /**
         * **not** lock free
         */
        iterator begin ();
        nonvirtual const_iterator begin () const;

    public:
        /**
         * **not** lock free
         */
        nonvirtual const_iterator cbegin () const;

    public:
        /**
         * lock free
         */
        iterator       end ();
        nonvirtual const_iterator end () const;

    public:
        /**
         * lock free
         */
        nonvirtual const_iterator cend () const;

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/insert_after
         *  All overloads lock_free
         *      << was comented only on first overload but appears to apply to all) returns a default constructed iterator if position is no longer valid
         */
        nonvirtual iterator insert_after (const_iterator position, T const& value);
        nonvirtual iterator  insert_after (const_iterator position, T&& value);
        nonvirtual iterator insert_after (const_iterator pos, int count, const T& value);
        template <class InputIt>
        nonvirtual iterator insert_after (const_iterator pos, InputIt first, InputIt last);
        nonvirtual iterator insert_after (const_iterator pos, std::initializer_list<T> ilist);

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/emplace_after
        // lock free
         */
        template <class... U>
        nonvirtual iterator emplace_after (const_iterator position, U&&... params);

    public:
        /**
         *  NOT standard forward_list<> funciton but something the original author found useful.
        // lock free
        // all the elements after position are moved to a new forward_list
        // IMPORTANT: existing iterators will still traverse the separated portion if already within the separated portion
         */
        nonvirtual bool separate_after (const_iterator position, forward_list<T>*& result);

    public:
        // NOT lock free
        nonvirtual bool erase_after (const_iterator position, T* value);

    public:
        // NOT lock free on a, lock free on b
        friend void std::swap (forward_list& a, forward_list& b) noexcept;

    private:
        std::atomic<node_*> fFirst_;

        // (LGP APPEARS TO) return iterator point at element just added
        // lock free
        static iterator insert_node_ (std::atomic<node_*>& atomic_ptr, node_* n);

        // lock free, removes all the nodes from *atomic_ptr forward, and returns that node_ with links still intact
        static node_* seperate_ (std::atomic<node_*>& atomic_ptr);

        // NOT lock free
        // returns true iff it removed something
        static bool remove_node_ (std::atomic<node_*>& atomic_ptr, T* value);

        static node_* terminal_ ();
        static node_* spin_ ();

        // lock free, increment node_::referenceCount, used for iterator and for prior-node_'s link
        static void decrement_reference_count_ (node_*& n);

        // lock free, used for iterators and for for prior-node_'s link
        // return a new "ownership"
        static node_* increment_reference_count_ (node_* n);

        // lock free, swap the node_ *s in left and right,
        static void exchange_ (std::atomic<node_*>& left, node_*& right);

        // NOT lock free on left, lock free on right
        static void exchange_ (std::atomic<node_*>& left, std::atomic<node_*>& right);

        // NOT lock free, set atomic_ptr to spin_ and return the node_ * leaving the node_ locked, unless atomic_ptr is already terminal_ then return terminal_
        // "ownership" is transferred from atomic_ptr to the return value
        static node_* owner_lock_ (std::atomic<node_*>& atomic_ptr);

        // lock free, but requires a preceding call to lock, changes atomic_ptr from spin_ to n, sets n to nullptr
        // "ownership" is transfered from n to atomic_ptr
        static void owner_unlock_ (std::atomic<node_*>& atomic_ptr, node_*& n);

        // NOT lock free
        static node_* new_ownership_ (std::atomic<node_*>& atomic_ptr);
    };

}
namespace std {
    /**
     * NOT lock free on a, lock free on b
     */
    template <typename T>
    void swap (Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& a, Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& b) noexcept;
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "forward_list.inl"

#endif /*_Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_ */
