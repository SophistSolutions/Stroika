/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BinaryInputStream_h_
#define	_Stroika_Foundation_Streams_BinaryInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

#include	"Seekable.h"

///VERY PRELININARY PROTOTYPE DESIGN - JUST WRITTEN DOWN TO REVIEWW - NOT USED ANYWHERE YET
///		-- LGP 2011-06-22


//// TODO - REVIEW .NET API for STREAMS - SEE IF WE MISSED ANYTHIGN IMPORTANT, or how our hierarchy matches up with theirs

/* **************** STREAMS MODULE TODO ******************************

	>NEW BUffered classes
		>	Need BUffered variants for Binary/Text and Input/Output. Do them separately - thats 4 classes.
		>	Must figure out of I want 2 more - BinaryBufferedInputOutputStream and TextBufferedInputOutputStream (good idea? - unclear?)
		>	Possibly use SmallStackBuffer<> inside as impl detail?
		>	Unclear about lifetime of arguments. COULD have CTORs take SharedPtr<> of appropriate 'base stream'. Thats sensible.
		>	UNCELAR how to handle 'seekability' - more variants, or only do buffered wrapper on 'seekable' combinations? Maybe do a TEMPLATED
		>	implmentation so can work with various combos?


	>NEW TextInputStreamAdaptor (subclass from TextInputStream, but takes BinaryInputStream as arg -)
		>	 like with buffered - do we require use of SharedPtr?
		>	Takes as argument input CODEPAGE (so we automatically map from binary data
		>	Takes as argument AUTODETECT INPUT CODEPAGE (very useful - LED already has code for this)
		>	Similarly on TextOutputStreamAdaptor (takes binary stream arg, and flags about codepage to use, and BOM flag)
		>	UNCLEAR if/how to deal with SEEKABLE option? EIther explosiion in classes (not the end of the world) - or 
		>	maybe find a way to use tempaltes to share code? Or revist the idea of baking Seekable directly into Binary/TextInput/OutputStream base classes
		>	NOTE - these classes are meant to be analagous to .Net TextReader  and TextWriter (maybe use those names? but I wanted to empahzie they are like streams?)
		>		(oops - I was wrong - they are analgous to the .Net StreamREader which subclassest from TextReader and StreamWRite rsubclassing from TextWriter) - and then
		>		thats closer naming/functionality


*/



namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {



			/*
			 * Design Overview:
			 *
			 *		o	All read's on a BinaryInputStream are BLOCKING. If there is a desire to have a non-blocking read, then create a new mixin interface
			 *			and through that interface you can do non-blocking reads, but this Read() method must always block.
			 *
			 *		o	EOF is handled by a return value of zero. Once EOF is returned - any subsequent calls to Read () will return EOF (unless some other
			 *			mechanism is used to tweak the state of the object, like a mixed in Seekable class and calling SEEK).
			 *
			 *		o	Exceptions indicate something went wrong - like an IO error, or possibly in some cases a formatting effort (e.g. if the source is encrypted,
			 *			and the stream is decrypting, then it might detect a format error and throw).
			 *
			 *		o	BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make an input/output stream. Similarly, they can both be
			 *			mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
			 */
			class	BinaryInputStream : public virtual Seekable {
				protected:
					BinaryInputStream ();
				
				public:
					// Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
					// BLOCKING until data is available, but can return with fewer bytes than bufSize without prjudice about how much more is available.
					nonvirtual	size_t	Read (Byte* intoStart, Byte* intoEnd);

				protected:
					// Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
					// BLOCKING until data is available, but can return with fewer bytes than bufSize without prejudice about how much more is available.
					virtual	size_t	_Read (Byte* intoStart, Byte* intoEnd)			=	0;
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
