/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_Seekable_h_
#define	_Stroika_Foundation_Streams_Seekable_h_	1

#include	"../StroikaPreComp.h"

#include	<cstdint>

#include	"../Configuration/Common.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {


			// When seeking, you can see an offset from the start (typical/start) or from the end of the stream, or from the current position)
			enum	Whence {
				FromStart_W,
				FromCurrent_W,
				FromEnd_W,
			};


			// Offset is signed, because you can see a negative # from the end or from the current position.
			typedef	int64_t	SeekOffsetType;


			/*
			 * Design Overview:
			 *
			 *		o	Designed to be mixed into BinaryInputStream, BinaryOutputStream, TextInputStream, or TextOutputStream. 
			 *
			 *		o	It should be mixed in as a virtual base, so an IO stream shares a COMMON notion of current offset. Note - this choice differs from iostreams, but mirrors
			 *			the choice in UNIX fileio ('read and write pointers the same).
			 *
			 *		o	If called on an input stream that has already returned EOF, may cause subsequence Read() calls to NOT be at EOF.
			 *
			 *		o	Offsets are in whatever unit makes sense for the kind of stream this is mixed into, so for TextInputStreams, offsets are in CHARACTERS
			 *			and in Binary streams, offsets are in Bytes. This implies one CANNOT reasonably mix together Binary streams and Text streams (one combines them
			 *			by use of a special TextStream that refers to another BinaryStream for actual IO).
			 *
			 *		o	Considered making this a MIXIN class - and NOT having BinaryInputStream(ETC) directly mix it in. That turned out to be inconvenient because ALL our
			 *			concrete implementation classes needed to support seeking (so they could be used when soemthign had seeking or not). The CanSeek() approach used
			 *			in .Net just seems more pragmatically simple.
			 *
			 *		o	Considered doing something like .Net CanSeek (), CanRead(), etc, but I decided it was simpler and more elegant to just keep things separate and
			 *			use mixin classes like 'interfaces' - and use dynamic_cast<> to see what functionality is available on a stream.
			 *
			 *		o	COULD have allowed for GetOffset() to work separately from Seek () - but decided this was a simpler API, and I could think of no
			 *			cases where it was particularly useful to track a 'seek offset' but not be able to Seek() - and if it is useful - its easy
			 *			enuf to do another way (e.g. wrap the stream you are using, and track read calls to it and increment your own offset).
			 */
			class	Seekable {
				public:
					nonvirtual	bool	CanSeek () const;
					nonvirtual	bool	CanSeek (Whence whence) const;
				protected:
					virtual	bool	_CanSeek (Whence whence) const						=	0;

				public:
					nonvirtual	SeekOffsetType	GetOffset () const;
				protected:
					virtual	SeekOffsetType	_GetOffset () const						=	0;

				public:
					/*
					 * The new position, measured in bytes, is obtained by adding offset bytes to the position specified by whence
					 */
					nonvirtual	void		Seek (SeekOffsetType offset);
					nonvirtual	void		Seek (Whence whence, SeekOffsetType offset);
				protected:
					virtual	void			_Seek (Whence whence, SeekOffsetType offset)			=	0;
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
