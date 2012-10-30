/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_MemoryAllocator_h_
#define _Stroika_Foundation_Memory_MemoryAllocator_h_   1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <memory>

#include    "../Execution/CriticalSection.h"

#include    "Common.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             * This defines a generic abstract 'Allocator' API - for allocating (and freeing) memory.
             * This is far simpler than than the STL/stdC++ allocator policy (for example, not templated, and uses C model
             * of memory - just a byte pointer and amount).
             *
             *  @todo   Consider if we should have a notion of alignment, or if any of the other stdc++ allocator stuff is
             *          needed  here...
             */
            class   AbstractGeneralPurposeAllocator {
            protected:
                virtual ~AbstractGeneralPurposeAllocator ();

                // Allocate never returns nullptr and Deallocate () should not be called with nullptr
                // Allocate throws bad_alloc () on failure to allocate
            public:
                virtual void*   Allocate (size_t size)      =   0;
                virtual void    Deallocate (void* p)        =   0;
            };


            /*
             * SimpleAllocator_CallLIBCMallocFree implements the AbstractGeneralPurposeAllocator just using malloc and free.
             */
            class   SimpleAllocator_CallLIBCMallocFree : public AbstractGeneralPurposeAllocator {
            public:
                virtual     void*   Allocate (size_t size) override;
                virtual     void    Deallocate (void* p) override;
            };



            /*
             * SimpleAllocator_CallLIBCNewDelete implements the AbstractGeneralPurposeAllocator just using stdC++ new/delete.
             */
            class   SimpleAllocator_CallLIBCNewDelete : public AbstractGeneralPurposeAllocator {
            public:
                virtual     void*   Allocate (size_t size) override;
                virtual     void    Deallocate (void* p) override;
            };



            /*
             * The STLAllocator takes a Stroika Allocator class (as template argument) and maps it
             * for usage as an STL-style allocator.
             *
             *  @todo   unsure about how to handle propagate_on_container_copy_assignment, etc. For all my current allocators, this
             *          is fine (false), but if they had data, obviously it would be differnt. Perhaps that choice should be
             *          parametized (as it is with normal C++ allocactors). Or maybe leave subclassing this STLAllocator<> as a way
             *          todo that? Thats probably good enuf...
             */
            template <typename T, typename BASE_ALLOCATOR = SimpleAllocator_CallLIBCMallocFree>
            class   STLAllocator  {
            public:
                typedef STLAllocator<T, BASE_ALLOCATOR> other;
                typedef T                               value_type;
                typedef value_type*                     pointer;
                typedef const value_type*               const_pointer;
                typedef void*                           void_pointer;
                typedef const void*                     const_void_pointer;
                typedef value_type&                     reference;
                typedef const value_type&               const_reference;
                typedef size_t                          size_type;
                typedef ptrdiff_t                       difference_type;
                typedef false_type                      propagate_on_container_copy_assignment;
                typedef false_type                      propagate_on_container_move_assignment;
                typedef false_type                      propagate_on_container_swap;

            public:
                template <typename OTHER>
                struct rebind {
                    typedef STLAllocator<OTHER, BASE_ALLOCATOR> other;
                };

            public:
                BASE_ALLOCATOR  fBaseAllocator;

            public:
                explicit STLAllocator ();
                STLAllocator (const STLAllocator<T, BASE_ALLOCATOR>& from);
                template    <typename OTHER>
                STLAllocator(const STLAllocator<OTHER, BASE_ALLOCATOR>& from);
                template    <typename OTHER>
                STLAllocator<T, BASE_ALLOCATOR>& operator= (const STLAllocator<OTHER, BASE_ALLOCATOR>& rhs);

            public:
                nonvirtual  STLAllocator<T, BASE_ALLOCATOR>  select_on_container_copy_construction() const;

            public:
                nonvirtual  pointer         address (reference v) const noexcept;
                nonvirtual  const_pointer   address (const_reference v) const noexcept;

            public:
                nonvirtual  pointer allocate (size_type nElements);
                nonvirtual  pointer allocate (size_type nElements, const void* ptr);
                nonvirtual  void    deallocate (pointer ptr, size_type sz);

            public:
                nonvirtual  void    construct (pointer p);
                nonvirtual  void    construct (pointer p, const T& v);

            public:
                template<class OTHERT>
                nonvirtual  void    destroy (OTHERT* p);

#if  qCompilerAndStdLib_Supports_varadic_templates
            public:
                template    <typename... ARGS>
                void construct (pointer p, ARGS && ... args);
#endif

            public:
                nonvirtual  size_t  max_size () const noexcept;

            public:
                nonvirtual  bool    operator== (const STLAllocator<T, BASE_ALLOCATOR>& rhs) const;
                nonvirtual  bool    operator!= (const STLAllocator<T, BASE_ALLOCATOR>& rhs) const;
            };




            /*
             * The SimpleSizeCountingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which keeps statistics, and delegates to
             * some real allocator (constructor argument).
             */
            class   SimpleSizeCountingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
            public:
                SimpleSizeCountingGeneralPurposeAllocator ();
                SimpleSizeCountingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
                ~SimpleSizeCountingGeneralPurposeAllocator ();
            public:
                virtual     void*   Allocate (size_t size) override;
                virtual     void    Deallocate (void* p) override;

            public:
                nonvirtual  size_t  GetNetAllocationCount () const;
                nonvirtual  size_t  GetNetAllocatedByteCount () const;

            private:
                AbstractGeneralPurposeAllocator&    fBaseAllocator;
                uint32_t                            fNetAllocationCount;
                size_t                              fNetAllocatedByteCount;
            };



            /*
             * The LeakTrackingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which keeps LOTS of statistics - it tracks all allocations,
             * and delegates to some real allocator (constructor argument).
             */
            class   LeakTrackingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
            public:
                typedef map<void*, size_t, less<void*>, STLAllocator<pair<const void*, size_t> > >    PTRMAP;

            public:
                LeakTrackingGeneralPurposeAllocator ();
                LeakTrackingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
                ~LeakTrackingGeneralPurposeAllocator ();
            public:
                virtual     void*   Allocate (size_t size) override;
                virtual     void    Deallocate (void* p) override;

            public:
                nonvirtual  size_t  GetNetAllocationCount () const;
                nonvirtual  size_t  GetNetAllocatedByteCount () const;

            public:
                class   Snapshot {
                public:
                    Snapshot ();
                    explicit Snapshot (const PTRMAP& m);

                public:
                    PTRMAP  fAllocations;
                };
            public:
                nonvirtual  Snapshot    GetSnapshot () const;
                nonvirtual  void        DUMPCurMemStats (const Snapshot& sinceSnapshot = Snapshot ());

            private:
                mutable Execution::CriticalSection  fCritSection;
                AbstractGeneralPurposeAllocator&    fBaseAllocator;
                PTRMAP                              fAllocations;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_MemoryAllocator_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MemoryAllocator.inl"
