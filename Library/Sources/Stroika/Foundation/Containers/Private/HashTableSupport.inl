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
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline size_t HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::bucket_count () const
    {
        using _SafeReadRepAccessor = typename THIS_CONTAINER::template _SafeReadRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        return _SafeReadRepAccessor{this}._GetReadableRep ().bucket_count ();
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline size_t HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::bucket_size (size_t bucketIdx) const
    {
        using _SafeReadRepAccessor = typename THIS_CONTAINER::template _SafeReadRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        return _SafeReadRepAccessor{this}._GetReadableRep ().bucket_size (bucketIdx);
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline float HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::load_factor () const
    {
        using _SafeReadRepAccessor = typename THIS_CONTAINER::template _SafeReadRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        return _SafeReadRepAccessor{this}._GetReadableRep ().load_factor ();
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline float HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::max_load_factor () const
    {
        using _SafeReadRepAccessor = typename THIS_CONTAINER::template _SafeReadRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        return _SafeReadRepAccessor{this}._GetReadableRep ().max_load_factor ();
    }
    template <typename THIS_CONTAINER, typename BASE_CONTAINER>
    inline void HashTableBasedContainer<THIS_CONTAINER, BASE_CONTAINER>::max_load_factor (float mlf)
    {
        using _SafeReadWriteRepAccessor = typename THIS_CONTAINER::template _SafeReadWriteRepAccessor<typename THIS_CONTAINER::IImplRepBase_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().max_load_factor (mlf);
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
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    size_t HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::bucket_count () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_};
        return Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_.bucket_count ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    size_t HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::bucket_size (size_t bucketIdx) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_};
        return Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_.bucket_size (bucketIdx);
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    float HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::load_factor () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_};
        return Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_.load_factor ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    float HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::max_load_factor () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_};
        return Debug::UncheckedDynamicCast<const THIS_CONTAINER_REP*> (this)->fData_.max_load_factor ();
    }
    template <typename THIS_CONTAINER_REP, typename BASE_CONTAINER_REP>
    void HashTableBasedContainerRepImpl<THIS_CONTAINER_REP, BASE_CONTAINER_REP>::max_load_factor (float mlf)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_};
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fData_.max_load_factor (mlf);
        Debug::UncheckedDynamicCast<THIS_CONTAINER_REP*> (this)->fChangeCounts_.PerformedChange ();
    }

}
