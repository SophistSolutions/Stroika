/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_AtomicOperations_h_
#define	_Stroika_Foundation_Execution_AtomicOperations_h_	1

#include	"../StroikaPreComp.h"

#include	<cstdint>

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			int32_t		AtomicIncrement (volatile int32_t* p);
			int64_t		AtomicIncrement (volatile int64_t* p);
			uint32_t	AtomicIncrement (volatile uint32_t* p);
			uint64_t	AtomicIncrement (volatile uint64_t* p);
			
			int32_t		AtomicDecrement (volatile int32_t* p);
			int64_t		AtomicDecrement (volatile int64_t* p);
			uint32_t	AtomicDecrement (volatile uint32_t* p);
			uint64_t	AtomicDecrement (volatile uint64_t* p);

			int32_t		AtomicAdd (volatile int32_t* p, int32_t arg);
			int64_t		AtomicAdd (volatile int64_t* p, int64_t arg);
			uint32_t	AtomicAdd (volatile uint32_t* p, uint32_t arg);
			uint64_t	AtomicAdd (volatile uint64_t* p, uint64_t arg);
			
			int32_t		AtomicSubtract (volatile int32_t* p, int32_t arg);
			int64_t		AtomicSubtract (volatile int64_t* p, int64_t arg);
			uint32_t	AtomicSubtract (volatile uint32_t* p, uint32_t arg);
			uint64_t	AtomicSubtract (volatile uint64_t* p, uint64_t arg);

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_AtomicOperations_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"AtomicOperations.inl"
