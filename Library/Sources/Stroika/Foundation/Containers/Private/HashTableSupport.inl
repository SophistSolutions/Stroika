/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     *********** HashTableBasedContainer<THIS_CONTAINER,BASE_CONTAINER> **************
     ********************************************************************************
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    template <typename... ARGS>
    inline HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::HashTableBasedContainer (ARGS... args)
        : BASE_CONTAINER{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    inline void HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::ReBalance ()
    {
        if constexpr (USING_IREP) {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
            _SafeReadWriteRepAccessor{this}._GetWriteableRep ().ReBalance ();
        }
        else {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
            return _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.ReBalance ();
        }
    }

    /*
     ********************************************************************************
     *** HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP> ******
     ********************************************************************************
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    template <typename... ARGS>
    inline HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::HashTableBasedContainerRepImpl (ARGS... args)
        : BASE_CONTAINER_REP{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::ReBalance ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.ReBalance ();
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }

}
