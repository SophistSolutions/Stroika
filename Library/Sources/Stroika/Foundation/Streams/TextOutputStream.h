/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStream_h_
#define _Stroika_Foundation_Streams_TextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Configuration/Common.h"
#include	"TextStream.h"

/**
 *  \file
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            using   Characters::Character;

            /*
             * Design Overview:
             *
             *      o   TextInputStream and TextOutputStream CAN be naturally mixed togehter to make an input/output stream. Simlarly, they can both be
             *          mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
			 *
			 *		o	One (potential) slight design flaw with this API, is that its not possible to have legal partial writes.
			 *			But not supporting partial writes makes use much simpler (since callers don't need
			 *			to worry about that case), and its practically never useful. In principle - this API could be
			 *			extended so that an exception (or extra method to ask about last write) could include information
			 *			about partial writes, but for now - I don't see any reason.
             */
			class   TextOutputStream : public TextStream {
            protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit TextOutputStream (const _SharedIRep& rep);


            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
				 *	Write the characters bounded by start and end. Start and End maybe equal, and only then can they be nullptr.
				 *
                 *	Writes always succeed fully or throw (no partial writes).
				 */
                nonvirtual  void    Write (const wchar_t* start, const wchar_t* end) const;
                nonvirtual  void    Write (const Character* start, const Character* end) const;
            };

            /**
             *
             */
            class   TextOutputStream::_IRep : public virtual TextStream::_IRep {
            public:
                _IRep ();
                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);

            public:
                /**
				 * pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                 * Writes always succeed fully or throw.
				 */
                virtual void    _Write (const Character* start, const Character* end)           =   0;
            };

		}
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextOutputStream.inl"
