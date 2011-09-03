/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_AtomicOperations_inl_
#define	_Stroika_Foundation_Execution_AtomicOperations_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if		qCompilerAndStdLib_Supports_stdatomic
	#include	<atomic>
#elif	qPlatform_Windows
	#include	<windows.h>
#endif

#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			inline	int32_t	AtomicIncrement (volatile int32_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int32_t*> ((int32_t*)p)++;
					#elif	qPlatform_Windows
						return ::InterlockedIncrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}
			inline	uint32_t	AtomicIncrement (volatile uint32_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint32_t*> ((uint32_t*)p)++;
					#elif	qPlatform_Windows
						return static_cast<uint32_t> (::InterlockedIncrement (reinterpret_cast<volatile LONG*> (p)));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	int64_t	AtomicIncrement (volatile int64_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int64_t*> ((int64_t*)p)++;
					#elif	qPlatform_Win64
						return ::InterlockedIncrement64 (p);
					#endif
				}
			inline	uint64_t	AtomicIncrement (volatile uint64_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint64_t*> ((uint64_t*)p)++;
					#elif	qPlatform_Win64
						return ::InterlockedIncrement64 (reinterpret_cast<volatile int64_t*> (p));
					#endif
				}
			#endif

			inline	int32_t	AtomicDecrement (volatile int32_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int32_t*> ((int32_t*)p)++;
					#elif	qPlatform_Windows
						return ::InterlockedDecrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	int64_t	AtomicDecrement (volatile int64_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int64_t*> ((int64_t*)p)++;
					#elif	qPlatform_Windows
						return ::InterlockedDecrement64 (p);
					#endif
				}
			inline	uint64_t	AtomicDecrement (volatile uint64_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint64_t*> ((uint64_t*)p)++;
					#elif	qPlatform_Windows
						return ::InterlockedDecrement64 (reinterpret_cast<volatile int64_t*> (p));
					#endif
				}
			#endif
			inline	uint32_t	AtomicDecrement (volatile uint32_t* p)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint32_t*> ((uint32_t*)p)++;
					#elif	qPlatform_Windows
						return ::InterlockedDecrement (reinterpret_cast<volatile LONG*> (p));
					#else
						AssertNotImplemented ();
					#endif
				}

			inline	int32_t	AtomicAdd (volatile int32_t* p, int32_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int32_t*> ((int32_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	int64_t	AtomicAdd (volatile int64_t* p, int64_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int64_t*> ((int64_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd64 (p, arg);
					#endif
				}
			#endif
			inline	uint32_t	AtomicAdd (volatile uint32_t* p, uint32_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint32_t*> ((uint32_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	uint64_t	AtomicAdd (volatile uint64_t* p, uint64_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint64_t*> ((uint64_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd64 (reinterpret_cast<volatile int64_t*> (p), arg);
					#endif
				}
			#endif

			inline	int32_t	AtomicSubtract (volatile int32_t* p, int32_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int32_t*> ((int32_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), -arg);
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	int64_t	AtomicSubtract (volatile int64_t* p, int64_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<int64_t*> ((int64_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd64 (p, -arg);
					#endif
				}
			#endif
			inline	uint32_t	AtomicSubtract (volatile uint32_t* p, uint32_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint32_t*> ((uint32_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd (reinterpret_cast<volatile LONG*> (p), -static_cast<int32_t> (arg));
					#else
						AssertNotImplemented ();
					#endif
				}
			#if		qCompilerAndStdLib_Supports_stdatomic || qPlatform_Win64
			inline	uint64_t	AtomicSubtract (volatile uint64_t* p, uint64_t arg)
				{
					RequireNotNull (p);
					#if		qCompilerAndStdLib_Supports_stdatomic
// A BIT of a WAG about how to use stdc++ atomics... -- LGP 2011-09-02
//Pretty sure wrong but not sure what right way is...
						atomic<uint64_t*> ((uint64_t*)p) += arg;
					#elif	qPlatform_Windows
						return ::InterlockedExchangeAdd64 (reinterpret_cast<volatile int64_t*> (p), -static_cast<int64_t> (arg));
					#endif
				}
			#endif

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_AtomicOperations_inl_*/
