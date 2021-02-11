/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::LockFreeDataStructures {

    /*
     ********************************************************************************
     ***************** LockFreeDataStructures::forward_list<T>::node_ ***************
     ********************************************************************************
     */
    template <typename T>
    class forward_list<T>::node_ {
    public:
        friend class ForwardIterator<T>;
        T                   value;
        std::atomic<node_*> next;
        std::atomic<int>    referenceCount; // for keeping a node_ referenced by an iterator alive

        node_ (T const& value)
            : value{value}
            , next{terminal_ ()}
            , referenceCount{1}
        {
        }
        node_ (T&& value)
            : value{std::move (value)}
            , next{terminal_ ()}
            , referenceCount{1}
        {
        }
        template <class... U>
        node_ (U&&... params)
            : value{std::forward<U> (params)...}
            , next{terminal_ ()}
            , referenceCount{1}
        {
        }
        ~node_ ()
        {
            node_* n = owner_lock_ (next); // change next to spin_
            if (n != terminal_ ()) {
                decrement_reference_count_ (n);                      // release ownership of next
                next.store (terminal_ (), std::memory_order_relaxed); // relaxed because observers will see spin_
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
        : fFirst_{terminal_ ()}
    {
    }
    template <typename T>
    inline forward_list<T>::forward_list (forward_list const& src)
        : forward_list{}
    {
        std::atomic<node_*>* nextPtr = &fFirst_;
        for (auto const& v : src) {
            std::atomic<node_*>& next    = *nextPtr;
            node_*               newNode = new node_ (v);
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
    inline bool forward_list<T>::empty () const
    {
        return fFirst_.load () == terminal_ ();
    }
    template <typename T>
    int forward_list<T>::clear ()
    {
        node_* current = terminal_ ();
        // detach the elements fFirst_ so that blocking is minimal
        exchange_ (fFirst_, current);
        if (current == terminal_ ()) {
            return 0;
        }
        // if we just delete the fFirst_ node_, it may cascade down all the
        // subsequent nodes. This would be fine, if not for the possibility
        // of blowing the stack. Instead we delete them in reverse.
        std::vector<node_*> nodes;
        while (current != terminal_ ()) {
            nodes.push_back (current);
            node_* temp = terminal_ ();
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
    int forward_list<T>::locked_clear ()
    {
        std::list<node_*> nodes;
        {
            node_* i = terminal_ ();
            exchange_ (fFirst_, i);
            while (i) {
                nodes.push_back (i);
                node_* temp = spin_ ();
                exchange_ (i->next, temp); // lock all the nodes
                i = temp;
            }
        }
        for (auto const& i = nodes.begin (); i != nodes.end (); ++i) {
            decrement_reference_count_ (*i);                        // remove prior nodes reference
            i->next.store (terminal_ (), std::memory_order_relaxed); // unlink, relaxed because observers will see spin_
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
        return insert_node_ (fFirst_, new node_ (std::move (value)));
    }
    template <typename T>
    template <class... U>
    inline auto forward_list<T>::emplace_front (U&&... params) -> reference
    {
        return *insert_node_ (fFirst_, new node_ (std::forward<U> (params)...));
    }

    template <typename T>
    bool forward_list<T>::remove_node_ (std::atomic<node_*>& atomic_ptr, T* value)
    {
        node_* x = owner_lock_ (atomic_ptr);
        if (x == terminal_ ()) {
            if (atomic_ptr.load () == terminal_ ())
                return false;
            node_* temp = terminal_ ();
            owner_unlock (atomic_ptr, temp);
            return false;
        }
        *value   = x->value;
        node_* y = owner_lock_ (x->next);
        owner_unlock (atomic_ptr, y);
        x->next.store (terminal_ ());
        decrement_reference_count_ (x);
        return true;
    }

    template <typename T>
    auto forward_list<T>::insert_node_ (std::atomic<node_*>& atomic_ptr, node_* n) -> iterator
    {
        iterator result (n); // it's possible that the node_ is removed before we return, so do this early
        n->next.store (n);
        exchange_ (n->next, atomic_ptr);
        return result;
    }
    template <typename T>
    auto forward_list<T>::seperate_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* oldNext = terminal_ ();
        exchange_ (atomic_ptr, oldNext);
        return oldNext;
    }
    template <typename T>
    auto forward_list<T>::terminal_ () -> node_*
    {
        return (node_*)Private_::concurrent_forward_list_details::terminal_;
    }
    template <typename T>
    auto forward_list<T>::spin_ () -> node_*
    {
        return (node_*)Private_::concurrent_forward_list_details::spin;
    }
    template <typename T>
    inline void forward_list<T>::decrement_reference_count_ (node_*& n)
    {
        Assert (n != nullptr);
        Assert (n != terminal_ ()); // not a valid node_
        Assert (n != spin_ ());     // not a valid node_
        if (n->referenceCount.fetch_sub (1) == 1) {
            delete n;
        }
        n = nullptr;
    }
    template <typename T>
    inline auto forward_list<T>::increment_reference_count_ (node_* n) -> node_*
    {
        Assert (n != nullptr); //must be a valid node_ because ownership is a precondition
        Assert (n != terminal_ ());
        Assert (n != spin_ ());
        n->referenceCount.fetch_add (1);
        return n;
    }
    template <typename T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, node_*& right)
    {
        assert (right != nullptr);
        assert (right != spin_ ()); // invalid node_
        node_* n = left.load ();
        do {
            while (n == spin_ ()) {
                n = left.load (std::memory_order_relaxed); // relaxed because visibility of unlocked state may be at systems leisure
            }
        } while (!left.compare_exchange_weak (n, right));
        assert (n != nullptr);
        right = n;
    }
    template <typename T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, std::atomic<node_*>& right)
    {
        node_* temp = owner_lock_ (left);
        exchange_ (right, temp);
        if (temp != terminal_ ()) {
            owner_unlock (left, temp);
        }
        else {
            left.store (terminal_ ());
        }
    }
    template <typename T>
    auto forward_list<T>::owner_lock_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* n = atomic_ptr.load ();
        do {
            while (n == spin_ ()) {                               // wait for owner_unlock
                n = atomic_ptr.load (std::memory_order_relaxed); // relaxed because visibility of unlocked state may be at systems leisure
            }
        } while (!atomic_ptr.compare_exchange_weak (n, spin_ ()));

        if (n == terminal_ ()) {                                        // the node_ has been deleted already
                                                                       // put terminal_ back in to owner_unlock
            atomic_ptr.store (terminal_ (), std::memory_order_relaxed); // relaxed because observers will see spin_
            return terminal_ ();
        } // else stays locked
        return n;
    }
    template <typename T>
    inline void forward_list<T>::owner_unlock (std::atomic<node_*>& atomic_ptr, node_*& n)
    {
        Assert (n != nullptr);
        Assert (n != spin_ ());
        Assert (atomic_ptr.load (std::memory_order_relaxed) == spin_ ()); // relaxed because it was set to spin_ by the current thread
        atomic_ptr.store (n, std::memory_order_relaxed);                 // relaxed because observers will see spin_
        n = nullptr;                                                     // make sure the caller cant use the pointer anymore
    }
    template <typename T>
    auto forward_list<T>::new_ownership_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* temp = owner_lock_ (atomic_ptr);
        if (temp == terminal_ ()) {
            return terminal_ ();
        }
        node_* result = temp != terminal_ () ? increment_reference_count_ (temp) : terminal_ ();
        owner_unlock (atomic_ptr, temp);
        return result;
    }

}
#endif /* _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_ */
