/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/TypeHints.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Memory/BlockAllocated.h"

#include "../Common.h"

/**
 *
 *
 * TODO:
 *
 *      @todo   TOTALLY UNIMPLEMENTED
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DataStructures {

                using Configuration::ArgByValueType;

                /*
                 *      https://en.wikipedia.org/wiki/Non-blocking_linked_list
                 *
                 *      or better � Herb Sutter provides a C++ implementation:
                 *      https://www.infoq.com/news/2014/10/cpp-lock-free-programming
                 */
                template <typename T>
                class LinkedList : public Debug::AssertExternallySynchronizedLock {
                public:
                    using value_type = T;

                public:
                    LinkedList ();
                    LinkedList (const LinkedList<T, TRAITS>& from);
                    ~LinkedList ();

                public:
                };

#if 0
                template
                struct Node { T t; shared_ptr<Node> next; };
                atomic<shared_ptr<Node>> head;

public:
                slist() = default;
                ~slist() = default;

                class reference {
                    shared_ptr<Node> p;
                public:
                    reference(shared_ptr<Node> p_) : p{_p} {}
                    T& operator*() { return p->t; }
                    T* operator->() { return &p->t; }
                };
                auto find(T t) const
                {
                    auto p = head.load();
                    while (p && p->t != t)
                        p = p->next;
                    return reference{move(p)};

                    void push_front(T t) {
                        auto p = make_shared<Node>();
                        p->t = t;
                        p->next = head;
                        while (head.compare_exchange_weak(p->next, p))
                        {}
                    }

                    void pop_front() {
                        auto p = head.load();
                        while (p && !head.compare_exchange_weak(p, p->next))
                        {}
                    }
                };
#endif
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_ */
