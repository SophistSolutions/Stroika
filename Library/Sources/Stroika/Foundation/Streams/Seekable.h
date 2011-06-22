/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
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

// CLARIFY about meaning of offset for CHARCTER streams. Offset in BYTES not CHARACTERS.

// WHAT does that mean for
// class F : public Seekable, BinaryInputStream, TextInputStream {}??
// MAYBE we just never mix together binaryinputsteream and textinputstream? UNCLEAR. Unclear what it would mean to combine them...
// MAYBE never mix them together, but create ADAPTRERS (witgh smart pointers) so they can reference each other.
// something that makes a textstream LOOK like a BINARY stream - via indriection, and a character set for decoding!! YES
// I THJINK THATS IT!!

			class	Seekable {
				public:
					SeekOffsetType	GetOffset () const;
					void			Seek (SeekOffsetType offset) const;
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
