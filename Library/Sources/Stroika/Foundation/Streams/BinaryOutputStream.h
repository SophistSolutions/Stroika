/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BinaryOutputStream_h_
#define	_Stroika_Foundation_Streams_BinaryOutputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"


/*
 *	TODO:
 *		o	Consider/document approaches to timeouts. We COULD have a stream class where it was a PROPERTY OF THE CLASS (or alternate API)
 *			where writes timeout after a certain point.
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			/*
			 * Design Overview:
			 *
			 *		o	BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make an input/output stream. Simlarly, they can both be
			 *			mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
			 */
			class	BinaryOutputStream {
				protected:
					BinaryOutputStream ();
				public:
					virtual ~BinaryOutputStream ();
				
				public:
					// pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
					// Writes always succeed fully or throw.
					nonvirtual	void	Write (const Byte* start, const Byte* end);

				protected:
					// pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
					// Writes always succeed fully or throw.
					virtual	void	_Write (const Byte* start, const Byte* end)			=	0;
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
