/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     *********** HashTableBasedContainer<THIS_CONTAINER,BASE_CONTAINER> *************
     ********************************************************************************
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline void HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::ReHash (size_t newBucketCount)
    {
        using _SafeReadWriteRepAccessor = typename THIS_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().ReHash (newBucketCount);
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline void HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::ReHashIfNeeded ()
    {
        using _SafeReadWriteRepAccessor = typename THIS_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().ReHashIfNeeded ();
    }

    /*
     ********************************************************************************
     *** HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP> *****
     ********************************************************************************
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::ReHash (size_t newBucketCount)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.ReHash (newBucketCount);
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::ReHashIfNeeded ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.ReHashIfNeeded ();
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }

}
