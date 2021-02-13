/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  Initial implementation copied from (so full credit for algorithms)
 *      https://codereview.stackexchange.com/questions/167252/c11-lock-free-collection-similar-to-stdforward-list-follow-up-2
 *      APPENTLY written by https://codereview.stackexchange.com/users/62314/brent
 *      All said about license we MIT licence, which is what Stroika uses, so I assume will be OK.
 * 
 *  TODO:
 *      @todo   Review https://en.wikipedia.org/wiki/Non-blocking_linked_list for more ideas on how to improve/fix
 *              this code.
 *      @todo   Review https://www.infoq.com/news/2014/10/cpp-lock-free-programming/ to make sure I've handled the
 *              parts he covers - at least - correctly.
 * 
 *              Sutters approach is fairly different. Worth considering, as much simpler. He probably showed all I really
 *              need - the definition of reference, and how that fits with push_front, pop_front... Could implement both ways
 *              with ifdefs?
 * 
 *              @see https://stroika.atlassian.net/browse/STK-540 for details on stuff todo above
 */

namespace Stroika::Foundation::Containers::LockFreeDataStructures {

    /**
     * similar to std::forward_list, but thread safe and (partially) lock free
     * 
     *      o   all methods are thread safe, baring the destructor
     *      o   push, insert, emplace, pop, erase, and iterator increment/dereference are O(1)
     *      o   locks that do occur are per-element spin_ locks
     *      o   push, insert, emplace, clear, and separate operations are lock free.
     *      o   pop and erase are not lock free.
     *      o   begin, cbegin, and incrementing iterators are not lock free
     *      o   dereferencing iterators is lock free
     *      o   uses reference counting to preserve iterators (and values) of removed elements
     *      o   iterators of removed elements will increment to end()
     *      o   insert_after or emplace_after on a removed iterator will return end() to indicate failure.
     * 
     *  Methods from std::forward_list NYI
     *      o   get_allocator & allocators in general (probably doesn't make sense here)
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/before_begin
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/max_size
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/resize
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/merge
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/splice_after
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/remove
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/reverse
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/unique
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/sort
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/operator_cmp
     *      o   https://en.cppreference.com/w/cpp/container/forward_list/erase2
     * 
     *  Methods from std::forward_list materially wrongly, incompletely implemented
     *      o   erase_after incomplete/wrong API
     */
    template <typename T>
    class forward_list {
    private:
        class node_;
        template <typename U>
        class ForwardIterator_;

    public:
        typedef T                         value_type;
        typedef value_type&               reference;
        typedef const value_type&         const_reference;
        typedef value_type*               pointer;
        typedef value_type const*         const_pointer;
        typedef ForwardIterator_<T>       iterator;
        typedef ForwardIterator_<const T> const_iterator;

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
         *  NYI
         */
        nonvirtual forward_list& operator= (const forward_list& rhs) = delete;

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/empty
         *
         * lock free
         */
        nonvirtual bool empty () const noexcept;

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
        nonvirtual size_t clear ();

    public:
        /**
         *   **NOT A METHOD OF std::forward_list<>
         * NOT lock free
         * All nodes are locked before removing the first element.
         * Incrementing an iterator will block, and then result in the end() iterator
         */
        nonvirtual size_t locked_clear ();

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/front
         * <ORIGINAL DOCS SAID 'lock free' - but it calls begin () which orignal docs say NOT lock-free, so unclear but probably NOT lockfree>
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
         * lock free
         */
        template <typename... U>
        nonvirtual reference emplace_front (U&&... params);

    public:
        /**
         * @ see https://en.cppreference.com/w/cpp/container/forward_list/pop_front
         * UNLIKE std version, this returns true iff it removed something, and has overload returning value popped as argument 
         * **not** lock free
         */
        nonvirtual bool pop_front ();
        nonvirtual bool pop_front (T* value);

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/begin
         *
         * **not** lock free
         */
        nonvirtual iterator        begin ();
        nonvirtual const_iterator begin () const;

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/begin
         *
         * **not** lock free
         */
        nonvirtual const_iterator cbegin () const;

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/end
         *
         * lock free
         */
        nonvirtual iterator         end ();
        nonvirtual const_iterator   end () const;

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/end
         *
         * lock free
         */
        nonvirtual const_iterator cend () const;

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/insert_after
         * 
         *  All overloads lock_free
         *      << was comented only on first overload but appears to apply to all) returns a default constructed iterator if position is no longer valid
         */
        nonvirtual iterator     insert_after (const_iterator position, T const& value);
        nonvirtual iterator     insert_after (const_iterator position, T&& value);
        nonvirtual iterator     insert_after (const_iterator pos, int count, const T& value);
        template <typename InputIt>
        nonvirtual iterator     insert_after (const_iterator pos, InputIt first, InputIt last);
        nonvirtual iterator     insert_after (const_iterator pos, std::initializer_list<T> ilist);

    public:
        /**
         * @see https://en.cppreference.com/w/cpp/container/forward_list/emplace_after
         * 
         * lock free
         */
        template <typename... U>
        nonvirtual iterator emplace_after (const_iterator position, U&&... params);

    public:
        /**
         *  NOT standard forward_list<> funciton but something the original author found useful.
         * lock free
         * 
         * all the elements after position are moved to a new forward_list
         * IMPORTANT: existing iterators will still traverse the separated portion if already within the separated portion
         */
        nonvirtual bool separate_after (const_iterator position, forward_list<T>*& result);

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/erase_after
         * 
         *  <<@todo fix - wrong API - sb two overloads and this is neither)
         *
         * NOT lock free
         */
        nonvirtual bool erase_after (const_iterator position, T* value);

    public:
        /**
         *  @see https://en.cppreference.com/w/cpp/container/forward_list/swap
         * 
         * NOT lock free on this, lock free on other
         */
        nonvirtual void swap (forward_list& other) noexcept;

    private:
        // provides a globally unique pointer for the lock/spin sentinals node_
        static constexpr struct { } kTerminalSentinalValue_;
        static inline const auto kTerminalSentinal_ = const_cast<node_*> (reinterpret_cast<const node_*> (&kTerminalSentinalValue_));
        static constexpr struct {} kSpinSentinalValue_;
        static inline const auto kSpinSentinal_ = const_cast<node_*> (reinterpret_cast<const node_*> (&kSpinSentinalValue_));

        std::atomic<node_*> fFirst_;

        // (LGP APPEARS TO) return iterator point at element just added
        // lock free
        static iterator insert_node_ (std::atomic<node_*>& atomic_ptr, node_* n);

        // lock free, removes all the nodes from *atomic_ptr forward, and returns that node_ with links still intact
        static node_* separate_ (std::atomic<node_*>& atomic_ptr);

        // NOT lock free
        // returns true iff it removed something
        static bool remove_node_ (std::atomic<node_*>& atomic_ptr, T* value);

        // lock free, decrement node_::referenceCount, used for iterator and for prior-node_'s link
        static void decrement_reference_count_ (node_*& n);

        // lock free, used for iterators and for for prior-node_'s link
        // return a new "ownership"
        static node_* increment_reference_count_ (node_* n);

        // fetch the data from node 'n' until we get a value which differs from kSpinSentinal_
        static node_* spin_get_ (const std::atomic<node_*>& n);

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
    template <class T>
    void swap (Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& lhs,
               Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& rhs) noexcept;
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "forward_list.inl"

#endif /*_Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_h_ */
