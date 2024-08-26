/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     *********** SkipListBasedContainer<THIS_CONTAINER,BASE_CONTAINER> **************
     ********************************************************************************
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    template <typename... ARGS>
    inline SkipListBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::SkipListBasedContainer (ARGS... args)
        : BASE_CONTAINER{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    inline void SkipListBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::ReBalance ()
    {
        if constexpr (USING_IREP) {
            using _SafeReadWriteRepAccessor =
                THIS_CONTAINER::template _SafeReadWriteRepAccessor<SkipListBasedContainerIRep<typename THIS_CONTAINER::IImplRepBase_>>;
            _SafeReadWriteRepAccessor{this}._GetWriteableRep ().ReBalance ();
        }
        else {
            using _SafeReadWriteRepAccessor = typename BASE_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
            _SafeReadWriteRepAccessor accessor{this};
            return accessor._GetWriteableRep ().fData_.ReBalance ();
        }
    }

    /*
     ********************************************************************************
     *** SkipListBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP> ******
     ********************************************************************************
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    template <typename... ARGS>
    inline SkipListBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::SkipListBasedContainerRepImpl (ARGS... args)
        : BASE_CONTAINER_REP{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void SkipListBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::ReBalance ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.ReBalance ();
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }

}
