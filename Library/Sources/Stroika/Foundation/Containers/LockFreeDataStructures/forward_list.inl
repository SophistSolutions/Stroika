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
        : first{terminal ()}
    {
    }
    template <class T>
    inline forward_list<T>::forward_list (forward_list const& src)
        : forward_list{}
    {
        std::atomic<node_*>* nextPtr = &first;
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
        exchange (src.first, first);
    }
    template <class T>
    inline forward_list<T>::~forward_list ()
    {
        clear ();
    }
    template <class T>
    inline bool forward_list<T>::empty () const
    {
        return first.load () == terminal ();
    }
    template <class T>
    int forward_list<T>::clear ()
    {
        node_* current = terminal ();
        // detach the elements first so that blocking is minimal
        exchange (first, current);
        if (current == terminal ()) {
            return 0;
        }
        // if we just delete the first node_, it may cascade down all the
        // subsequent nodes. This would be fine, if not for the possibility
        // of blowing the stack. Instead we delete them in reverse.
        std::vector<node_*> nodes;
        while (current != terminal ()) {
            nodes.push_back (current);
            node_* temp = terminal ();
            // take ownership of the next node_
            exchange (current->next, temp);
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
            exchange (first, i);
            while (i) {
                nodes.push_back (i);
                node_* temp = spin ();
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

}
#endif /* _Stroika_Foundation_Containers_LockFreeDataStructures_forward_list_inl_ */
