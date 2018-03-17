/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_SortedMultiSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "DefaultTraits/SortedMultiSet.h"
#include "MultiSet.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /**
             *      A SortedMultiSet is a MultiSet<T, TRAITS> which remains sorted (iterator).
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  \note   \em Thread-Safety   xxxxxxno-revisuit<a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \req    RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
             *
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             */
            template <typename T, typename TRAITS = DefaultTraits::SortedMultiSet<T>>
            class SortedMultiSet : public MultiSet<T, typename TRAITS::MultisetTraitsType> {
            private:
                using inherited = MultiSet<T, typename TRAITS::MultisetTraitsType>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedMultiSet<T, TRAITS>;

            protected:
                class _IRep;

            protected:
                using _SortedMultiSetRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                SortedMultiSet ();
                SortedMultiSet (const SortedMultiSet& src) noexcept = default;
                SortedMultiSet (SortedMultiSet&& src) noexcept      = default;
                SortedMultiSet (const initializer_list<T>& src);
                SortedMultiSet (const initializer_list<CountedValue<T>>& src);
                template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value and not std::is_convertible<const CONTAINER_OF_T*, const SortedMultiSet<T, TRAITS>*>::value>::type>
                SortedMultiSet (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                SortedMultiSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedMultiSet (const _SortedMultiSetRepSharedPtr& src) noexcept;
                explicit SortedMultiSet (_SortedMultiSetRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedMultiSet<T, TRAITS>& operator= (const SortedMultiSet<T, TRAITS>& rhs) = default;
                nonvirtual SortedMultiSet<T, TRAITS>& operator= (SortedMultiSet<T, TRAITS>&& rhs) = default;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

            public:
                RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

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
             *  \brief  Implementation detail for SortedMultiSet<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMultiSet<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template <typename T, typename TRAITS>
            class SortedMultiSet<T, TRAITS>::_IRep : public MultiSet<T, typename TRAITS::MultisetTraitsType>::_IRep {
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMultiSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedMultiSet_h_ */
