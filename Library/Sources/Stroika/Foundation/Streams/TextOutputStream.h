/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStream_h_
#define _Stroika_Foundation_Streams_TextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "TextStream.h"



/**
 *  \file
 *
 *      @todo   Add Close() as with - binaryoutputstream!
 *
 *      @todo   Add Flush() like we have for binaryoutputstream!
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;


            /**
             * Design Overview:
             *
             *      o   TextOutputStream represents a sink for Characters. It may or may not be Seekable, and can be
             *          combined with a TextInputStream (@see TextStream for details).
             *
             *      o   One (potential) slight design flaw with this API, is that its not possible to have legal partial writes.
             *          But not supporting partial writes makes use much simpler (since callers don't need
             *          to worry about that case), and its practically never useful. In principle - this API could be
             *          extended so that an exception (or extra method to ask about last write) could include information
             *          about partial writes, but for now - I don't see any reason.
             *
             *  @See TextStream
             */
            class   TextOutputStream : public TextStream {
            protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable::_IRep - and if so - this automatically uses it.
                 */
                explicit TextOutputStream (const _SharedIRep& rep);


            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  Write the characters bounded by start and end. Start and End maybe equal, and only
                 *  then can they be nullptr.
                 *
                 *  Writes always succeed fully or throw (no partial writes).
                 *
                 *  Write/1 (cstr arg) assumes its argument is NUL-terminated, and does not write the trailing NUL-character.
                 */
                nonvirtual  void    Write (const wchar_t* start, const wchar_t* end) const;
                nonvirtual  void    Write (const Character* start, const Character* end) const;
                nonvirtual  void    Write (const wchar_t* cStr) const;
                nonvirtual  void    Write (const String& s) const;
            };


            /**
             *
             */
            class   TextOutputStream::_IRep : public virtual TextStream::_IRep {
            public:
                _IRep ();
                _IRep (const _IRep&) = delete;

            public:
                nonvirtual  const _IRep& operator= (const _IRep&) = delete;


            public:
                /**
                 * pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                 * Writes always succeed fully or throw.
                 */
                virtual void    Write (const Character* start, const Character* end)           =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_TextOutputStream_h_*/
