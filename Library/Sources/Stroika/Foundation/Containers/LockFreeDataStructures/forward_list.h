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
locks that do occur are per-element spin locks
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

            // provides a globally unique pointer used for the terminal node_
            constexpr struct {
            } terminal_v;
            constexpr void* terminal = (void*)&terminal_v;

            // provides a globally unique pointer for the lock node_
            constexpr struct {
            } spin_v;
            constexpr void* spin = (void*)&spin_v;

        }
    }

    // similar to std::forward_list, but thread safe and partiallt lock free
    template <class T>
    class forward_list {

    private:
        class node_;

    public:
        template <class U>
        // construction is lock free (though begin() is not)
        // incrementing is NOT lock free
        class ForwardIterator {
            friend class forward_list;
            node_*                            current;
            typedef std::forward_iterator_tag iterator_category;
            typedef U                         value_type;
            typedef U&                        reference;
            typedef U*                        pointer;

        public:
            ForwardIterator ()
                : current (terminal ())
            {
            }

            ForwardIterator (node_* n)
                : current (n != terminal () ? increment_reference_count (n) : terminal ())
            {
            }

            ForwardIterator (ForwardIterator const& other)
                : current (other.current != terminal () ? increment_reference_count (other.current) : terminal ())
            {
            }

            ForwardIterator (ForwardIterator&& other) noexcept
                : current (terminal ())
            {
                std::swap (current, other.current);
            }

            ~ForwardIterator ()
            {
                if (current == terminal ()) {
                    return;
                }
                decrement_reference_count (current);
            }

            ForwardIterator& operator= (ForwardIterator const& other)
            {
                if (current != terminal ()) {
                    decrement_reference_count (current);
                }
                current = other.current != terminal () ? increment_reference_count (other.current) : terminal ();
                return *this;
            }

            template <typename V>
            ForwardIterator& operator= (V const& other)
            {
                if (current != terminal ()) {
                    decrement_reference_count (current);
                }
                current = other.current != terminal () ? increment_reference_count (other.current) : terminal ();
                return *this;
            }

            T& operator* () const
            {
                if (current == terminal ()) {
                    throw std::logic_error ("invalid iterator");
                }
                return current->value;
            }

            T* operator-> () const
            {
                if (current == terminal ()) {
                    throw std::logic_error ("invalid iterator");
                }
                return &current->value;
            }

            ForwardIterator& operator++ ()
            {
                assert (current != terminal ()); // this is the end()
                node_* temp = new_ownership (current->next);
                std::swap (current, temp);
                if (temp != terminal ()) {
                    decrement_reference_count (temp); // discard newly created ownership
                }
                return *this;
            }

            ForwardIterator operator++ (int)
            {
                assert (current != terminal ()); // this is the end()
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

        typedef T                        value_type;
        typedef value_type&              reference;
        typedef const value_type&        const_reference;
        typedef value_type*              pointer;
        typedef value_type const*        const_pointer;
        typedef ForwardIterator<T>       iterator;
        typedef ForwardIterator<const T> const_iterator;

    public:
        forward_list ();
        forward_list (forward_list const& copy);
        forward_list (forward_list&& move) noexcept;

    public:
        ~forward_list ()
        {
            clear ();
        }

    public:
        // lock free
        bool empty () const
        {
            return first.load () == terminal ();
        }

        // lock free
        // The first node_ is removed before locking the other nodes.
        // Push will not block.
        // An iterator incremented externally may move to a value no longer in the list
        int clear ()
        {
            node* current = terminal ();
            // detach the elements first so that blocking is minimal
            exchange (first, current);
            if (current == terminal ()) {
                return 0;
            }
            // if we just delete the first node_, it may cascade down all the
            // subsequent nodes. This would be fine, if not for the possibility
            // of blowing the stack. Instead we delete them in reverse.
            std::vector<node*> nodes;
            while (current != terminal ()) {
                nodes.push_back (current);
                node* temp = terminal ();
                // take ownership of the next node_
                exchange (current->next, temp);
                current = temp;
            }
            for (auto i = nodes.rbegin (); i != nodes.rend (); ++i) {
                decrement_reference_count (*i);
            }
            return nodes.size (); // return number of deleted elements
        }

        // NOT lock free
        // All nodes are locked before removing the first element.
        // Incrementing an iterator will block, and then result in the end() iterator
        int locked_clear ()
        {
            std::list<node*> nodes;
            {
                node* i = terminal ();
                exchange (first, i);
                while (i) {
                    nodes.push_back (i);
                    node* temp = spin ();
                    exchange (i->next, temp); // lock all the nodes
                    i = temp;
                }
            }
            for (auto const& i = nodes.begin (); i != nodes.end (); ++i) {
                decrement_reference_count (*i);                         // remove prior nodes reference
                i->next.store (terminal (), std::memory_order_relaxed); // unlink, relaxed because observers will see spin
            }
            return nodes.size (); // return number of deleted elements
        }

        // lock free
        T& front ()
        {
            return *begin ();
        }

        // lock free
        iterator push_front (const T& value)
        {
            auto* nodePtr = new node (value);
            return insert_node (first, nodePtr);
        }

        // lock free
        iterator push_front (T&& value)
        {
            return insert_node (first, new node (std::move (value)));
        }

        // lock free
        template <class... U>
        iterator emplace_front (U&&... params)
        {
            return insert_node (first, new node (std::forward<U> (params)...));
        }

        // NOT lock free
        bool pop_front (T* value)
        {
            return remove_node (first, value);
        }

        // NOT lock free
        iterator begin ()
        {
            node*    n = new_ownership (first); // wait for unlock
            iterator result (n);
            if (n != terminal ()) {
                decrement_reference_count (n); // discard newly created ownership
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
            // const_cast is needed to lock first
            std::atomic<node*>& nonConstFirst = *const_cast<std::atomic<node*>*> (&first);
            node*               n             = new_ownership (nonConstFirst);
            if (n == terminal ()) {
                return end ();
            }
            const_iterator result (n);
            decrement_reference_count (n);
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
            return insert_node (position.current->next, new node (value));
        }

        // lock free
        iterator insert_after (const_iterator position, T&& value)
        {
            return insert_node (position.current->next, new node (value));
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
            return insert_node (position, new node (std::forward (params)...));
        }

        // lock free
        // all the elements after position are moved to a new forward_list
        // IMPORTANT: existing iterators will still traverse the separated portion if already within the separated portion
        bool separate_after (const_iterator position, forward_list<T>*& result)
        {
            node* n = seperate (position.current->next);
            if (!n)
                return false;
            result        = new forward_list<T> ();
            result->first = n;
            return true;
        }

        // NOT lock free
        bool erase_after (const_iterator position, T* value)
        {
            return remove_node (position.current->next, value);
        }

        // NOT lock free on a, lock free on b
        friend void swap (forward_list& a, forward_list& b) noexcept
        {
            exchange (a.first, b.first);
        }

    private:
        std::atomic<node*> first; // mutable because iterator construction requires a lock

        // lock free
        static iterator insert_node (std::atomic<node*>& atomic_ptr, node* n)
        {
            iterator result (n); // it's possible that the node_ is removed before we return, so do this early
            n->next.store (n);
            exchange (n->next, atomic_ptr);
            return result;
        }

        // lock free, removes all the nodes from *atomic_ptr forward, and returns that node_ with links still intact
        static node* seperate (std::atomic<node*>& atomic_ptr)
        {
            node* oldNext = terminal ();
            exchange (atomic_ptr, oldNext);
            return oldNext;
        }

        // NOT lock free
        static bool remove_node (std::atomic<node*>& atomic_ptr, T* value)
        {
            node* x = owner_lock (atomic_ptr);
            if (x == terminal ()) {
                if (atomic_ptr.load () == terminal ())
                    return false;
                node* temp = terminal ();
                owner_unlock (atomic_ptr, temp);
                return false;
            }
            *value  = x->value;
            node* y = owner_lock (x->next);
            owner_unlock (atomic_ptr, y);
            x->next.store (terminal ());
            decrement_reference_count (x);
            return true;
        }

        static node* terminal () { return (node*)Private_::concurrent_forward_list_details::terminal; }
        static node* spin () { return (node*)Private_::concurrent_forward_list_details::spin; }

        class node_ {
        public:
            friend class ForwardIterator<T>;
            T                   value;
            std::atomic<node_*> next;
            std::atomic<int>    referenceCount; // for keeping a node_ referenced by an iterator alive

            node_ (T const& value)
                : value (value)
                , next (terminal ())
                , referenceCount (1)
            {
            }

            node_ (T&& value)
                : value (std::move (value))
                , next (terminal ())
                , referenceCount (1)
            {
            }

            template <class... U>
            node_ (U&&... params)
                : value (std::forward<U> (params)...)
                , next (terminal ())
                , referenceCount (1)
            {
            }

            ~node_ ()
            {
                node_* n = owner_lock (next); // change next to spin
                if (n != terminal ()) {
                    decrement_reference_count (n);                       // release ownership of next
                    next.store (terminal (), std::memory_order_relaxed); // relaxed because observers will see spin
                }
            }
        };

        // lock free, increment node_::referenceCount, used for iterator and for prior-node_'s link
        static void decrement_reference_count (node*& n)
        {
            assert (n != nullptr);
            assert (n != terminal ()); // not a valid node_
            assert (n != spin ());     // not a valid node_
            if (n->referenceCount.fetch_sub (1) == 1) {
                delete n;
            }
            n = nullptr;
        }

        // lock free, used for iterators and for for prior-node_'s link
        // return a new "ownership"
        static node* increment_reference_count (node* n)
        {
            assert (n != nullptr); //must be a valid node_ because ownership is a precondition
            assert (n != terminal ());
            assert (n != spin ());
            n->referenceCount.fetch_add (1);
            return n;
        }

        // lock free, swap the node_ *s in left and right,
        static void exchange (std::atomic<node*>& left, node*& right)
        {
            assert (right != nullptr);
            assert (right != spin ()); // invalid node_
            node* n = left.load ();
            do {
                while (n == spin ()) {
                    n = left.load (std::memory_order_relaxed); // relaxed because visibility of unlocked state may be at systems leisure
                }
            } while (!left.compare_exchange_weak (n, right));
            assert (n != nullptr);
            right = n;
        }

        // NOT lock free on left, lock free on right
        static void exchange (std::atomic<node*>& left, std::atomic<node*>& right)
        {
            node* temp = owner_lock (left);
            exchange (right, temp);
            if (temp != terminal ()) {
                owner_unlock (left, temp);
            }
            else {
                left.store (terminal ());
            }
        }

        // NOT lock free, set atomic_ptr to spin and return the node_ * leaving the node_ locked, unless atomic_ptr is already terminal then return terminal
        // "ownership" is transferred from atomic_ptr to the return value
        static node* owner_lock (std::atomic<node*>& atomic_ptr)
        {
            node* n = atomic_ptr.load ();
            do {
                while (n == spin ()) {                               // wait for owner_unlock
                    n = atomic_ptr.load (std::memory_order_relaxed); // relaxed because visibility of unlocked state may be at systems leisure
                }
            } while (!atomic_ptr.compare_exchange_weak (n, spin ()));

            if (n == terminal ()) {                                        // the node_ has been deleted already
                                                                           // put terminal back in to owner_unlock
                atomic_ptr.store (terminal (), std::memory_order_relaxed); // relaxed because observers will see spin
                return terminal ();
            } // else stays locked
            return n;
        }

        // lock free, but requires a preceding call to lock, changes atomic_ptr from spin to n, sets n to nullptr
        // "ownership" is transfered from n to atomic_ptr
        static void owner_unlock (std::atomic<node*>& atomic_ptr, node*& n)
        {
            assert (n != nullptr);
            assert (n != spin ());
            assert (atomic_ptr.load (std::memory_order_relaxed) == spin ()); // relaxed because it was set to spin by the current thread
            atomic_ptr.store (n, std::memory_order_relaxed);                 // relaxed because observers will see spin
            n = nullptr;                                                     // make sure the caller cant use the pointer anymore
        }

        // NOT lock free,
        static node* new_ownership (std::atomic<node*>& atomic_ptr)
        {
            node* temp = owner_lock (atomic_ptr);
            if (temp == terminal ()) {
                return terminal ();
            }
            node* result = temp != terminal () ? increment_reference_count (temp) : terminal ();
            owner_unlock (atomic_ptr, temp);
            return result;
        }
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "forward_list.inl"

#endif /*_Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_ */
