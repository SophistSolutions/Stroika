/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_SortedAssociation_h_
#define _Stroika_Foundation_Containers_SortedAssociation_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Association.h"

/**
 *
 *
 **  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
 *
 *
 *
 *  TODO:
 *      @todo   Add MOVE CTOR (low priority)
 */

namespace Stroika::Foundation::Containers {

    /**
     *      A SortedAssociation is a Association<Key,T> which remains sorted (iterator) by the Key.
     *
     *  Note - this class might have been called "Dictionary".
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  @see    Association<Key,T>
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
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    protected:
        class _IRep;

    protected:
        using _SortedAssociationRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

    public:
        /**
        *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
        */
        using ArchetypeContainerType = SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
        *  This CAN be used as the argument to a SortedAssociation<> as InOrderComparerType, but
        *  we allow any template in the SortedSet<> CTOR for an inorderComparer that follows Common::IsStrictInOrderComparer () concept
        */
        using InOrderKeyComparerType = Common::ComparisonRelationDeclaration<function<bool(KEY_TYPE, KEY_TYPE)>, Common::ComparisonRelationType::eStrictInOrder>;

    public:
        /**
        *  This constructor creates a concrete sorted Association object, either empty,
        *  or initialized with any argument values.
        *
        *  The underlying data structure of the Association is defined by @see Factory::Association_Factory<>
        */
        SortedAssociation ();
        SortedAssociation (const SortedAssociation& src) noexcept = default;
        SortedAssociation (SortedAssociation&& src) noexcept      = default;
        SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        SortedAssociation (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<(Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> or Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>)and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
        SortedAssociation (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    protected:
        explicit SortedAssociation (const _SortedAssociationRepSharedPtr& src) noexcept;
        explicit SortedAssociation (_SortedAssociationRepSharedPtr&& src) noexcept;

    public:
        /**
        */
        nonvirtual SortedAssociation& operator= (const SortedAssociation& rhs) = default;
        nonvirtual SortedAssociation& operator= (SortedAssociation&& rhs) = default;

    public:
        /**
        */
        nonvirtual InOrderKeyComparerType GetInOrderKeyComparer () const
        {
            // tmphack
            return InOrderKeyComparerType{less<KEY_TYPE>{}};
        }

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
    *  \brief  Implementation detail for SortedAssociation<T> implementors.
    *
    *  Protected abstract interface to support concrete implementations of
    *  the SortedAssociation<T> container API.
    *
    *  Note that this doesn't add any methods, but still serves the purpose of allowing
    *  testing/validation that the subtype information is correct (it is sorted).
    */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedAssociation.inl"

#endif /*_Stroika_Foundation_Containers_SortedAssociation_h_ */
