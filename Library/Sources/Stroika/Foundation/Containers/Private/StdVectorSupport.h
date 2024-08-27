/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_StdVectorSupport_h_
#define _Stroika_Foundation_Containers_Private_StdVectorSupport_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

#include "Stroika/Foundation/Containers/Common.h"

/**
 *  \file support classes for Concrete classes 'extensions' of behavior specific to the std::vector data structure
 */

namespace Stroika::Foundation::Containers::Private {

    /**
     *  \brief StdVectorBasedContainer is a Stroika implementation detail, but its public methods are fair game and fully supported (as used in subclasses)
     *
     *  This mechanism allows all the array based concrete containers (such as Sequence_StdVector) to all
     *  share the same API and implementation of the API access functions (shrink_to_fit, reserve etc) but without
     *  any genericity implied in the API (just code sharing).
     * 
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    class StdVectorBasedContainer : public BASE_CONTAINER {
    public:
        /**
         */
        template <typename... ARGS>
        StdVectorBasedContainer (ARGS... args);

    public:
        /*
         *  \brief Return the number of allocated vector/array elements.
         * 
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note alias GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note Alias SetCapacity ();
         * 
         *  \note Note that this does not affect the semantics of the container.
         * 
         *  \req slotsAllocated >= size ()
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StdVectorSupport.inl"

#endif /*_Stroika_Foundation_Containers_Private_StdVectorSupport_h_ */
