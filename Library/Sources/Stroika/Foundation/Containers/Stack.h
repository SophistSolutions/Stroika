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
             *
             *
             */
            template    <typename T>
            class   Stack : public Iterable<T> {
            public:
                Stack ();
                Stack (const Stack<T>& src);

            protected:
                Stack (StackRep<T>* src);

            public:
                nonvirtual  Stack<T>& operator= (const Stack<T>& src);

            public:
                nonvirtual  size_t  GetLength () const;

            public:
                nonvirtual  Boolean IsEmpty () const;

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  void    Compact ();

            public:
                nonvirtual  operator Iterator<T> () const;

            public:
                nonvirtual  void    Push (T item);

            public:
                nonvirtual  T       Pop ();

            public:
                nonvirtual  T       Top () const;

            protected:
                nonvirtual  const StackRep<T>*  GetRep () const;
                nonvirtual  StackRep<T>*        GetRep ();

            private:
                Shared<StackRep<T> >    fRep;

                static  StackRep<T>*    Clone (const StackRep<T>& src);

                friend  Boolean operator== (const Stack<T>& lhs, const Stack<T>& rhs);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Stack_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Stack.inl"
