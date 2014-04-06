/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_h_
#define _Stroika_Foundation_Containers_Deque_h_  1

#include    "../StroikaPreComp.h"

#include    "Queue.h"



/*
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a> -- not even taht yet...
 *
 *
 *  TODO:
 *
 *      @todo   Add CTOR(Iterator<T>) after next release....
 *
 *      @todo   Consider  using the words front/back and push_front/pop_back etc - like with STL
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             *  NOTE - Traits for Queue<T, TRAITS> don't NEED an EQUALS_COMPARER, and the default one should
             *  should be fine (never called if never used).
             *
             *  It will only be invoked if you call
             *      o   Queue<T,TRAITS>::Equals ()
             *
             *  This means that
             *      Queue<SOME_TYPE_WITH_NO_OPERATOR_EQUALS> x;
             *      // works FINE, UNTIL you try to call Equals - and at that point you must adjust
             *      // the traits to specify the Equals() compare function.
             *
             */
            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<T>>
            struct   Deque_DefaultTraits : Queue_DefaultTraits<T, EQUALS_COMPARER> {
            };


            /**
             * Description:
             *
             *      A Deque is a Queue that allows additions and removals at either end.
             *
             *      Deques - like Queues - iterate from Head to Tail.
             *
             * Notes:
             *
             *  <<NOTE - FUTURE WORK - AND DONT DOCUMENT DEFAULTIMPL HERE>>> - SEE FACTORY CODE
             *      We currently default to the circular array implementation, as it is
             *  fastest under most circumstances. One drawback to it is that it has
             *  unpredictable costs for an Add operation. DoubleLinkList is usually
             *  slower, but has very predictable costs.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Deque_DefaultTraits<T>>
            class   Deque : public Queue<T, TRAITS> {
            private:
                using   inherited   =   Queue<T, TRAITS>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Deque<T, TRAITS>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 */
                Deque ();
                Deque (const Deque<T, TRAITS>& src);
                Deque (const std::initializer_list<T>& d);
                template <typename CONTAINER_OF_T>
                explicit Deque (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                /**
                 */
                explicit Deque (const _SharedPtrIRep& rep);

            public:
                /**
                 */
                nonvirtual  Deque<T, TRAITS>&   operator= (const Deque<T, TRAITS>& rhs);

            public:
                /**
                 */
                nonvirtual  void    AddHead (T item);

            public:
                /**
                 */
                nonvirtual  T       RemoveTail ();

            public:
                /**
                 */
                nonvirtual  T       Tail () const;

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Deque<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Deque<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Deque<T, TRAITS>::_IRep : public Queue<T, TRAITS>::_IRep {
            public:
                virtual void        AddHead (T item)                        =   0;
                virtual T           RemoveTail ()                           =   0;
                virtual T           Tail () const                           =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Deque.inl"

#endif  /*_Stroika_Foundation_Containers_Deque_h_ */
