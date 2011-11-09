/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_MemoryBinaryInputStream_h_
#define	_Stroika_Foundation_Streams_MemoryBinaryInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Execution/CriticalSection.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"BinaryInputStream.h"


/*
 * TODO:
 *			o	Re-implemnt using atomics to avoid critical section (cheaper).
 *
 *			o	Consider mixing in Seekable here. Would be easy
 */




namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			/*
			 *
			 * This class is threadsafe - meaning _Read() can safely be called from multiple threads at a time freely.
			 */
			class	MemoryBinaryInputStream : public BinaryInputStream {
				public:
					NO_DEFAULT_CONSTRUCTOR(MemoryBinaryInputStream);
					NO_COPY_CONSTRUCTOR(MemoryBinaryInputStream);
					NO_ASSIGNMENT_OPERATOR(MemoryBinaryInputStream);

				public:
					MemoryBinaryInputStream (const Byte* start, const Byte* end);
				
				protected:
					virtual	size_t	_Read (Byte* intoStart, Byte* intoEnd) override;

				private:
					Execution::CriticalSection			fCriticalSection_;
					// round size of usage up to around 1k (include vtableptr) - no real good reason - # doesnt matter much...
					Memory::SmallStackBuffer<Byte,(1024-sizeof(Execution::CriticalSection)-2*sizeof(Byte*))>	fData_;
					const Byte*							fCursor_;
			};

		}
	}
}

#endif	/*_Stroika_Foundation_Streams_MemoryBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
