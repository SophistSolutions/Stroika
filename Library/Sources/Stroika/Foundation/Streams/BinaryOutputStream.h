/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BinaryOutputStream_h_
#define	_Stroika_Foundation_Streams_BinaryOutputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			// Use when reading from a structured stream the data is ill-formed
			class	BinaryOutputStream {
				public:
					// pointer must refer to valid memory at least bufSize long, and cannot be NULL. BufSize must always be >= 1.
					// Writes always succeed fully or throw.
					virtual	void	Write (const Byte* buffer, size_t bufSize)			=	0;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_BinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BinaryOutputStream.inl"
