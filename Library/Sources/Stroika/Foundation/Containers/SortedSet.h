/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "DefaultTraits/SortedSet.h"
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
            template <typename T, typename TRAITS = DefaultTraits::SortedSet<T>>
            class SortedSet : public Set<T, typename TRAITS::SetTraitsType> {
            private:
                using inherited = Set<T, typename TRAITS::SetTraitsType>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedSet<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of SortedSet<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

            protected:
                class _IRep;

            protected:
                using _SortedSetRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 */
                SortedSet ();
                SortedSet (const SortedSet<T, TRAITS>& src) noexcept;
                SortedSet (SortedSet<T, TRAITS>&& src) noexcept;
                SortedSet (const initializer_list<T>& src);
                template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value and not std::is_convertible<const CONTAINER_OF_T*, const SortedSet<T, TRAITS>*>::value>::type>
                SortedSet (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedSet (const _SortedSetRepSharedPtr& src) noexcept;
                explicit SortedSet (_SortedSetRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedSet<T, TRAITS>& operator= (const SortedSet<T, TRAITS>& rhs) = default;
                nonvirtual SortedSet<T, TRAITS>& operator= (SortedSet<T, TRAITS>&& rhs) = default;

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
             *  \brief  Implementation detail for SortedSet<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedSet<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template <typename T, typename TRAITS>
            class SortedSet<T, TRAITS>::_IRep : public Set<T, typename TRAITS::SetTraitsType>::_IRep {
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
