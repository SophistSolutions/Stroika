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

    namespace Private_ {
        namespace concurrent_forward_list_details {

            // provides a globally unique pointer used for the terminal_ node_
            constexpr struct {
            } terminal_v;
            constexpr void* terminal_ = (void*)&terminal_v;

            // provides a globally unique pointer for the lock node_
            constexpr struct {
            } spin_v;
            constexpr void* spin = (void*)&spin_v;

        }
    }

    // similar to std::forward_list, but thread safe and partiallt lock free
    template <typename T>
    class forward_list {
    private:
        class node_;

    public:
        /**
        // construction is lock free (though begin() is not)
        // incrementing is NOT lock free
         */
        template <class U>
        class ForwardIterator {
            friend class forward_list;
            node_*                            current;
            typedef std::forward_iterator_tag iterator_category;
            typedef U                         value_type;
            typedef U&                        reference;
            typedef U*                        pointer;

        public:
            ForwardIterator ()
                : current (terminal_ ())
            {
            }

            ForwardIterator (node_* n)
                : current (n != terminal_ () ? increment_reference_count_ (n) : terminal_ ())
            {
            }

            ForwardIterator (ForwardIterator const& other)
                : current (other.current != terminal_ () ? increment_reference_count_ (other.current) : terminal_ ())
            {
            }

            ForwardIterator (ForwardIterator&& other) noexcept
                : current (terminal_ ())
            {
                std::swap (current, other.current);
            }

            ~ForwardIterator ()
            {
                if (current == terminal_ ()) {
                    return;
                }
                decrement_reference_count_ (current);
            }

            ForwardIterator& operator= (ForwardIterator const& other)
            {
                if (current != terminal_ ()) {
                    decrement_reference_count_ (current);
                }
                current = other.current != terminal_ () ? increment_reference_count_ (other.current) : terminal_ ();
                return *this;
            }

            template <typename V>
            ForwardIterator& operator= (V const& other)
            {
                if (current != terminal_ ()) {
                    decrement_reference_count_ (current);
                }
                current = other.current != terminal_ () ? increment_reference_count_ (other.current) : terminal_ ();
                return *this;
            }

            T& operator* () const
            {
                if (current == terminal_ ()) {
                    throw std::logic_error{"invalid iterator"};
                }
                return current->value;
            }

            T* operator-> () const
            {
                if (current == terminal_ ()) {
                    throw std::logic_error{"invalid iterator"};
                }
                return &current->value;
            }

            ForwardIterator& operator++ ()
            {
                assert (current != terminal_ ()); // this is the end()
                node_* temp = new_ownership_ (current->next);
                std::swap (current, temp);
                if (temp != terminal_ ()) {
                    decrement_reference_count_ (temp); // discard newly created ownership
                }
                return *this;
            }

            ForwardIterator operator++ (int)
            {
                assert (current != terminal_ ()); // this is the end()
                ForwardIterator temp = *this;
                ++*this;
                return temp;
            }

            friend void swap (ForwardIterator& a, ForwardIterator& b) noexcept
            {
                using std::swap; // bring in swap for built-in types
                swap (a.current, b.current);
            }

            operator ForwardIterator<const T> () const
            {
                return ForwardIterator<const T> (current);
            }

            template <typename V>
            bool operator== (V const& rhs)
            {
                return current == rhs.current;
            }

            template <typename V>
            bool operator!= (V const& rhs)
            {
                return !(*this == rhs);
            }
        };

    public:
        typedef T                        value_type;
        typedef value_type&              reference;
        typedef const value_type&        const_reference;
        typedef value_type*              pointer;
        typedef value_type const*        const_pointer;
        typedef ForwardIterator<T>       iterator;
        typedef ForwardIterator<const T> const_iterator;

    public:
        forward_list ();
        forward_list (const forward_list& src);
        forward_list (forward_list&& src) noexcept;

    public:
        ~forward_list ();

    public:
        // lock free
        nonvirtual bool empty () const;

    public:
        /**
        // lock free
        // The first node_ is removed before locking the other nodes.
        // Push will not block.
        // An iterator incremented externally may move to a value no longer in the list

         *  \note UNLIKE https://en.cppreference.com/w/cpp/container/forward_list/clear, this method returns the number of elements deleted
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
        nonvirtual reference front ();
        nonvirtual const_reference front () const;

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/push_front
         *  but returns iterator pointing to element just added
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
        // NOT lock free
        bool pop_front (T* value)
        {
            return remove_node_ (fFirst_, value);
        }

        // NOT lock free
        iterator begin ()
        {
            node_*   n = new_ownership_ (fFirst_); // wait for unlock
            iterator result (n);
            if (n != terminal_ ()) {
                decrement_reference_count_ (n); // discard newly created ownership
            }
            return result;
        }

        // lock free
        iterator end ()
        {
            return iterator ();
        }

        // NOT lock free
        const_iterator begin () const
        {
            // const_cast is needed to lock fFirst_
            std::atomic<node_*>& nonConstFirst = *const_cast<std::atomic<node_*>*> (&fFirst_);
            node_*               n             = new_ownership_ (nonConstFirst);
            if (n == terminal_ ()) {
                return end ();
            }
            const_iterator result (n);
            decrement_reference_count_ (n);
            return result;
        }

        // NOT lock free
        const_iterator cbegin () const
        {
            // add const using const_cast to invoke the const version of begin
            return const_cast<forward_list const*> (this)->begin ();
        }

        // lock free
        const_iterator end () const
        {
            return const_iterator ();
        }

        const_iterator cend () const
        {
            return const_iterator ();
        }

        // lock free
        // returns a default constructed iterator if position is no longer valid
        iterator insert_after (const_iterator position, T const& value)
        {
            return insert_node_ (position.current->next, new node_ (value));
        }

        // lock free
        iterator insert_after (const_iterator position, T&& value)
        {
            return insert_node_ (position.current->next, new node_ (value));
        }

        // lock free
        iterator insert_after (const_iterator pos, int count, const T& value)
        {
            if (count <= 0)
                return iterator ();
            iterator result = pos = insert_after (pos, value);
            for (int i = 1; i < count; i++) {
                pos = insert_after (pos, value);
            }
            return result;
        }

        // lock free
        template <class InputIt>
        iterator insert_after (const_iterator pos, InputIt first, InputIt last)
        {
            if (first == last)
                return iterator ();
            iterator result = pos = insert_after (pos, *first);
            ++first;
            while (first != last) {
                pos = insert_after (pos, first);
                ++first;
            }
            return result;
        }

        // lock free
        iterator insert_after (const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert_after (pos, ilist.begin (), ilist.end ());
        }

        // lock free
        template <class... U>
        iterator emplace_after (const_iterator position, U&&... params)
        {
            return insert_node_ (position, new node_ (std::forward (params)...));
        }

        // lock free
        // all the elements after position are moved to a new forward_list
        // IMPORTANT: existing iterators will still traverse the separated portion if already within the separated portion
        bool separate_after (const_iterator position, forward_list<T>*& result)
        {
            node_* n = seperate_ (position.current->next);
            if (!n)
                return false;
            result        = new forward_list<T> ();
            result->fFirst_ = n;
            return true;
        }

        // NOT lock free
        bool erase_after (const_iterator position, T* value)
        {
            return remove_node_ (position.current->next, value);
        }

        // NOT lock free on a, lock free on b
        friend void swap (forward_list& a, forward_list& b) noexcept
        {
            exchange_ (a.fFirst_, b.fFirst_);
        }

    private:
        std::atomic<node_*> fFirst_;

        // (LGP APPEARS TO) return iterator point at element just added
        // lock free
        static iterator insert_node_ (std::atomic<node_*>& atomic_ptr, node_* n);

        // lock free, removes all the nodes from *atomic_ptr forward, and returns that node_ with links still intact
        static node_* seperate_ (std::atomic<node_*>& atomic_ptr);

     private:
        // NOT lock free
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
        static void owner_unlock (std::atomic<node_*>& atomic_ptr, node_*& n);

        // NOT lock free
        static node_* new_ownership_ (std::atomic<node_*>& atomic_ptr);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "forward_list.inl"

#endif /*_Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_ */
