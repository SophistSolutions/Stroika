/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Set.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Support Iterable<>::Where overload?
 *
 *      @todo   CRITICAL - need version where you can pass in a lambda to compare two things for <, so
 *              easy to construct a sorted set with your own sorter function!!!
 *
 *              This applies equally to other Stroika sorted types
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /**
             *      A SortedSet is a Set<T> which remains sorted (iterator).
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename T>
            class SortedSet : public Set<T> {
            private:
                using inherited = Set<T>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedSet<T>;

            protected:
                class _IRep;

            protected:
                using _SortedSetRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 */
                using EqualityComparerType = typename inherited::EqualityComparerType;

            public:
                /**
                 */
                SortedSet ();
                template <typename LESS_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Configuration::is_callable<LESS_COMPARER>::value>>
                explicit SortedSet (const LESS_COMPARER& lessComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                SortedSet (const SortedSet& src) noexcept = default;
                SortedSet (SortedSet&& src) noexcept      = default;
                SortedSet (const initializer_list<T>& src);
                SortedSet (const EqualityComparerType& equalityComparer, const initializer_list<T>& src);
                template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value and not std::is_convertible<const CONTAINER_OF_T*, const SortedSet*>::value>>
                SortedSet (const CONTAINER_OF_T& src);
                template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value and not std::is_convertible<const CONTAINER_OF_T*, const SortedSet*>::value>>
                SortedSet (const EqualityComparerType& equalityComparer, const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T, typename ENABLE_IF = enable_if_t<Configuration::is_iterator<COPY_FROM_ITERATOR_OF_T>::value>>
                SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template <typename COPY_FROM_ITERATOR_OF_T, typename ENABLE_IF = enable_if_t<Configuration::is_iterator<COPY_FROM_ITERATOR_OF_T>::value>>
                SortedSet (const EqualityComparerType& equalityComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedSet (const _SortedSetRepSharedPtr& src) noexcept;
                explicit SortedSet (_SortedSetRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedSet& operator= (const SortedSet& rhs) = default;
                nonvirtual SortedSet& operator= (SortedSet&& rhs) = default;

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

            /**
             *  \brief  Implementation detail for SortedSet<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedSet<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template <typename T>
            class SortedSet<T>::_IRep : public Set<T>::_IRep {
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedSet_h_ */
