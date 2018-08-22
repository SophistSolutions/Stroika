/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
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

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            using Configuration::ArgByValueType;
            using Traversal::Iterable;
            using Traversal::Iterator;

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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename T>
            class Stack : public Iterable<T> {
            private:
                using inherited = Iterable<T>;

            protected:
                class _IRep;

            protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
                using _StackRepSharedPtr = Memory::SharedPtr<_IRep>;
#else
                using _StackRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;
#endif

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = Stack<T>;

            public:
                /**
                 *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Push)
                 *
                 *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
                 */
                template <typename POTENTIALLY_ADDABLE_T>
                static constexpr bool IsAddable = is_convertible<POTENTIALLY_ADDABLE_T, T>::value;

            public:
                /**
                 *  @todo   MUST WORK OUT DETAILS OF SEMANTICS FOR ITERATOR ADD cuz naive interpreation of above
                 *          rules owuld lead to having a copy reverse the stack (SEE FILE-TODO-NOTE)
                 */
                Stack ();
                Stack (const Stack& src) noexcept = default;
                Stack (Stack&& src) noexcept      = default;
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const Stack<T>*>::value>::type>
                Stack (const CONTAINER_OF_ADDABLE& src);
                template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
                Stack (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

            protected:
                explicit Stack (const _StackRepSharedPtr& src) noexcept;
                explicit Stack (_StackRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual Stack<T>& operator= (const Stack<T>& rhs) = default;
                nonvirtual Stack<T>& operator= (Stack<T>&& rhs) = default;

            public:
                /**
                 */
                nonvirtual void Push (ArgByValueType<T> item);

            public:
                /**
                 */
                nonvirtual T Pop ();

            public:
                /**
                 */
                nonvirtual T Top () const;

            public:
                /**
                 */
                nonvirtual void RemoveAll ();

            public:
                /**
                 *  Two Stacks are considered equal if they contain the same elements (by comparing them with EQUALS_COMPARER)
                 *  in exactly the same order.
                 *
                 *  Equals is commutative().
                 *
                 *  A Stack<T> doesn't generally require a comparison for individual elements
                 *  be be defined, but obviously to compare if the containers are equal, you must
                 *  compare the individual elements (at least sometimes).
                 *
                 *  If == is predefined, you can just call Equals() - but if its not, or if you wish
                 *  to compare with an alternative comparer, just pass it as a template parameter.
                 */
                template <typename EQUALS_COMPARER = std::equal_to<T>>
                nonvirtual bool Equals (const Stack& rhs, const EQUALS_COMPARER& equalsComparer = {}) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual void clear ();

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual void _AssertRepValidType () const;
            };

            using Traversal::IteratorOwnerID;

            /**
             *  \brief  Implementation detail for Stack<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Stack<T> container API.
             */
            template <typename T>
            class Stack<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            protected:
                using _StackRepSharedPtr = typename Stack<T>::_StackRepSharedPtr;

            public:
                virtual _StackRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
                virtual void               Push (ArgByValueType<T> item)                          = 0;
                virtual T                  Pop ()                                                 = 0;
                virtual T                  Top () const                                           = 0;
            };

            /**
             *      Syntactic sugar for Equals()
             *
             *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
             *          use a different comparer, call Equals() directly.
             */
            template <typename T>
            bool operator== (const Stack<T>& lhs, const Stack<T>& rhs);

            /**
             *      Syntactic sugar for not Equals()
             *
             *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
             *          use a different comparer, call Equals() directly.
             */
            template <typename T>
            bool operator!= (const Stack<T>& lhs, const Stack<T>& rhs);
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Stack.inl"

#endif /*_Stroika_Foundation_Containers_Stack_h_ */
