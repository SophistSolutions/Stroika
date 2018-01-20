/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_MemoryAllocator_h_
#define _Stroika_Foundation_Memory_MemoryAllocator_h_ 1

#include "../StroikaPreComp.h"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>

#include "Common.h"

/**
 *  \file
 *
 *      @todo   REDO operator== etc using non-member functions
 *              (see See coding conventions document about operator usage: Compare () and operator<, operator>, etc comments)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            /**
             * This defines a generic abstract 'Allocator' API - for allocating (and freeing) memory.
             * This is far simpler than than the STL/stdC++ allocator policy (for example, not templated, and uses C model
             * of memory - just a byte pointer and amount).
             *
             *  @todo   Consider if we should have a notion of alignment, or if any of the other stdc++ allocator stuff is
             *          needed  here...
             */
            class AbstractGeneralPurposeAllocator {
            protected:
                virtual ~AbstractGeneralPurposeAllocator () = default;

                // Allocate never returns nullptr and Deallocate () should not be called with nullptr
                // Allocate throws bad_alloc () on failure to allocate
            public:
                virtual void* Allocate (size_t size) = 0;
                virtual void  Deallocate (void* p)   = 0;
            };

            /**
             * SimpleAllocator_CallLIBCMallocFree implements the AbstractGeneralPurposeAllocator just using malloc and free.
             */
            class SimpleAllocator_CallLIBCMallocFree : public AbstractGeneralPurposeAllocator {
            public:
                virtual void* Allocate (size_t size) override;
                virtual void  Deallocate (void* p) override;
            };

            /**
             * SimpleAllocator_CallLIBCNewDelete implements the AbstractGeneralPurposeAllocator just using stdC++ new/delete.
             */
            class SimpleAllocator_CallLIBCNewDelete : public AbstractGeneralPurposeAllocator {
            public:
                virtual void* Allocate (size_t size) override;
                virtual void  Deallocate (void* p) override;
            };

            /**
             * The STLAllocator takes a Stroika Allocator class (as template argument) and maps it
             * for usage as an STL-style allocator.
             *
             *  @todo   unsure about how to handle propagate_on_container_copy_assignment, etc. For all my current allocators, this
             *          is fine (false), but if they had data, obviously it would be different. Perhaps that choice should be
             *          parametized (as it is with normal C++ allocators). Or maybe leave subclassing this STLAllocator<> as a way
             *          todo that? Thats probably good enuf...
             */
            template <typename T, typename BASE_ALLOCATOR = SimpleAllocator_CallLIBCMallocFree>
            class STLAllocator {
            public:
                using other                                  = STLAllocator<T, BASE_ALLOCATOR>;
                using value_type                             = T;
                using pointer                                = value_type*;
                using const_pointer                          = const value_type*;
                using void_pointer                           = void*;
                using const_void_pointer                     = const void*;
                using reference                              = value_type&;
                using const_reference                        = const value_type&;
                using size_type                              = size_t;
                using difference_type                        = ptrdiff_t;
                using propagate_on_container_copy_assignment = false_type;
                using propagate_on_container_move_assignment = false_type;
                using propagate_on_container_swap            = false_type;

            public:
                template <typename OTHER>
                struct rebind {
                    using other = STLAllocator<OTHER, BASE_ALLOCATOR>;
                };

            public:
                BASE_ALLOCATOR fBaseAllocator;

            public:
                explicit STLAllocator ();
                STLAllocator (const STLAllocator<T, BASE_ALLOCATOR>& from);
                template <typename OTHER>
                STLAllocator (const STLAllocator<OTHER, BASE_ALLOCATOR>& from);
                template <typename OTHER>
                nonvirtual STLAllocator<T, BASE_ALLOCATOR>& operator= (const STLAllocator<OTHER, BASE_ALLOCATOR>& rhs);

            public:
                nonvirtual STLAllocator<T, BASE_ALLOCATOR> select_on_container_copy_construction () const;

            public:
                nonvirtual pointer address (reference v) const noexcept;
                nonvirtual const_pointer address (const_reference v) const noexcept;

            public:
                nonvirtual pointer allocate (size_type nElements);
                nonvirtual pointer allocate (size_type nElements, const void* ptr);
                nonvirtual void    deallocate (pointer ptr, size_type sz);

            public:
                nonvirtual void construct (pointer p);
                nonvirtual void construct (pointer p, const T& v);

            public:
                template <typename OTHERT>
                nonvirtual void destroy (OTHERT* p);

            public:
                template <typename... ARGS>
                nonvirtual void construct (pointer p, ARGS&&... args);

            public:
                nonvirtual size_t max_size () const noexcept;

            public:
                nonvirtual bool operator== (const STLAllocator<T, BASE_ALLOCATOR>& rhs) const;
                nonvirtual bool operator!= (const STLAllocator<T, BASE_ALLOCATOR>& rhs) const;
            };

            /**
             * The SimpleSizeCountingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which keeps statistics, and delegates to
             * some real allocator (constructor argument).
             */
            class SimpleSizeCountingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
            public:
                SimpleSizeCountingGeneralPurposeAllocator ();
                SimpleSizeCountingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
                ~SimpleSizeCountingGeneralPurposeAllocator ();

            public:
                virtual void* Allocate (size_t size) override;
                virtual void  Deallocate (void* p) override;

            public:
                nonvirtual size_t GetNetAllocationCount () const;
                nonvirtual size_t GetNetAllocatedByteCount () const;

            private:
                AbstractGeneralPurposeAllocator& fBaseAllocator_;
                atomic<uint32_t>                 fNetAllocationCount_;
                atomic<size_t>                   fNetAllocatedByteCount_;
            };

            /**
             * The LeakTrackingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which
             * keeps LOTS of statistics - it tracks all allocations, and delegates to some real allocator
             * (constructor argument).
             */
            class LeakTrackingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
            public:
                using PTRMAP = map<void*, size_t, less<void*>, STLAllocator<pair<void* const, size_t>>>;

            public:
                LeakTrackingGeneralPurposeAllocator ();
                LeakTrackingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
                ~LeakTrackingGeneralPurposeAllocator ();

            public:
                virtual void* Allocate (size_t size) override;
                virtual void  Deallocate (void* p) override;

            public:
                nonvirtual size_t GetNetAllocationCount () const;
                nonvirtual size_t GetNetAllocatedByteCount () const;

            public:
                class Snapshot {
                public:
                    Snapshot ();
                    explicit Snapshot (const PTRMAP& m);

                public:
                    PTRMAP fAllocations;
                };

            public:
                nonvirtual Snapshot GetSnapshot () const;
                nonvirtual void     DUMPCurMemStats (const Snapshot& sinceSnapshot = Snapshot ());

            private:
                mutable recursive_mutex          fCritSection_;
                AbstractGeneralPurposeAllocator& fBaseAllocator_;
                PTRMAP                           fAllocations_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryAllocator.inl"

#endif /*_Stroika_Foundation_Memory_MemoryAllocator_h_*/
