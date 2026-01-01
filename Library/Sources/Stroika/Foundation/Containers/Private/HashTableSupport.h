/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_HashTableSupport_h_
#define _Stroika_Foundation_Containers_Private_HashTableSupport_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/DataStructures/HashTable.h"

/**
 *  \file 
 *      support classes for Concrete classes 'extensions' of behavior specific to the HashTable data structure
 */

namespace Stroika::Foundation::Containers::Private {

    /**
     *  \brief HashTableBasedContainer is a Stroika implementation detail, but its public methods are fair game and fully supported (as used in subclasses)
     *
     *  This mechanism allows all the array based concrete containers (such as Set_HashTable, Sequence_HashTable) to all
     *  share the same API and implementation of the API access functions (ReBalance etc) but without
     *  any genericity implied in the API (just code sharing).
     * 
     *  \par Example Usage
     *      \code
     *          class Association_Array : public  Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
     *          using inherited =  Association<KEY_TYPE, MAPPED_VALUE_TYPE>;
     *          // BECOMES
     *          class Association_Array : public Private::HashTableBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true> {
     *          using inherited = Private::HashTableBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true>;
     * 
     *          // AND need in main public container, 
     *          friend inherited;       // for HashTableBasedContainer
     *      \endcode
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    class HashTableBasedContainer : public BASE_CONTAINER {
    public:
        /**
         */
        using BASE_CONTAINER::BASE_CONTAINER;

    public:
        /**
         *  \brief 
         */
        nonvirtual void ReHash (size_t newBucketCount);

    public:
        /**
         *  \brief 
         */
        nonvirtual void ReHashIfNeeded ();

    public:
        /**
         *  \brief 
         */
        nonvirtual size_t bucket_count () const;

    public:
        /**
         *  \brief 
         */
        nonvirtual size_t bucket_size (size_t bucketIdx) const;

    public:
        /**
         *  \brief 
         */
        nonvirtual float load_factor () const;

    public:
        /**
         *  \brief 
         */
        nonvirtual float max_load_factor () const;

    public:
        /**
         *  \brief 
         */
        nonvirtual void max_load_factor (float mlf);
    };

    /**
     *  \brief impl detail for array based container support
     * 
     *  \par Example Usage
     *      \code
     *          // In Association_HashTable template
     *          //using IImplRepBase_ = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;      // BECOMES
     *          using IImplRepBase_ = Private::HashTableBasedContainerIRep<    typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep     >;
     *      \endcode
     */
    template <typename CONTAINER_REP_BASE_CLASS>
    class HashTableBasedContainerIRep : public CONTAINER_REP_BASE_CLASS {
    public:
        virtual void   ReHash (size_t newBucketCount)       = 0;
        virtual void   ReHashIfNeeded ()                    = 0;
        virtual size_t bucket_count () const                = 0;
        virtual size_t bucket_size (size_t bucketIdx) const = 0;
        virtual float  load_factor () const                 = 0;
        virtual float  max_load_factor () const             = 0;
        virtual void   max_load_factor (float mlf)          = 0;
    };

    /**
     *  \brief CRTP applied when HashTableBasedContainerIRep used
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
     *              : public Private::HashTableBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>,
     *              public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
     *              using inherited = Private::HashTableBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>;
     * 
     *          // AND need in Rep_, 
     *          friend inherited;       // for HashTableBasedContainerRepImpl
     *      \endcode
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    class HashTableBasedContainerRepImpl : public BASE_CONTAINER_REP {
    public:
        /**
         */
        using BASE_CONTAINER_REP::BASE_CONTAINER_REP;
        virtual void   ReHash (size_t newBucketCount) override;
        virtual void   ReHashIfNeeded () override;
        virtual size_t bucket_count () const override;
        virtual size_t bucket_size (size_t bucketIdx) const override;
        virtual float  load_factor () const override;
        virtual float  max_load_factor () const override;
        virtual void   max_load_factor (float mlf) override;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HashTableSupport.inl"

#endif /*_Stroika_Foundation_Containers_Private_HashTableSupport_h_ */
