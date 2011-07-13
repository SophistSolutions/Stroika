/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_Seekable_h_
#define	_Stroika_Foundation_Streams_Seekable_h_	1

#include	"../StroikaPreComp.h"

#include	<cstdint>

#include	"../Configuration/Common.h"

///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22


namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {


			typedef	uint64_t	SeekOffsetType;

			/*
			 * Design Overview:
			 *
			 *		o	Designed to be mixed into BinaryInputStream, BinaryOutputStream, TextInputStream, or TextOutputStream.
			 *
			 *		o	If called on an input stream that has already returned EOF, may cause subsequence Read() calls to NOT be at EOF.
			 *
			 *		o	Offsets are in whatever unit makes sense for the kind of stream this is mixed into, so for TextInputStreams, offsets are in CHARACTERS
			 *			and in Binary streams, offsets are in Bytes. This implies one CANNOT reasonably mix together Binary streams and Text streams (one combines them
			 *			by use of a special TextStream that refers to another BinaryStream for actual IO).
			 *
			 *		o	Considered doing something like .Net CanSeek (), CanRead(), etc, but I decided it was simpler and more elegant to just keep things separate and
			 *			use mixin classes like 'interfaces' - and use dynamic_cast<> to see what functionality is available on a stream. Also - COULD have allowed
			 *			for GetOffset() to work separately from Seek () - but decided this was a simpler API, and I could think of no cases where it was particularly useful to
			 *			track a 'seek offset' but not be able to Seek() - and if it is useful - its easy enuf to do another way (e.g. wrap the stream you are using, and track
			 *			read calls to it and increment your own offset).
			 */
			class	Seekable {
				public:
					virtual	SeekOffsetType	GetOffset () const						=	0;
					virtual	void			Seek (SeekOffsetType offset)			=	0;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_Seekable_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Seekable.inl"
