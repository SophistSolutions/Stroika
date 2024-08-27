/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     ********* StdVectorBasedContainer<THIS_CONTAINER,BASE_CONTAINER> ***************
     ********************************************************************************
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    template <typename... ARGS>
    inline StdVectorBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::StdVectorBasedContainer (ARGS... args)
        : BASE_CONTAINER{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline size_t StdVectorBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::capacity () const
    {

        using _SafeReadRepAccessor = typename THIS_CONTAINER ::template _SafeReadRepAccessor<typename THIS_CONTAINER::Rep_>;
        return _SafeReadRepAccessor{this}._ConstGetRep ().fData_.capacity ();
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline void StdVectorBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::reserve (size_t slotsAlloced)
    {

        using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
        return _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.reserve (slotsAlloced);
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline void StdVectorBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::shrink_to_fit ()
    {

        using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
        return _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.shrink_to_fit ();
    }

}
