/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_  1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Traversal/Iterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *      @todo   Add AddAll()  for now - that creates a temporary stack to revserse. But then use
 *              enable_if - to check if you can create reverse iterator, and then use that to go backwards?
 *
 *      @todo   Actual implmeentaitons incomplete - especailly cuz CTOR with params arg ambiguious -
 *              clarify... and implement copy
 *
 *              >>> I THINK DEFINITION TO USE FOR ITERATOR ORDER/CTOR IS:
 *              >>>     >   Iterates from TOP OF STACK to LAST (so natural order data structured organinized for)
 *              >>>     >   CTOR order is TOP of STACK to LAST (same) so easy to pass in regular iterators
 *              >>>     BUT! THen implementing CTOR of REPS is tricky cuz they must be responsible to effectively 'reverse'
 *              >>>     OR DO ADDS THEMESELVES
 *
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


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             *  NOTE - Traits for Stack<T> don't NEED an EQUALS_COMPARER, and the default one should
             *  should be fine (never called if never used).
             *
             *  It will only be invoked if you call
             *      o   Stack<T,TRAITS>::Equals ()
             *
             *  This means that
             *      Stack<SOME_TYPE_WITH_NO_OPERATOR_EQUALS> x;
             *      // works FINE, UNTIL you try to call Equals - and at that point you must adjust
             *      // the traits to specify the Equals() compare function.
             *
             */
            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<T>>
            struct   Stack_DefaultTraits {
                /**
                 */
                using   EqualsCompareFunctionType   =   EQUALS_COMPARER;
            };


            /**
             *      Standard LIFO (Last in first out) Stack. See Sedgewick, 30-31.
             *      Iteration proceeds from the top to the bottom of the stack. Top
             *      is the FIRST IN (also first out).
             *
             *  *Design Note*:
             *      We considered NOT having Stack<T> inherit from Iterable<T>, but that made copying of
             *      a stack intrinsically more costly, as you had to copy, and then pop items to see them,
             *      and put them into a new stack. A special copy API (private to stack) would have limited
             *      the ease of interoperating the Stack<T> container with other sorts of containers.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Stack_DefaultTraits<T>>
            class   Stack : public Iterable<T> {
            private:
                using   inherited   =   Iterable<T>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template _USING_SHARED_IMPL_<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   Stack<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Stack<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using   TraitsType                  =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 *
                 *  Note - though the type must exist, the implied 'Equals' function may never be compiled (so can be invalid)
                 *  if you avoid the documented methods (see EqualsCompareFunctionType above).
                 */
                using   EqualsCompareFunctionType   =   typename TraitsType::EqualsCompareFunctionType;

            public:
                /**
                 *  @todo   MUST WORK OUT DETAILS OF SEMANTICS FOR ITERATOR ADD cuz naieve interpreation of above
                 *          rules owuld lead to having a copy reverse the stack (SEE FILE-TODO-NOTE)
                 */
                Stack ();
                Stack (const Stack<T, TRAITS>& s);
                template <typename CONTAINER_OF_T>
                explicit Stack (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Stack (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Stack (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Stack<T, TRAITS>& operator= (const Stack<T, TRAITS>& src);

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
                 *  \req RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                 *
                 *  Computational Complexity: O(N)
                 */
                nonvirtual  bool    Equals (const Stack<T, TRAITS>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 *      Syntactic sugar for Equals()
                 */
                nonvirtual  bool    operator== (const Stack<T, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar for not Equals()
                 */
                nonvirtual  bool    operator!= (const Stack<T, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Stack<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Stack<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Stack<T, TRAITS>::_IRep : public Iterable<T>::_IRep {
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
