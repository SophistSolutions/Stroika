/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BinaryInputStream_h_
#define	_Stroika_Foundation_Streams_BinaryInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22


//// TODO - REVIEW .NET API for STREAMS - SEE IF WE MISSED ANYTHIGN IMPORTANT, or how our hierarchy matches up with theirs



namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

// THINK OUT - ON FAILURE TO READ CUZ NO DATA AVIALBLE NOW, VERSUS EOF ETC - DO WE THROW - OR RETURN ZERO?????
// MUST THINK OUT CAREFULLY
			class	BinaryInputStream {
				public:
					// pointer must refer to valid memory at least bufSize long, and cannot be NULL. BufSize must always be >= 1
					virtual	size_t	Read (Byte* buffer, size_t bufSize)			=	0;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_BinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BinaryInputStream.inl"
