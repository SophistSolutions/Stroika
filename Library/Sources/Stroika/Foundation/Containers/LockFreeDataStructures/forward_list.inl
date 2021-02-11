/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::LockFreeDataStructures {

    /*
     ********************************************************************************
     ***************** LockFreeDataStructures::forward_list<T> **********************
     ********************************************************************************
     */
    template <class T>
    inline forward_list<T>::forward_list ()
        : fFirst_{terminal ()}
    {
    }
    template <class T>
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
    template <class T>
    inline forward_list<T>::forward_list (forward_list&& src) noexcept
        : forward_list{}
    {
        exchange_ (src.fFirst_, fFirst_);
    }
    template <class T>
    inline forward_list<T>::~forward_list ()
    {
        clear ();
    }
    template <class T>
    inline bool forward_list<T>::empty () const
    {
        return fFirst_.load () == terminal ();
    }
    template <class T>
    int forward_list<T>::clear ()
    {
        node_* current = terminal ();
        // detach the elements fFirst_ so that blocking is minimal
        exchange_ (fFirst_, current);
        if (current == terminal ()) {
            return 0;
        }
        // if we just delete the fFirst_ node_, it may cascade down all the
        // subsequent nodes. This would be fine, if not for the possibility
        // of blowing the stack. Instead we delete them in reverse.
        std::vector<node_*> nodes;
        while (current != terminal ()) {
            nodes.push_back (current);
            node_* temp = terminal ();
            // take ownership of the next node_
            exchange_ (current->next, temp);
            current = temp;
        }
        for (auto i = nodes.rbegin (); i != nodes.rend (); ++i) {
            decrement_reference_count (*i);
        }
        return nodes.size (); // return number of deleted elements
    }
    template <class T>
    int forward_list<T>::locked_clear ()
    {
        std::list<node_*> nodes;
        {
            node_* i = terminal ();
            exchange_ (fFirst_, i);
            while (i) {
                nodes.push_back (i);
                node_* temp = spin ();
                exchange_ (i->next, temp); // lock all the nodes
                i = temp;
            }
        }
        for (auto const& i = nodes.begin (); i != nodes.end (); ++i) {
            decrement_reference_count (*i);                         // remove prior nodes reference
            i->next.store (terminal (), std::memory_order_relaxed); // unlink, relaxed because observers will see spin
        }
        return nodes.size (); // return number of deleted elements
    }
    template <class T>
    inline auto forward_list<T>::front () -> reference
    {
        return *begin ();
    }
    template <class T>
    inline auto forward_list<T>::front () const -> const_reference
    {
        return *begin ();
    }
    template <class T>
    inline auto forward_list<T>::push_front (const T& value) -> iterator
    {
        auto* nodePtr = new node_{value};
        return insert_node_ (fFirst_, nodePtr);
    }
    template <class T>
    inline auto forward_list<T>::push_front (T&& value) -> iterator
    {
        return insert_node_ (fFirst_, new node_ (std::move (value)));
    }
    template <class T>
    template <class... U>
    inline auto forward_list<T>::emplace_front (U&&... params) -> reference
    {
        return *insert_node_ (fFirst_, new node_ (std::forward<U> (params)...));
    }

    template <class T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, node_*& right)
    {
        assert (right != nullptr);
        assert (right != spin ()); // invalid node_
        node_* n = left.load ();
        do {
            while (n == spin ()) {
                n = left.load (std::memory_order_relaxed); // relaxed because visibility of unlocked state may be at systems leisure
            }
        } while (!left.compare_exchange_weak (n, right));
        assert (n != nullptr);
        right = n;
    }
    template <class T>
    void forward_list<T>::exchange_ (std::atomic<node_*>& left, std::atomic<node_*>& right)
    {
        node_* temp = owner_lock_ (left);
        exchange_ (right, temp);
        if (temp != terminal ()) {
            owner_unlock (left, temp);
        }
        else {
            left.store (terminal ());
        }
    }
    template <class T>
    auto forward_list<T>::owner_lock_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* n = atomic_ptr.load ();
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
    template <class T>
    inline void forward_list<T>::owner_unlock (std::atomic<node_*>& atomic_ptr, node_*& n)
    {
        Assert (n != nullptr);
        Assert (n != spin ());
        Assert (atomic_ptr.load (std::memory_order_relaxed) == spin ()); // relaxed because it was set to spin by the current thread
        atomic_ptr.store (n, std::memory_order_relaxed);                 // relaxed because observers will see spin
        n = nullptr;                                                     // make sure the caller cant use the pointer anymore
    }
    template <class T>
    auto forward_list<T>::new_ownership_ (std::atomic<node_*>& atomic_ptr) -> node_*
    {
        node_* temp = owner_lock_ (atomic_ptr);
        if (temp == terminal ()) {
            return terminal ();
        }
        node_* result = temp != terminal () ? increment_reference_count (temp) : terminal ();
        owner_unlock (atomic_ptr, temp);
        return result;
    }

}
#endif /* _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_ */
