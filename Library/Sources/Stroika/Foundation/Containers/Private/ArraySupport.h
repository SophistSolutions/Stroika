/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_ArraySupport_h_
#define _Stroika_Foundation_Containers_Private_ArraySupport_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/DataStructures/Array.h"

/**
 *  \file
 *      support classes for Concrete classes 'extensions' of behavior specific to the Array data structure
 */

namespace Stroika::Foundation::Containers::Private {

    /**
     *  \brief ArrayBasedContainer is a Stroika implementation detail, but its public methods are fair game and fully supported (as used in subclasses)
     *
     *  This mechanism allows all the array based concrete containers (such as Set_Array, Sequence_Array) to all
     *  share the same API and implementation of the API access functions (shrink_to_fit, reserve etc) but without
     *  any genericity implied in the API (just code sharing).
     * 
     *  \note bool USING_IREP:
     *      Can we just peek from ArrayBasedContainer<> into the fData_ and do the array operations? Yes, if Rep_ is not
     *      type-erased, but no otherwise. If we specify false, we can avoid putting references to the ArrayBasedContainerIRep
     *      into the vtable, and linking a bunch of often not used code. But if we've got extra template parameters to the Rep_,
     *      we cannot peek at it from this container, so must indirect.
     * 
     *  \par Example Usage
     *      \code
     *          class Association_Array : public Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE> {
     *          using inherited = Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>;
     *          // BECOMES
     *          class Association_Array : public Private::ArrayBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true> {
     *          using inherited = Private::ArrayBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true>;
     * 
     *          // AND need in main public container, 
     *          friend inherited;       // for ArrayBasedContainer
     *      \endcode
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    class ArrayBasedContainer : public BASE_CONTAINER {
    public:
        /**
         */
        using BASE_CONTAINER::BASE_CONTAINER;

    public:
        /*
         *  \brief Return the number of allocated vector/array elements.
         * 
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  @aliases GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  @aliases SetCapacity ();
         * 
         *  \note Note that this does not affect the semantics of the container.
         * 
         *  \pre slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the container contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete container, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();
    };

    /**
     *  \brief impl detail for array based container support (see ArrayBasedContainer docs on bool USING_IREP)
     * 
     *  \par Example Usage
     *      \code
     *          //using IImplRepBase_ = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;      // BECOMES
     *          using IImplRepBase_ = Containers::Private::ArrayBasedContainerIRep<    typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep     >;
     *      \endcode
     */
    template <typename CONTAINER_REP_BASE_CLASS>
    class ArrayBasedContainerIRep : public CONTAINER_REP_BASE_CLASS {
    public:
        virtual void   shrink_to_fit ()              = 0;
        virtual size_t capacity () const             = 0;
        virtual void   reserve (size_t slotsAlloced) = 0;
    };

    /**
     *  \brief CRTP applied when ArrayBasedContainerIRep used
     * 
     *  \par Example Usage
     *      \code
     *          template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
     *          class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
     *              : public Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_,
     *              public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
     *                  using inherited = Rep_<KEY_EQUALS_COMPARER>;
     *          // BECOMES
     *          template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
     *              class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
     *              : public Private::ArrayBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>,
     *              public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
     *              using inherited = Private::ArrayBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>;
     * 
     *          // AND need in Rep_, 
     *          friend inherited;       // for ArrayBasedContainerRepImpl
     *      \endcode
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    class ArrayBasedContainerRepImpl : public BASE_CONTAINER_REP {
    public:
        /**
         */
        using BASE_CONTAINER_REP::BASE_CONTAINER_REP;
        virtual void   shrink_to_fit () override;
        virtual size_t capacity () const override;
        virtual void   reserve (size_t slotsAlloced) override;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ArraySupport.inl"

#endif /*_Stroika_Foundation_Containers_Private_ArraySupport_h_ */
