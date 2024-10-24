/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <list>
#include <memory>
#include <vector>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Valgrind.h"

namespace Stroika::Foundation::Containers::LockFreeDataStructures {

    /**
     * construction is lock free (though begin() is not)
     * incrementing is NOT lock free
     */
    template <typename T>
    template <typename U>
    class forward_list<T>::ForwardIterator_ {
        friend class forward_list;
        node_* current;
        using iterator_category = std::forward_iterator_tag;
        using value_type        = U;
        using reference         = U&;
        using pointer           = U*;
        //using difference_type = ptrdiff_t;    // doesn't work cuz we dont support operator- on iterators (not sure what it should do?)

    public:
        ForwardIterator_ ()
            : current{kTerminalSentinel_}
        {
        }
        ForwardIterator_ (node_* n)
            : current{n != kTerminalSentinel_ ? increment_reference_count_ (n) : kTerminalSentinel_}
        {
        }
        ForwardIterator_ (const ForwardIterator_& other)
            : current{other.current != kTerminalSentinel_ ? increment_reference_count_ (other.current) : kTerminalSentinel_}
        {
        }
        ForwardIterator_ (ForwardIterator_&& other) noexcept
            : current{kTerminalSentinel_}
        {
            std::swap (current, other.current);
        }
        ~ForwardIterator_ ()
        {
            if (current == kTerminalSentinel_) {
                return;
            }
            decrement_reference_count_ (current);
        }
        ForwardIterator_& operator= (ForwardIterator_ const& other)
        {
            if (current != kTerminalSentinel_) {
                decrement_reference_count_ (current);
            }
            current = other.current != kTerminalSentinel_ ? increment_reference_count_ (other.current) : kTerminalSentinel_;
            return *this;
        }
        template <typename V>
        ForwardIterator_& operator= (const V& rhs)
        {
            if (current != kTerminalSentinel_) {
                decrement_reference_count_ (current);
            }
            current = rhs.current != kTerminalSentinel_ ? increment_reference_count_ (rhs.current) : kTerminalSentinel_;
            return *this;
        }
        T& operator* () const
        {
            if (current == kTerminalSentinel_) {
                throw std::logic_error{"invalid iterator"};
            }
            return current->value;
        }
        T* operator->() const
        {
            if (current == kTerminalSentinel_) {
                throw std::logic_error{"invalid iterator"};
            }
            return &current->value;
        }
        ForwardIterator_& operator++ ()
        {
            Assert (current != kTerminalSentinel_); // this is the end()
            node_* temp = new_ownership_ (current->next);
            std::swap (current, temp);
            if (temp != kTerminalSentinel_) {
                decrement_reference_count_ (temp); // discard newly created ownership
            }
            return *this;
        }
        ForwardIterator_ operator++ (int)
        {
            Assert (current != kTerminalSentinel_); // this is the end()
            ForwardIterator_ temp = *this;
            ++*this;
            return temp;
        }
        friend void swap (ForwardIterator_& a, ForwardIterator_& b) noexcept
        {
            using std::swap; // bring in swap for built-in types
            swap (a.current, b.current);
        }
        operator ForwardIterator_<const T> () const
        {
            return ForwardIterator_<const T> (current);
        }
        bool operator== (const ForwardIterator_& rhs) const
        {
            return current == rhs.current;
        }
    };
    // static_assert (weakly_incrementable<forward_list<int>::const_iterator>); // fails cuz cannot take difference of two iterators...

    /*
     ********************************************************************************
     ***************** LockFreeDataStructures::forward_list<T>::node_ ***************
     ********************************************************************************
     */
    template <typename T>
    class forward_list<T>::node_ {
    public:
        friend class ForwardIterator_<T>;
        T                   value;
        std::atomic<node_*> next;
        std::atomic<int>    referenceCount; // for keeping a node_ referenced by an iterator alive

        node_ (T const& value)
            : value{value}
            , next{kTerminalSentinel_}
            , referenceCount{1}
        {
        }
        node_ (T&& value)
            : value{std::move (value)}
            , next{kTerminalSentinel_}
            , referenceCount{1}
        {
        }
        template <typename... U>
        node_ (U&&... params)
            : value{std::forward<U> (params)...}
            , next{kTerminalSentinel_}
            , referenceCount{1}
        {
        }
        ~node_ ()
        {
            node_* n = owner_lock_ (next); // change next to kSpinSentinel_
            if (n != kTerminalSentinel_) {
                decrement_reference_count_ (n);                             // release ownership of next
                next.store (kTerminalSentinel_, std::memory_order_relaxed); // relaxed because observers will see kSpinSentinel_
            }
        }
    };

    /*
     ********************************************************************************
     ***************** LockFreeDataStructures::forward_list<T> **********************
     ********************************************************************************
     */
    template <typename T>
    inline forward_list<T>::forward_list ()
        : fFirst_{kTerminalSentinel_}
    {
    }
    template <typename T>
    inline forward_list<T>::forward_list (forward_list const& src)
        : forward_list{}
    {
        std::atomic<node_*>* nextPtr = &fFirst_;
        for (auto const& v : src) {
            std::atomic<node_*>& next    = *nextPtr;
            node_*               newNode = new node_{v};
            next.store (newNode);
            nextPtr = &newNode->next;
        }
    }
    template <typename T>
    inline forward_list<T>::forward_list (forward_list&& src) noexcept
        : forward_list{}
    {
        exchange_ (src.fFirst_, fFirst_);
    }
    template <typename T>
    inline forward_list<T>::~forward_list ()
    {
        clear ();
    }
    template <typename T>
    inline bool forward_list<T>::empty () const noexcept
    {
        return fFirst_.load () == kTerminalSentinel_;
    }
    template <typename T>
    size_t forward_list<T>::clear ()
    {
        node_* current = kTerminalSentinel_;
        // detach the elements fFirst_ so that blocking is minimal
        exchange_ (fFirst_, current);
        if (current == kTerminalSentinel_) {
            return 0;
        }
        // if we just delete the fFirst_ node_, it may cascade down all the
        // subsequent nodes. This would be fine, if not for the possibility
        // of blowing the stack. Instead we delete them in reverse.
        std::vector<node_*> nodes;
        while (current != kTerminalSentinel_) {
            nodes.push_back (current);
            node_* temp = kTerminalSentinel_;
            // take ownership of the next node_
            exchange_ (current->next, temp);
            current = temp;
        }
        for (auto i = nodes.rbegin (); i != nodes.rend (); ++i) {
            decrement_reference_count_ (*i);
        }
        return nodes.size (); // return number of deleted elements
    }
    template <typename T>
    size_t forward_list<T>::locked_clear ()
    {
        std::list<node_*> nodes;
        {
            node_* i = kTerminalSentinel_;
            exchange_ (fFirst_, i);
            while (i) {
                nodes.push_back (i);
                node_* temp = kSpinSentinel_;
                exchange_ (i->next, temp); // lock all the nodes
                i = temp;
            }
        }
        for (auto const& i = nodes.begin (); i != nodes.end (); ++i) {
            decrement_reference_count_ (*i);                               // remove prior nodes reference
            i->next.store (kTerminalSentinel_, std::memory_order_relaxed); // unlink, relaxed because observers will see kSpinSentinel_
        }
        return nodes.size (); // return number of deleted elements
    }
    template <typename T>
    inline auto forward_list<T>::front () -> reference
    {
        return *begin ();
    }
    template <typename T>
    inline auto forward_list<T>::front () const -> const_reference
    {
        return *begin ();
    }
    template <typename T>
    inline auto forward_list<T>::push_front (const T& value) -> iterator
    {
        auto* nodePtr = new node_{value};
        return insert_node_ (fFirst_, nodePtr);
    }
    template <typename T>
    inline auto forward_list<T>::push_front (T&& value) -> iterator
    {
        return insert_node_ (fFirst_, new node_{std::move (value)});
    }
    template <typename T>
    template <typename... U>
    inline auto forward_list<T>::emplace_front (U&&... params) -> reference
    {
        return *insert_node_ (fFirst_, new node_{std::forward<U> (params)...});
    }
    template <typename T>
    inline std::optional<T> forward_list<T>::pop_front ()
    {
        T r{};
        return remove_node_ (fFirst_, &r) ? r : std::optional<T>{};
    }
    template <typename T>
    inline auto forward_list<T>::begin () -> iterator
    {
        node_*   n = new_ownership_ (fFirst_); // wait for unlock
        iterator result{n};
        if (n != kTerminalSentinel_) {
            decrement_reference_count_ (n); // discard newly created ownership
        }
        return result;
    }
    template <typename T>
    auto forward_list<T>::begin () const -> const_iterator
    {
        // const_cast is needed to lock fFirst_
        std::atomic<node_*>& nonConstFirst = *const_cast<std::atomic<node_*>*> (&fFirst_);
        node_*               n             = new_ownership_ (nonConstFirst);
        if (n == kTerminalSentinel_) {
            return end ();
        }
        const_iterator result{n};
        decrement_reference_count_ (n);
        return result;
    }
    template <typename T>
    inline auto forward_list<T>::cbegin () const -> const_iterator
    {
        // add const using const_cast to invoke the const version of begin
        return const_cast<forward_list const*> (this)->begin ();
    }
    template <typename T>
    inline auto forward_list<T>::end () -> iterator
    {
        return iterator{};
    }
    template <typename T>
    inline auto forward_list<T>::end () const -> const_iterator
    {
        return const_iterator{};
    }
    template <typename T>
    inline auto forward_list<T>::cend () const -> const_iterator
    {
        return const_iterator{};
    }
    template <typename T>
    inline auto forward_list<T>::insert_after (const_iterator position, T const& value) -> iterator
    {
        return insert_node_ (position.current->next, new node_{value});
    }
    template <typename T>
    inline auto forward_list<T>::insert_after (const_iterator position, T&& value) -> iterator
    {
        return insert_node_ (position.current->next, new node_{value});
    }
    template <typename T>
    auto forward_list<T>::insert_after (const_iterator pos, int count, const T& value) -> iterator
    {
        if (count <= 0)
            return iterator{};
        iterator result = pos = insert_after (pos, value);
        for (int i = 1; i < count; ++i) {
            pos = insert_after (pos, value);
        }
        return result;
    }
    template <typename T>
    template <typename InputIt>
    auto forward_list<T>::insert_after (const_iterator pos, InputIt first, InputIt last) -> iterator
    {
        while (first != last) {
            pos = insert_after (pos, first);
            ++first;
        }
        return pos;
    }
    template <typename T>
    inline auto forward_list<T>::insert_after (const_iterator pos, std::initializer_list<T> ilist) -> iterator
    {
        return insert_after (pos, ilist.begin (), ilist.end ());
    }
    template <typename T>
    template <typename... U>
    inline auto forward_list<T>::emplace_after (const_iterator position, U&&... params) -> iterator
    {
        return insert_node_ (position, new node_{std::forward (params)...});
    }
    template <typename T>
    bool forward_list<T>::separate_after (const_iterator position, forward_list<T>*& result)
    {
        node_* n = separate_ (position.current->next);
        if (!n)
            return false;
        result          = new forward_list<T>{};
        result->fFirst_ = n;
        return true;
    }
    template <typename T>
    inline bool forward_list<T>::erase_after (const_iterator position, T* value)
    {
        return remove_node_ (position.current->next, value);
    }
    template <typename T>
    inline void forward_list<T>::swap (forward_list& other) noexcept
    {
        exchange_ (fFirst_, other.fFirst_);
    }
    template <typename T>
    bool forward_list<T>::remove_node_ (std::atomic<node_*>& atomic_ptr, T* value)
    {
        node_* x = owner_lock_ (atomic_ptr);
        if (x == kTerminalSentinel_) {
            if (atomic_ptr.load () == kTerminalSentinel_)
                return false;
            node_* temp = kTerminalSentinel_;
            owner_unlock_ (atomic_ptr, temp);
            return false;
        }
        *value   = x->value;
        node_* y = owner_lock_ (x->next);
        owner_unlock_ (atomic_ptr, y);
        x->next.store (kTerminalSentinel_);
        decrement_reference_count_ (x);
        return true;
    }
    template <typename T>
    inline auto forward_list<T>::insert_node_ (std::atomic<node_*>& atomic_ptr, node_* n) -> iterator
    {
        iterator result{n}; // it's possible that the node_ is removed before we return, so do this early
        n->next.store (n);
        exchange_ (n->next, atomic_ptr);
        return result;
    }
    template <typename T>
    inline auto forward_list<T>::separate_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* oldNext = kTerminalSentinel_;
        exchange_ (atomic_ptr, oldNext);
        return oldNext;
    }
    template <typename T>
    inline void forward_list<T>::decrement_reference_count_ (node_*& n)
    {
        Assert (n != nullptr);
        Assert (n != kTerminalSentinel_); // not a valid node_
        Assert (n != kSpinSentinel_);     // not a valid node_
        if (n->referenceCount-- == 1) {
            delete n;
        }
        n = nullptr;
    }
    template <typename T>
    inline auto forward_list<T>::increment_reference_count_ (node_* n) -> node_*
    {
        Assert (n != nullptr); //must be a valid node_ because ownership is a precondition
        Assert (n != kTerminalSentinel_);
        Assert (n != kSpinSentinel_);
        ++n->referenceCount;
        return n;
    }
    template <typename T>
    inline auto forward_list<T>::spin_get_ (const std::atomic<node_*>& n) -> node_*
    {
        while (true) {
            auto p = n.load ();
            if (p != kSpinSentinel_) {
                return p;
            }
        }
    }
    template <typename T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, node_*& right)
    {
        Assert (right != nullptr);
        Assert (right != kSpinSentinel_); // invalid node_
        node_* n = left.load ();
        do {
            n = spin_get_ (left);
        } while (!left.compare_exchange_weak (n, right));
        Assert (n != nullptr);
        right = n;
    }
    template <typename T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, std::atomic<node_*>& right)
    {
        node_* temp = owner_lock_ (left);
        exchange_ (right, temp);
        if (temp != kTerminalSentinel_) {
            owner_unlock_ (left, temp);
        }
        else {
            left.store (kTerminalSentinel_);
        }
    }
    template <typename T>
    auto forward_list<T>::owner_lock_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* n = atomic_ptr.load ();
        do {
            n = spin_get_ (atomic_ptr);
        } while (!atomic_ptr.compare_exchange_weak (n, kSpinSentinel_));

        if (n == kTerminalSentinel_) {                                        // the node_ has been deleted already
                                                                              // put terminal_ back in to owner_unlock_
            atomic_ptr.store (kTerminalSentinel_, std::memory_order_relaxed); // relaxed because observers will see kSpinSentinel_
            return kTerminalSentinel_;
        } // else stays locked
        return n;
    }
    template <typename T>
    inline void forward_list<T>::owner_unlock_ (std::atomic<node_*>& atomic_ptr, node_*& n)
    {
        Assert (n != nullptr);
        Assert (n != kSpinSentinel_);
        Assert (atomic_ptr.load (std::memory_order_relaxed) == kSpinSentinel_); // relaxed because it was set to kSpinSentinel_ by the current thread
        atomic_ptr.store (n, std::memory_order_relaxed);                        // relaxed because observers will see kSpinSentinel_
        n = nullptr;                                                            // make sure the caller cant use the pointer anymore
    }
    template <typename T>
    auto forward_list<T>::new_ownership_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* temp = owner_lock_ (atomic_ptr);
        if (temp == kTerminalSentinel_) {
            return kTerminalSentinel_;
        }
        node_* result = temp != kTerminalSentinel_ ? increment_reference_count_ (temp) : kTerminalSentinel_;
        owner_unlock_ (atomic_ptr, temp);
        return result;
    }

}

namespace std {
    template <typename T>
    inline void swap (Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& lhs,
                      Stroika::Foundation::Containers::LockFreeDataStructures::forward_list<T>& rhs) noexcept
    {
        lhs.swap (rhs);
    }
}
