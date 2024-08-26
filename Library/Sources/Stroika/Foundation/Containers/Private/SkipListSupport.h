/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_SkipListSupport_h_
#define _Stroika_Foundation_Containers_Private_SkipListSupport_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"

/**
 *  \file support classes for Concrete classes 'extensions' of behavior specific to the SkipList data structure
 */

namespace Stroika::Foundation::Containers::Private {

    /**
     *  \brief SkipListBasedContainer is a Stroika implementation detail, but its public methods are fair game and fully supported (as used in subclasses)
     *
     *  This mechanism allows all the array based concrete containers (such as Set_SkipList, Sequence_SkipList) to all
     *  share the same API and implementation of the API access functions (ReBalance etc) but without
     *  any genericity implied in the API (just code sharing).
     * 
     *  \note bool USING_IREP:
     *      Can we just peek from SkipListBasedContainer<> into the fData_ and do the array operations? Yes, if Rep_ is not
     *      type-erased, but no otherwise. If we specify false, we can avoid putting references to the SkipListBasedContainerIRep
     *      into the vtable, and linking a bunch of often not used code. But if we've got extra template parameters to the Rep_,
     *      we cannot peek at it from this container, so must indirect.
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    class SkipListBasedContainer : public BASE_CONTAINER {
    public:
        /**
         */
        template <typename... ARGS>
        SkipListBasedContainer (ARGS... args);

    public:
        /**
         *  \brief 
         */
        nonvirtual void ReBalance ();
    };

    /**
     *  \brief impl detail for array based container support (see SkipListBasedContainer docs on bool USING_IREP)
     */
    template <typename CONTAINER_REP_BASE_CLASS>
    class SkipListBasedContainerIRep : public CONTAINER_REP_BASE_CLASS {
    public:
        virtual void ReBalance () = 0;
    };

    /**
     *  \brief CRTP applied when SkipListBasedContainerIRep used
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    class SkipListBasedContainerRepImpl : public BASE_CONTAINER_REP {
    public:
        /**
         */
        template <typename... ARGS>
        SkipListBasedContainerRepImpl (ARGS... args);
        virtual void ReBalance () override;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SkipListSupport.inl"

#endif /*_Stroika_Foundation_Containers_Private_SkipListSupport_h_ */
