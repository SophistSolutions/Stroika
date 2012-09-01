/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "BinaryStream.h"




/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Unclear about lifetime of stream objects. Right now - the overall design is agnostic.
 *              But I could redo class X as X (envelope) and X::IRep is implementaiton using shared_ptr<>.
 *              This would have the advantage of making the lifetime issues clear and simple.
 *              But right now - so far - things seem to be going OK without that.
 *
 *      @todo   Consider template basic_istream<>
 *              and do of BYTE and CHARACTER (for binary and text i/ostreams). Abstract bases.
 *              Points the way towards creating streams of OTHER sorts of objects if desired.
 *
 *      @todo   Explain in docs how Stroika streams differ from iostream
 *              o   MUCH MUCH easier to implement your own complaint stream
 *              o   Separarate interface from implementation (thats why easier to implement)
 *              o   Doesn’t mix text with binary APIs. Keeps them logically separate (view as
 *                  you will – diff – if better or worse)
 *
 *      @todo   Consider if we should use
 *              typedef shared_ptr<BinaryInputStream>   BinaryInputStreamPtr;
 *              or just use direct classes. Very unclear.
 *
 *      @todo   Perhaps add a GetBytesAvailable() method. This is effectively like converting
 *              blocking to safe read calls...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

			class   BinaryInputOutputStream;

			
			/**
             *  \brief  BinaryInputStream is an abstract class defining the interface to a binary source of data.
             *
             * Design Overview:
             *
             *      o   All read's on a BinaryInputStream are BLOCKING. If there is a desire to have a
             *          non-blocking read, then create a new mixin interface and through that interface
             *          you can do non-blocking reads, but this Read() method must always block.
             *
             *      o   EOF is handled by a return value of zero. Once EOF is returned - any subsequent
             *          calls to Read () will return EOF (unless some other mechanism is used to tweak
             *          the state of the object, like a mixed in Seekable class and calling SEEK).
             *
             *      o   Exceptions indicate something went wrong - like an IO error, or possibly in some cases
             *          a formatting effort (e.g. if the source is encrypted, and the stream is decrypting,
             *          then it might detect a format error and throw).
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make an
             *          input/output stream. Similarly, they can both be mixed together with Seekable.
             *          But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             */
            class   BinaryInputStream : public BinaryStream {
            protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                BinaryInputStream (const _SharedIRep& rep);


            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prjudice about how much more is available.
                 */
                nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd);

			private:
				friend	class   BinaryInputOutputStream;
            };


            /**
             *
             */
            class   BinaryInputStream::_IRep : public virtual BinaryStream::_IRep {
            public:
                _IRep ();
                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 */
                virtual size_t  Read (Byte* intoStart, Byte* intoEnd)          =   0;
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryInputStream.inl"
