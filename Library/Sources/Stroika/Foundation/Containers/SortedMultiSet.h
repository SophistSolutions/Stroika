/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_SortedMultiSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "DefaultTraits/MultiSet.h"
#include "MultiSet.h"

/**
 *  \file
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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             */
            template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
            class SortedMultiSet : public MultiSet<T, TRAITS> {
            private:
                using inherited = MultiSet<T, TRAITS>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedMultiSet<T, TRAITS>;

            protected:
                class _IRep;

            protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
                using _SortedMultiSetRepSharedPtr = Memory::SharedPtr<_IRep>;
#else
                using _SortedMultiSetRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;
#endif

            public:
                /**
                 *  Ordering reletion applied to sort a 'SortedMultiSet'. Returned by GetInOrderComparer ();
                 */
                using InOrderComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of SortedMultiSet<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 */
                SortedMultiSet ();
                template <typename INORDER_COMPARER, typename ENABLE_IF = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                explicit SortedMultiSet (INORDER_COMPARER&& inorderComparer, ENABLE_IF* = nullptr);
                SortedMultiSet (const SortedMultiSet& src) noexcept = default;
                SortedMultiSet (SortedMultiSet&& src) noexcept      = default;
                SortedMultiSet (const initializer_list<T>& src);
                template <typename INORDER_COMPARER, typename ENABLE_IF = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src);
                SortedMultiSet (const initializer_list<CountedValue<T>>& src);
                template <typename INORDER_COMPARER, typename ENABLE_IF = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<CountedValue<T>>& src);
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedMultiSet<T, TRAITS>*>::value>::type>
                SortedMultiSet (const CONTAINER_OF_ADDABLE& src);
                template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedMultiSet<T, TRAITS>*>::value>::type>
                SortedMultiSet (INORDER_COMPARER&& inorderComparer, const CONTAINER_OF_ADDABLE& src);
                template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF = enable_if_t<Configuration::is_iterator<COPY_FROM_ITERATOR_OF_ADDABLE>::value>>
                SortedMultiSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
                template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::is_iterator<COPY_FROM_ITERATOR_OF_ADDABLE>::value>>
                SortedMultiSet (INORDER_COMPARER&& inorderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

            protected:
                explicit SortedMultiSet (const _SortedMultiSetRepSharedPtr& src) noexcept;
                explicit SortedMultiSet (_SortedMultiSetRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedMultiSet& operator= (const SortedMultiSet& rhs) = default;
                nonvirtual SortedMultiSet& operator= (SortedMultiSet&& rhs) = default;

            public:
                /**
                 *  Return the function used to compare if two elements are in-order (sorted properly)
                 */
                nonvirtual InOrderComparerType GetInOrderComparer () const;

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
            class SortedMultiSet<T, TRAITS>::_IRep : public MultiSet<T, TRAITS>::_IRep {
            public:
                virtual InOrderComparerType GetInOrderComparer () const = 0;
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
