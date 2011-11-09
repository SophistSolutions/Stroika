/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_
#define	_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Execution/CriticalSection.h"

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
			 *	ExternallyOwnedMemoryBinaryInputStream is a subtype of BinaryInputStream but the creator must gaurantee, so long as the memory pointed to
			 * in the argument has a
			 *		o	lifetime > lifetime of the ExternallyOwnedMemoryBinaryInputStream object,
			 *		o	and data never changes value
			 *
			 * This class is threadsafe - meaning _Read() can safely be called from multiple threads at a time freely.
			 */
			class	ExternallyOwnedMemoryBinaryInputStream : public BinaryInputStream {
				public:
					NO_DEFAULT_CONSTRUCTOR(ExternallyOwnedMemoryBinaryInputStream);
					NO_COPY_CONSTRUCTOR(ExternallyOwnedMemoryBinaryInputStream);
					NO_ASSIGNMENT_OPERATOR(ExternallyOwnedMemoryBinaryInputStream);

				public:
					ExternallyOwnedMemoryBinaryInputStream (const Byte* start, const Byte* end);
				
				protected:
					virtual	size_t	_Read (Byte* intoStart, Byte* intoEnd) override;

				private:
					Execution::CriticalSection	fCriticalSection_;
					const Byte*					fStart_;
					const Byte*					fEnd_;
					const Byte*					fCursor_;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
