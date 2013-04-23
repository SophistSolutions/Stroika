/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"

#include    "BinaryStream.h"



/**
 *  \file
 *
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where
 *              it was a PROPERTY OF THE CLASS (or alternate API) where writes timeout after
 *              a certain point.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Memory::Byte;


            class   BinaryInputOutputStream;


            /**
             * Design Overview:
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make
             *          an input/output stream. Simlarly, they can both be mixed together with Seekable.
             *          But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             *
             *  Note - when you Seek() away from the end of a binary output stream, and then write, you automatically
             *  extend the stream to the point seeked to, and if you seek back (less) than the end and write, this overwrites
             *  instead of inserting.
             */
            class   BinaryOutputStream : public BinaryStream {
            protected:
            public:
                class   _IRep;

			public:
                /**
				 *	defaults to null (empty ())
                 */
				BinaryOutputStream ();

			protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                BinaryOutputStream (const _SharedIRep& rep);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  Write the bytes bounded by start and end. Start and End maybe equal, and only
                 *  then can they be nullptr.
                 *
                 *  Writes always succeed fully or throw (no partial writes).
                 */
                nonvirtual  void    Write (const Byte* start, const Byte* end) const;

            public:
                /**
                 *  \brief forces any data contained in this stream to be written.
                 *
                 *  Forces any data contained in this stream to be written.
                 *
                 *  For some streams (such as buffered streams, sockets, and OpenSSLCryptStream) - they may not
                 *  finish their writes until they are destroyed. The trouble then - is that they cannot
                 *  propagate exceptions! Calling Flush() before destroying the output stream allows exceptions
                 *  to be propagated properly.
                 */
                nonvirtual  void    Flush () const;

            private:
                friend  class   BinaryInputOutputStream;
            };



            /**
             *
             */
            class   BinaryOutputStream::_IRep : public virtual BinaryStream::_IRep {
            public:
                _IRep ();
                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  BufSize must always be >= 1.
                 *  Writes always succeed fully or throw.
                 */
                virtual void    Write (const Byte* start, const Byte* end)         =   0;

            public:
                /**
                 *
                 */
                virtual void    Flush ()         =   0;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryOutputStream.inl"
