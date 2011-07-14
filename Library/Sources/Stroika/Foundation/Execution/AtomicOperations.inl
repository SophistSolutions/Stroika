/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Event_inl_
#define	_Stroika_Foundation_Execution_Event_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if		qPlatform_Windows
#include	<windows.h>
#endif

#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			inline	int32_t	AtomicIncrement (volatile int32_t* p)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedIncrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}
			inline	uint32_t	AtomicIncrement (volatile uint32_t* p)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return static_cast<uint32_t> (::InterlockedIncrement (reinterpret_cast<volatile LONG*> (p)));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	int64_t	AtomicIncrement (volatile int64_t* p)
				{
					RequireNotNil (p);
					return ::InterlockedIncrement64 (p);
				}
			inline	uint64_t	AtomicIncrement (volatile uint64_t* p)
				{
					RequireNotNil (p);
					return ::InterlockedIncrement64 (reinterpret_cast<volatile int64_t*> (p));
				}
			#endif

			inline	int32_t	AtomicDecrement (volatile int32_t* p)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedDecrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	int64_t	AtomicDecrement (volatile int64_t* p)
				{
					RequireNotNil (p);
					return ::InterlockedDecrement64 (p);
				}
			inline	uint64_t	AtomicDecrement (volatile uint64_t* p)
				{
					RequireNotNil (p);
					return ::InterlockedDecrement64 (reinterpret_cast<volatile int64_t*> (p));
				}
			#endif
			inline	uint32_t	AtomicDecrement (volatile uint32_t* p)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedDecrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}

			inline	int32_t	AtomicAdd (volatile int32_t* p, int32_t arg)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	int64_t	AtomicAdd (volatile int64_t* p, int64_t arg)
				{
					RequireNotNil (p);
					return ::InterlockedExchangeAdd64 (p, arg);
				}
			#endif
			inline	uint32_t	AtomicAdd (volatile uint32_t* p, uint32_t arg)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	uint64_t	AtomicAdd (volatile uint64_t* p, uint64_t arg)
				{
					RequireNotNil (p);
					return ::InterlockedExchangeAdd64 (reinterpret_cast<volatile int64_t*> (p), arg);
				}
			#endif

			inline	int32_t	AtomicSubtract (volatile int32_t* p, int32_t arg)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), -arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	int64_t	AtomicSubtract (volatile int64_t* p, int64_t arg)
				{
					RequireNotNil (p);
					return ::InterlockedExchangeAdd64 (p, -arg);
				}
			#endif
			inline	uint32_t	AtomicSubtract (volatile uint32_t* p, uint32_t arg)
				{
					RequireNotNil (p);
					#if		qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), -static_cast<int32_t> (arg));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qPlatform_Win64
			inline	uint64_t	AtomicSubtract (volatile uint64_t* p, uint64_t arg)
				{
					RequireNotNil (p);
					return ::InterlockedExchangeAdd64 (reinterpret_cast<volatile int64_t*> (p), -static_cast<int64_t> (arg));
				}
			#endif

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Event_inl_*/
