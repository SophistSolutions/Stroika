/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     ************* ArrayBasedContainer<THIS_CONTAINER,BASE_CONTAINER> ***************
     ********************************************************************************
     */
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    template <typename... ARGS>
    inline ArrayBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::ArrayBasedContainer (ARGS... args)
        : BASE_CONTAINER{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    inline size_t ArrayBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::capacity () const
    {
        if constexpr (USING_IREP) {
            using _SafeReadRepAccessor = typename THIS_CONTAINER ::template _SafeReadRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
            return _SafeReadRepAccessor{this}._ConstGetRep ().capacity ();
        }
        else {
            using _SafeReadRepAccessor = typename THIS_CONTAINER ::template _SafeReadRepAccessor<typename THIS_CONTAINER::Rep_>;
            return _SafeReadRepAccessor{this}._ConstGetRep ().fData_.capacity ();
        }
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    inline void ArrayBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::reserve (size_t slotsAlloced)
    {
        if constexpr (USING_IREP) {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
            _SafeReadWriteRepAccessor{this}._GetWriteableRep ().reserve (slotsAlloced);
        }
        else {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
            return _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.reserve (slotsAlloced);
        }
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER, bool USING_IREP>
    inline void ArrayBasedContainer<THIS_CONTAINER, BASE_CONTAINER, USING_IREP>::shrink_to_fit ()
    {
        if constexpr (USING_IREP) {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
            _SafeReadWriteRepAccessor{this}._GetWriteableRep ().shrink_to_fit ();
        }
        else {
            using _SafeReadWriteRepAccessor = typename THIS_CONTAINER ::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::Rep_>;
            return _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.shrink_to_fit ();
        }
    }

    /*
     ********************************************************************************
     ***** ArrayBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP> *******
     ********************************************************************************
     */
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    template <typename... ARGS>
    inline ArrayBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::ArrayBasedContainerRepImpl (ARGS... args)
        : BASE_CONTAINER_REP{forward<ARGS> (args)...}
    {
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void ArrayBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::shrink_to_fit ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.shrink_to_fit ();
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    size_t ArrayBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::capacity () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_};
        return Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_.capacity ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void ArrayBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::reserve (size_t slotsAlloced)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.reserve (slotsAlloced);
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }

}
