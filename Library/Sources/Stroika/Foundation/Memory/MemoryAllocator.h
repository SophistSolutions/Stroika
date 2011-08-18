/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_MemoryAllocator_h_
#define	_Stroika_Foundation_Memory_MemoryAllocator_h_	1

#include	"../StroikaPreComp.h"

#include	<map>
#include	<memory>

#include	"../Execution/CriticalSection.h"

#include	"Common.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


			/*
			 * This defines a generic abstract 'Allocator' API - for allocating (and freeing) memory.
			 */
			class	AbstractGeneralPurposeAllocator {
				protected:
					virtual ~AbstractGeneralPurposeAllocator ();
		
				// Allocate never returns NULL and Deallocate () should not be called with NULL
				// Allocate throws bad_alloc () on failure to allocate
				public:
					virtual	void*	Allocate (size_t size)		=	0;
					virtual	void	Deallocate (void* p)		=	0;
			};


			/*
			 * SimpleAllocator_CallLIBCMallocFree implements the AbstractGeneralPurposeAllocator just using malloc and free.
			 */
			class	SimpleAllocator_CallLIBCMallocFree : public AbstractGeneralPurposeAllocator {
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);
			};



			/*
			 * SimpleAllocator_CallLIBCNewDelete implements the AbstractGeneralPurposeAllocator just using stdC++ new/delete.
			 */
			class	SimpleAllocator_CallLIBCNewDelete : public AbstractGeneralPurposeAllocator {
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);
			};



			/*
			 * The STLAllocator takes a Stroika Allocator class (as template argument) and maps it for usage as an STL-style allocator.
			 */
			template <typename T, typename BASE_ALLOCATOR = SimpleAllocator_CallLIBCMallocFree>
				class	STLAllocator : public allocator<T> {
					public:
						typedef allocator<T>			_Mybase;
						typedef typename _Mybase::value_type	value_type;
					#if		qPlatform_Windows
						typedef value_type _FARQ*				pointer;
						typedef value_type _FARQ&				reference;
						typedef const value_type _FARQ*			const_pointer;
						typedef const value_type _FARQ&			const_reference;
					#else
						typedef value_type*						pointer;
						typedef value_type&						reference;
						typedef const value_type*				const_pointer;
						typedef const value_type&				const_reference;
					#endif
						typedef size_t							size_type;
						typedef ptrdiff_t						difference_type;

					public:
						typename BASE_ALLOCATOR	fBaseAllocator;

					public:
						template <typename _Other, typename BASE_ALLOCATOR = SimpleAllocator_CallLIBCMallocFree>
							struct rebind {
								typedef STLAllocator<_Other,BASE_ALLOCATOR> other;
							};

					public:
						nonvirtual	pointer			address (reference _Val) const;
						nonvirtual	const_pointer	address (const_reference _Val) const;

					public:
						STLAllocator ();
						STLAllocator (const STLAllocator<T,BASE_ALLOCATOR>&);

					public:
						template	<typename _Other, typename BASE_ALLOCATOR>
							STLAllocator(const STLAllocator<_Other, BASE_ALLOCATOR>& from):
								fBaseAllocator (from.fBaseAllocator)
								{
								}
						template	<typename _Other,typename BASE_ALLOCATOR>
							STLAllocator<T,BASE_ALLOCATOR>& operator= (const STLAllocator<_Other,BASE_ALLOCATOR>& rhs)
								{
									fBaseAllocator = rhs.from.fBaseAllocator;
									return (*this);
								}

					public:
						nonvirtual	pointer allocate (size_type _Count);
						nonvirtual	pointer allocate (size_type _Count, const void _FARQ *);
						nonvirtual	void	deallocate (pointer _Ptr, size_type);
					public:
						nonvirtual	void	construct (pointer _Ptr, const T& _Val);
						nonvirtual	void	destroy (pointer _Ptr);
					public:
						nonvirtual	_SIZT	max_size() const throw ();
				};




			/*
			 * The SimpleSizeCountingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which keeps statistics, and delegates to
			 * some real allocator (constructor argument).
			 */
			class	SimpleSizeCountingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
				public:
					SimpleSizeCountingGeneralPurposeAllocator ();
					SimpleSizeCountingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
					~SimpleSizeCountingGeneralPurposeAllocator ();
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);

				public:
					nonvirtual	size_t	GetNetAllocationCount () const;
					nonvirtual	size_t	GetNetAllocatedByteCount () const;

				private:
					AbstractGeneralPurposeAllocator&	fBaseAllocator;
					uint32_t							fNetAllocationCount;
					size_t								fNetAllocatedByteCount;
			};



			/*
			 * The LeakTrackingGeneralPurposeAllocator is a Stroika-style AbstractGeneralPurposeAllocator which keeps LOTS of statistics - it tracks all allocations,
			 * and delegates to some real allocator (constructor argument).
			 */
			class	LeakTrackingGeneralPurposeAllocator : public AbstractGeneralPurposeAllocator {
				public:
					typedef	map<void*,size_t,less<void*>,STLAllocator<pair<void*,size_t> > >	PTRMAP;

				public:
					LeakTrackingGeneralPurposeAllocator ();
					LeakTrackingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator);
					~LeakTrackingGeneralPurposeAllocator ();
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);

				public:
					nonvirtual	size_t	GetNetAllocationCount () const;
					nonvirtual	size_t	GetNetAllocatedByteCount () const;

				public:
					class	Snapshot {
						public:
							Snapshot ();
							explicit Snapshot (const PTRMAP& m);

						public:
							PTRMAP	fAllocations;
					};
				public:
					nonvirtual	Snapshot	GetSnapshot () const;
					nonvirtual	void		DUMPCurMemStats (const Snapshot& sinceSnapshot = Snapshot ());

				private:
					mutable	Execution::CriticalSection	fCritSection;
					AbstractGeneralPurposeAllocator&		fBaseAllocator;
					PTRMAP									fAllocations;
				private:
					friend	class	LeakTrackingGeneralPurposeAllocator;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Memory_MemoryAllocator_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"MemoryAllocator.inl"
