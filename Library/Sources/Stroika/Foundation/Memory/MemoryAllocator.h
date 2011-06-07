/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__MemoryAllocator_h__
#define	__MemoryAllocator_h__	1

#include	"../StroikaPreComp.h"

#include	<map>
#include	<memory>

#include	"../Execution/CriticalSection.h"

#include	"Basics.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


			class	AbstractGeneralPurposeAllocator {
				protected:
					virtual ~AbstractGeneralPurposeAllocator ();
		
				// Allocate never returns NULL and Deallocate () should not be called with NULL
				// Allocate throws bad_alloc () on failure to allocate
				public:
					virtual	void*	Allocate (size_t size)		=	0;
					virtual	void	Deallocate (void* p)		=	0;
			};


			class	SimpleAllocator_CallLIBCMallocFree : public AbstractGeneralPurposeAllocator {
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);
			};



			class	SimpleAllocator_CallLIBCNewDelete : public AbstractGeneralPurposeAllocator {
				public:
					override	void*	Allocate (size_t size);
					override	void	Deallocate (void* p);
			};




			// For use integrating our allocators with STL
			template <typename T, typename BASE_ALLOCATOR = SimpleAllocator_CallLIBCMallocFree>
				class	STLAllocator : public _Allocator_base<T> {
					public:
						typedef _Allocator_base<T>			_Mybase;
						typedef typename _Mybase::value_type	value_type;
						typedef value_type _FARQ*				pointer;
						typedef value_type _FARQ&				reference;
						typedef const value_type _FARQ*			const_pointer;
						typedef const value_type _FARQ&			const_reference;
						typedef _SIZT							size_type;
						typedef _PDFT							difference_type;

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
					LONG								fNetAllocationCount;
					LONG								fNetAllocatedByteCount;
			};




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
#endif	/*__MemoryAllocator_h__*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"MemoryAllocator.inl"
