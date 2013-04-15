/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_h_
#define _Stroika_Foundation_Containers_Deque_h_  1

/*
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *  TODO:
 *
 *      @todo         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Deque.hh
 *
 *      @todo   Consider  using the words front/back and push_front/pop_back etc - like with STL
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             * Description:
             *
             *      A Deque is a queue that allows additions and removals at either end.
             *
             *      Deques always iterate from Head to Tail.
             *
             *      We currently default to the circular array implementation, as it is
             *  fastest under most circumstances. One drawback to it is that it has
             *  unpredictable costs for an Add operation. DoubleLinkList is usually
             *  slower, but has very predictable costs.
             *
             * TODO:
             *      +   Add CTOR(Iterator<T>) after next release....
             *
             * Notes:
             *
             *      We currently default to the circular array implementation, as it is
             *  fastest under most circumstances. One drawback to it is that it has
             *  unpredictable costs for an Add operation. DoubleLinkList is usually
             *  slower, but has very predictable costs.
             *
            */
            template    <tyoename T>
            class   Deque : public Iterable<T> {
            public:
                Deque ();
                Deque (const Deque<T>& src);

            protected:
                Deque (DequeRep<T>* rep);

            public:
                nonvirtual  Deque<T>& operator= (const Deque<T>& src);

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  void    Compact ();

            public:
                nonvirtual  void    AddHead (T item);

            public:
                nonvirtual  T       RemoveHead ();

            public:
                nonvirtual  T       Head () const;

            public:
                nonvirtual  void    AddTail (T item);

            public:
                nonvirtual  T       RemoveTail ();

            public:
                nonvirtual  T       Tail () const;

            protected:
                nonvirtual  const DequeRep<T>*  GetRep () const;
                nonvirtual  DequeRep<T>*        GetRep ();

            private:
                Shared<DequeRep<T> >    fRep;

                static  DequeRep<T>*    Clone (const DequeRep<T>& rep);

                friend  Boolean operator== (const Deque<T>& lhs, const Deque<T>& rhs);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Deque_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Deque.inl"
