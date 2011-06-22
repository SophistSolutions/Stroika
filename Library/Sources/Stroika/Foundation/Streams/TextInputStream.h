/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_TextInputStream_h_
#define	_Stroika_Foundation_Streams_TextInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"


///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {



// THINK OUT - ON FAILURE TO READ CUZ NO DATA AVIALBLE NOW, VERSUS EOF ETC - DO WE THROW - OR RETURN ZERO?????
// MUST THINK OUT CAREFULLY

			// Use when reading from a structured stream the data is ill-formed
			class	TextInputStream {
				public:
					// pointer must refer to valid memory at least bufSize characters long, and cannot be NULL. bufSize must always be >= 1
					// returns the number of characters read (which is always > 0. If non data available, a
					virtual	size_t	Read (wchar_t* buffer, size_t bufSize)			=	0;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_TextInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"TextInputStream.inl"
