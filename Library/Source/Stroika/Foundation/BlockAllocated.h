/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__BlockAllocated_h__
#define	__BlockAllocated_h__	1

#include	"StroikaPreComp.h"

#include	<algorithm>

#include	"ModuleInit.h"
#include	"Support.h"


namespace	Stroika {	
	namespace	Foundation {


/*
@CONFIGVAR:		qAllowBlockAllocation
@DESCRIPTION:	<p>Allow use of block-allocation. The main reason to
	disable it indescriminantly is for debugging purposes (looking for
	memory leaks). But others may have other reasons.</p>
		<p>Defaults to true.</p>
 */
//#define	qAllowBlockAllocation						0
//#define	qAllowBlockAllocation						1
#ifndef	qAllowBlockAllocation
	#define	qAllowBlockAllocation						1
#endif




/*
@CLASS:			BlockAllocated<T>
@DESCRIPTION:	<p><code>BlockAllocated&ltT&gt</code> is a templated class designed to allow easy use
	of a block-allocated memory strategy. This means zero overhead malloc/memory allocation for fixed
	size blocks (with the only problem being the storage is never - or almost never - returned to the
	free store - it doesn't leak - but cannot be used for other things). This is often a useful
	tradeoff for things you allocate a great number of.</p>
		<p>You shouldn't disable it lightly. But you may wish to temporarily disable block-allocation
	while checking for memory leaks by shutting of the @'qAllowBlockAllocation' compile-time configuration variable.</p>
		<p>Note also - you can avoid some of the uglines of the overload declarations by using the 
	@'DECLARE_USE_BLOCK_ALLOCATION' macro.</p>
*/
template	<typename	T>	class	BlockAllocated {
	public:
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

#if		qAllowBlockAllocation
	private:
		static	void	GetMem_ ();	// Break out into separate function so we can
									// make op new inline for MOST important case
									// were alloc is cheap linked list operation...

		static	void*	sNextLink;
		static	void*	GetNextLink_ ();
		static	void	SetNextLink_ (void* nextLink);
#endif
};


/*
@CLASS:			DECLARE_USE_BLOCK_ALLOCATION
@DESCRIPTION:	<p>This helper macro can be used to avoid some of the C++ gorp required in declaring that you are
	using block-allocation with a given class.</p>
		<p>An example of use might be:
	<code>
	<pre>
		class	Foo {
			public:
				DECLARE_USE_BLOCK_ALLOCATION(Foo);
		}
	</pre>
	</code>
		</p>
		<p>See also - @'DECLARE_DONT_USE_BLOCK_ALLOCATION' and @'BlockAllocated<T>'.</p>
*/
#if		qAllowBlockAllocation
	#define	DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
			static	void*	operator new (size_t n)							{	return (BlockAllocated<THIS_CLASS>::operator new (n));	}\
			static	void*	operator new (size_t n,int,const char*,int)		{	return (BlockAllocated<THIS_CLASS>::operator new (n));	}\
			static	void	operator delete (void* p)						{	BlockAllocated<THIS_CLASS>::operator delete (p);		}\
			static	void	operator delete (void* p,int,const char*,int)	{	BlockAllocated<THIS_CLASS>::operator delete (p);		}
#else
	#define	DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif

/*
@CLASS:			DECLARE_DONT_USE_BLOCK_ALLOCATION
@DESCRIPTION:	<p>If you subclass from a class which uses block-allication, you can use this to turn off block allocation
	in your particular subclass.</p>
		<p>See also @'DECLARE_USE_BLOCK_ALLOCATION' and @'BlockAllocated<T>'.</p>
*/
#if		qAllowBlockAllocation
	#define	DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)\
			static	void*	operator new (size_t n)		{	return ::operator new (n);	}\
			static	void	operator delete (void* p)	{	::operator delete (p);		}
#else
	#define	DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif














	}
}






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#include	"Assertions.h"
	#include	"Memory.h"
	#include	"ThreadUtils.h"

namespace	Stroika {	
	namespace	Foundation {

	namespace	ThreadUtils {
		class	CriticalSection;
	}
	namespace	BlockAllocation {
		namespace	Private {
			using	ThreadUtils::CriticalSection;
			struct	ActualModuleInit {
				ActualModuleInit ();
				~ActualModuleInit ();
			};

			extern	CriticalSection*	sCritSection_;

			CriticalSection&	GetCritSection_ ();
		}
	};
	namespace	{
		ModuleInit::ModuleInitializer<BlockAllocation::Private::ActualModuleInit>	_MI_;	// this object constructed for the CTOR/DTOR per-module side-effects
	}


	inline	ThreadUtils::CriticalSection&	BlockAllocation::Private::GetCritSection_ ()
		{
			AssertNotNil (sCritSection_);	// automatically initailized by BlockAllocated::Private::ActualModuleInit
			return *sCritSection_;
		}


#if		qAllowBlockAllocation
	namespace	BlockAllocation {
		namespace	Private {
			// This must be included here to keep genclass happy, since the .cc file will not be included
			// in the genclassed .cc file....
			inline	void**	GetMem_Util_ (const size_t sz)
				{
					Assert (sz >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link

					/*
					 * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
					 * a chunk which memory allocator can do a good job on.
					 */
					const	size_t	kTargetMallocSize	=	16360;					// 16384 = 16K - leave a few bytes sluff...
					const	size_t	kChunks = max (static_cast<size_t> (kTargetMallocSize / sz), static_cast<size_t> (10));

					/*
					 * Please note that the following line is NOT a memory leak. Please look at the
					 * Led FAQ question#29 - "Does Led have any memory leaks?
					 * How does qAllowBlockAllocation affect memory leaks?"
					 */
					void**	newLinks	=	(void**)DEBUG_NEW char [kChunks*sz];
					void**	curLink		=	newLinks;
					for (size_t i = 1; i < kChunks; i++) {
						*curLink = &(((char*)newLinks)[i*sz]);
						curLink = (void**)*curLink;
					}
					*curLink = NULL;		// NULL-terminate the link list
					return (newLinks);
				}



			/*
			 *		In order to reduce fragmentation, we allocate all chunks of common sizes out of
			 *	the same pool. Assuming the compiler is able to inline the references to these
			 *	below, there is really no cost in adding more. I picked the ones I thought most
			 *	likely to come up, but adding more should be just fine - strictly a win.
			 *
			 *		Don't bother implementing Block_Alloced_sizeof_N_GetMems() cuz flunging will
			 *	genericly give us the same code-sharing effect.
			 */
				extern	void*	sSizeof_4_NextLink;
				extern	void*	sSizeof_8_NextLink;
				extern	void*	sSizeof_12_NextLink;
				extern	void*	sSizeof_16_NextLink;
				extern	void*	sSizeof_20_NextLink;
				extern	void*	sSizeof_24_NextLink;
				extern	void*	sSizeof_28_NextLink;
				extern	void*	sSizeof_32_NextLink;
				extern	void*	sSizeof_36_NextLink;
				extern	void*	sSizeof_40_NextLink;
				extern	void*	sSizeof_44_NextLink;
				extern	void*	sSizeof_48_NextLink;
				extern	void*	sSizeof_52_NextLink;
				extern	void*	sSizeof_56_NextLink;
				extern	void*	sSizeof_60_NextLink;
				extern	void*	sSizeof_64_NextLink;
				extern	void*	sSizeof_68_NextLink;
				extern	void*	sSizeof_72_NextLink;
				extern	void*	sSizeof_76_NextLink;
				extern	void*	sSizeof_80_NextLink;
		}
	}


	template	<typename	T>	inline	void*	BlockAllocated<T>::GetNextLink_ ()
		{
			using	namespace	BlockAllocation::Private;

			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
				#pragma warn -8066
			#endif
			/*
			 * It is hoped that since all these comparisons can be evaluated at compile
			 * time, they will be. Then this code reduces to just a return of a single
			 * global variable. That should inline nicely.
			 *
			 *	NB: For Apples compiler (by far our worst at optimizing), this does successfully
			 *		get translated into just one variable reference (C 3.2.3, CFront 3.2, ETO #8, Thursday, November 5, 1992 1:51:42 PM)
			 */
			if (sizeof (T) <= 4)			{	return (sSizeof_4_NextLink);		}
			else if (sizeof (T) <= 8)		{	return (sSizeof_8_NextLink);		}
			else if (sizeof (T) <= 12)		{	return (sSizeof_12_NextLink);		}
			else if (sizeof (T) <= 16)		{	return (sSizeof_16_NextLink);		}
			else if (sizeof (T) <= 20)		{	return (sSizeof_20_NextLink);		}
			else if (sizeof (T) <= 24)		{	return (sSizeof_24_NextLink);		}
			else if (sizeof (T) <= 28)		{	return (sSizeof_28_NextLink);		}
			else if (sizeof (T) <= 32)		{	return (sSizeof_32_NextLink);		}
			else if (sizeof (T) <= 36)		{	return (sSizeof_36_NextLink);		}
			else if (sizeof (T) <= 40)		{	return (sSizeof_40_NextLink);		}
			else if (sizeof (T) <= 44)		{	return (sSizeof_44_NextLink);		}
			else if (sizeof (T) <= 48)		{	return (sSizeof_48_NextLink);		}
			else if (sizeof (T) <= 52)		{	return (sSizeof_52_NextLink);		}
			else if (sizeof (T) <= 56)		{	return (sSizeof_56_NextLink);		}
			else if (sizeof (T) <= 60)		{	return (sSizeof_60_NextLink);		}
			else if (sizeof (T) <= 64)		{	return (sSizeof_64_NextLink);		}
			else if (sizeof (T) <= 68)		{	return (sSizeof_68_NextLink);		}
			else if (sizeof (T) <= 72)		{	return (sSizeof_72_NextLink);		}
			else if (sizeof (T) <= 76)		{	return (sSizeof_76_NextLink);		}
			else if (sizeof (T) <= 80)		{	return (sSizeof_80_NextLink);		}
			else							{	return (sNextLink);					}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}
	template	<typename	T>	inline	void	BlockAllocated<T>::SetNextLink_ (void* nextLink)
		{
			using	namespace	BlockAllocation::Private;

			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
				#pragma warn -8066
			#endif
			/*
			 * It is hoped that since all these comparisons can be evaluated at compile
			 * time, they will be. Then this code reduces to just an assignement to a single
			 * global variable. That should inline nicely.
			 */
			if (sizeof (T) <= 4)			{	sSizeof_4_NextLink = nextLink;		}
			else if (sizeof (T) <= 8)		{	sSizeof_8_NextLink = nextLink;		}
			else if (sizeof (T) <= 12)		{	sSizeof_12_NextLink = nextLink;		}
			else if (sizeof (T) <= 16)		{	sSizeof_16_NextLink = nextLink;		}
			else if (sizeof (T) <= 20)		{	sSizeof_20_NextLink = nextLink;		}
			else if (sizeof (T) <= 24)		{	sSizeof_24_NextLink = nextLink;		}
			else if (sizeof (T) <= 28)		{	sSizeof_28_NextLink = nextLink;		}
			else if (sizeof (T) <= 32)		{	sSizeof_32_NextLink = nextLink;		}
			else if (sizeof (T) <= 36)		{	sSizeof_36_NextLink = nextLink;		}
			else if (sizeof (T) <= 40)		{	sSizeof_40_NextLink = nextLink;		}
			else if (sizeof (T) <= 44)		{	sSizeof_44_NextLink = nextLink;		}
			else if (sizeof (T) <= 48)		{	sSizeof_48_NextLink = nextLink;		}
			else if (sizeof (T) <= 52)		{	sSizeof_52_NextLink = nextLink;		}
			else if (sizeof (T) <= 56)		{	sSizeof_56_NextLink = nextLink;		}
			else if (sizeof (T) <= 60)		{	sSizeof_60_NextLink = nextLink;		}
			else if (sizeof (T) <= 64)		{	sSizeof_64_NextLink = nextLink;		}
			else if (sizeof (T) <= 68)		{	sSizeof_68_NextLink = nextLink;		}
			else if (sizeof (T) <= 72)		{	sSizeof_72_NextLink = nextLink;		}
			else if (sizeof (T) <= 76)		{	sSizeof_76_NextLink = nextLink;		}
			else if (sizeof (T) <= 80)		{	sSizeof_80_NextLink = nextLink;		}
			else							{	sNextLink = nextLink;				}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}
#endif
	template	<typename	T>	inline	void*	BlockAllocated<T>::operator new (size_t n)
		{
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma push
			#pragma warn -8008
			#pragma warn -8066
		#endif
			Assert (sizeof (T) >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link
			Require (n == sizeof (T));
			Arg_Unused (n);							// n only used for debuggging, avoid compiler warning
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma pop
		#endif

			#if		qAllowBlockAllocation
				ThreadUtils::AutoCriticalSection	critSec (BlockAllocation::Private::GetCritSection_ ());
				/*
				 * To implement linked list of BlockAllocated(T)'s before they are
				 * actually alloced, re-use the begining of this as a link pointer.
				 */
				if (GetNextLink_ () == NULL) {
					GetMem_ ();
				}
				void*	result = GetNextLink_ ();
				SetNextLink_ (*(void**)GetNextLink_ ());

				return (result);
			#else
				return ::operator new (n);
			#endif
		}
	template	<typename	T>	inline	void	BlockAllocated<T>::operator delete (void* p)
		{
			#if		qAllowBlockAllocation
				if (p != NULL) {
					ThreadUtils::AutoCriticalSection	critSec (BlockAllocation::Private::GetCritSection_ ());
					(*(void**)p) = GetNextLink_ ();
					SetNextLink_ (p);
				}
			#else
				::operator delete (p);
			#endif
		}
#if		qAllowBlockAllocation
	template	<typename	T>	void	BlockAllocated<T>::GetMem_ ()
		{
			using	namespace	BlockAllocation::Private;
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
                #pragma warn -8066
			#endif
		#if		defined (_DEBUG)
			/*
			 * Temporary hack to verify were getting the most out of our blockallocator.
			 * If this is ever gets called, add extra sizes. NO COST, just wins!!!
			 */
			if (sizeof (T) > 80) {
		//		DebugMessage ("BlockAllocator<T> has a size > 80: %d", sizeof (T));
			}
		#endif

			if (sizeof (T) <= 4)		{	sSizeof_4_NextLink = GetMem_Util_ (4);		}
			else if (sizeof (T) <= 8)	{	sSizeof_8_NextLink = GetMem_Util_ (8);		}
			else if (sizeof (T) <= 12)	{	sSizeof_12_NextLink = GetMem_Util_ (12);	}
			else if (sizeof (T) <= 16)	{	sSizeof_16_NextLink = GetMem_Util_ (16);	}
			else if (sizeof (T) <= 20)	{	sSizeof_20_NextLink = GetMem_Util_ (20);	}
			else if (sizeof (T) <= 24)	{	sSizeof_24_NextLink = GetMem_Util_ (24);	}
			else if (sizeof (T) <= 28)	{	sSizeof_28_NextLink = GetMem_Util_ (28);	}
			else if (sizeof (T) <= 32)	{	sSizeof_32_NextLink = GetMem_Util_ (32);	}
			else if (sizeof (T) <= 36)	{	sSizeof_36_NextLink = GetMem_Util_ (36);	}
			else if (sizeof (T) <= 40)	{	sSizeof_40_NextLink = GetMem_Util_ (40);	}
			else if (sizeof (T) <= 44)	{	sSizeof_44_NextLink = GetMem_Util_ (44);	}
			else if (sizeof (T) <= 48)	{	sSizeof_48_NextLink = GetMem_Util_ (48);	}
			else if (sizeof (T) <= 52)	{	sSizeof_52_NextLink = GetMem_Util_ (52);	}
			else if (sizeof (T) <= 56)	{	sSizeof_56_NextLink = GetMem_Util_ (56);	}
			else if (sizeof (T) <= 60)	{	sSizeof_60_NextLink = GetMem_Util_ (60);	}
			else if (sizeof (T) <= 64)	{	sSizeof_64_NextLink = GetMem_Util_ (64);	}
			else if (sizeof (T) <= 68)	{	sSizeof_68_NextLink = GetMem_Util_ (68);	}
			else if (sizeof (T) <= 72)	{	sSizeof_72_NextLink = GetMem_Util_ (72);	}
			else if (sizeof (T) <= 76)	{	sSizeof_76_NextLink = GetMem_Util_ (76);	}
			else if (sizeof (T) <= 80)	{	sSizeof_80_NextLink = GetMem_Util_ (80);	}
			else {
				/*
				 * NON-Shared (across types of same size) freepool. This should rarely, if ever happen, but we fully support
				 * it anyhow.
				 */
				SetNextLink_ (GetMem_Util_ (sizeof (T)));
			}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}

	template	<typename	T>	void*	BlockAllocated<T>::sNextLink = NULL;
#endif

	}
}



#endif	/*__BlockAllocated_h__*/
