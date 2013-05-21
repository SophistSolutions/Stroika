/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *      @todo   Embellish test cases (regression tests), and fix/make sure copying works.
 *
 *      @todo   Review
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Stack.hh
 *              for any important lackings
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      Standard FIFO (first in - first out) Stack. See Sedgewick, 30-31.
             *      Iteration proceeds from the top to the bottom of the stack. Top
             *      is the FIRST IN (also first out).
             *
             *  *Design Note*:
             *      We considered NOT having Stack<T> inherit from Iterable<T>, but that made copying of
             *      a stack intrinsically more costly, as you had to copy, and then pop items to see them,
             *      and put them into a new stack. A specail copy API (private to stack) would have limited
             *      the ease of interoperating the Stack<T> container with other sorts of containers.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   Stack : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 *  @todo   MUST WORK OUT DETAILS OF SEMANTICS FOR ITERATOR ADD cuz naieve interpreation of above
                 *          rules owuld lead to having a copy reverse the stack
                 */
                Stack ();
                Stack (const Stack<T>& s);
                template <typename CONTAINER_OF_T>
                explicit Stack (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR>
                explicit Stack (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

            protected:
                explicit Stack (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Stack<T>& operator= (const Stack<T>& src);

            public:
                /**
                 */
                nonvirtual  void    Push (T item);

            public:
                /**
                 */
                nonvirtual  T       Pop ();

            public:
                /**
                 */
                nonvirtual  T       Top () const;

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /*
                 *  Two Stacks are considered equal if they contain the same elements (by comparing them with operator==)
                 *  in exactly the same order.
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this method (UNLIKE THE REST OF SET) requires operator==(T,T) to be defined.
                 *
                 *  Computational Complexity: O(N)
                 */
                nonvirtual  bool    Equals (const Stack<T>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 *      Syntactic sugar for Equals()
                 */
                nonvirtual  bool    operator== (const Stack<T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar for not Equals()
                 */
                nonvirtual  bool    operator!= (const Stack<T>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Stack<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Stack<T> container API.
             */
            template    <typename T>
            class   Stack<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void                Push (T item)                           =   0;
                virtual T                   Pop ()                                  =   0;
                virtual T                   Top () const                            =   0;
                virtual void                RemoveAll ()                            =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Stack.inl"

#endif  /*_Stroika_Foundation_Containers_Stack_h_ */
