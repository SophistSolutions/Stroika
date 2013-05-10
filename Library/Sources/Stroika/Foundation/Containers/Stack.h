/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_  1

/*
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Stack.hh
 *
 *
 *      (o)         Stack<T> should NOT be ITERABLE, but other types like Bag<> and Tally<> sb iterable
 *                  (but tally probably Iterable<TallyEntry<T>>.
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      Standard FIFO (first in - first out) Stack. See Sedgewick, 30-31.
             *      Iteration proceeds from the top to the bottom of the stack. Top
             *      is the FIRST IN.
             *
             *  @todo explain why Stack<T> inherits from Iterable<T> - basically cuz handy (for debugging etc) to be able to traverse and not unreasonable
             *      or makes impl harder.
             *
             */
            template    <typename T>
            class   Stack {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                Stack ();
                Stack (const Stack<T>& s);
                explicit Stack (const T* start, const T* end);

            protected:
                explicit Stack (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Stack<T>& operator= (const Stack<T>& src);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

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
                nonvirtual  Iterable<T>   Elements () const;

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
                virtual void        Push (T item) const                     =   0;
                virtual T           Pop ()                                  =   0;
                virtual T           Top () const                            =   0;
                virtual void        RemoveAll ()                            =   0;
                virtual Iterable<T> Elements () const                       =   0;
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
