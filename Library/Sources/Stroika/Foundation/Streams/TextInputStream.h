/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_TextInputStream_h_
#define	_Stroika_Foundation_Streams_TextInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"


///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {



			/*
			 * Design Overview:
			 *
			 *		o	All read's on a TextInputStream are BLOCKING. If there is a desire to have a non-blocking read, then create a new mixin interface
			 *			and through that interface you can do non-blocking reads, but this Read() method must always block.
			 *
			 *		o	EOF is handled by a return value of zero. Once EOF is returned - any subsequent calls to Read () will return EOF (unless some other
			 *			mechanism is used to tweak the state of the object, like a mixed in Seekable class and calling SEEK).
			 *
			 *		o	Exceptions indicate something went wrong - like an IO error, or  formatting effort (e.g. if the source is encrypted,
			 *			and the stream is decrypting, then it might detect a format error and throw).
			 *
			 *		o	TextInputStream and TextOutputStream CAN be naturally mixed togehter to make an input/output stream. Simlarly, they can both be
			 *			mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
			 *
			 *BECAUSE OF DIFFICULTIES DOING SOME STUFF WE WANT - LIKE READLINE - and probably also characterset stuff - we MAY want to have TEXTINPUTSTREAM REQUIRE seekability>
			 *PROBABLY NO. BUT A THOUGHT... --LGP 2011-06-22
			 */
			class	TextInputStream {
				protected:
					TextInputStream ();
				
				public:
					// Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of characters read
					// BLOCKING until data is available, but can return with fewer bytes than bufSize without prjudice about how much more is available.
					nonvirtual	size_t	Read (wchar_t* buffer, size_t bufSize);
					
					// Blocking read of a single character. Returns a NUL-character on EOF ('\0')
					nonvirtual	wchar_t	Read ();

					// WANTED todo this - but cannot DO SO - without PEEK/SEEKABILITY!!!! (after you read CR, you must look ahead for LF, but cannot)
					// We COULD define this API so it somehow worked out (set  aflag saying last read CR so if next Read of char is LF, then successive readlines work, but a bit kludgy)
					// return result includes trailing CR and or LF, if any
					//nonvirtual	wstring	ReadLine ();

				protected:
					// Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of characters read
					// BLOCKING until data is available, but can return with fewer bytes than bufSize without prjudice about how much more is available.
					virtual	size_t	_Read (wchar_t* buffer, size_t bufSize)			=	0;
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
