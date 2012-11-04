/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputOutputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "BinaryInputStream.h"
#include    "BinaryOutputStream.h"




/**
 *  \file
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  \brief  BinaryInputOutputStream is a stream that has BOTH a BinaryInputStream and an BinaryOutputStream.
             *
             *  BinaryInputOutputStream could have inherited from BinaryInputOutputStream and BinaryOutputStream, but then
             *  it would have had two copies of the shared_ptr. It seemed more economical to have just one such base,
             *  and make it easy to convert (conversion operator).
             *
             *  A BinaryInputOutputStream is a *combined* input and output stream. That means they are somehow related.
             *  If the BinaryInputOutputStream is seekable, then the same seek offset applies to BOTH the input and
             *  output sides. If you wish to have separate seek offsets, maintain two separate input and output stream
             *  objects.
             *
             *  Note that this concept applies to a file, but NOT to a socket (tcp) stream. A TCP/stream
             *  really consists of two related (paired) streams going in opposite directions.
             */
            class   BinaryInputOutputStream : public Streams::BinaryStream {
            protected:
                /**
                 * BinaryStream::_SharedIRep arg - MUST inherit from BOTH Streams::BinaryInputStream::_IRep AND Streams::BinaryOutputStream::_IRep.
                 *
                 *  \pre dynamic_cast(rep.get (), _SharedInputIRep::element_type) != nullptr
                 *  \pre dynamic_cast(rep.get (), _SharedOutputIRep::element_type) != nullptr
                 */
                BinaryInputOutputStream (const BinaryStream::_SharedIRep& rep);

            protected:
                typedef BinaryInputStream::_SharedIRep      _SharedInputIRep;
                typedef BinaryOutputStream::_SharedIRep     _SharedOutputIRep;

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedInputIRep _GetInputRep () const;

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedOutputIRep _GetOutputRep () const;

            public:
                /**
                 * (UNCLEAR IF SB operator convretion - but we used to just inherit form binaryinputstream
                 *  and that was like this but stronger)
                 */
                nonvirtual  operator BinaryInputStream () const;

            public:
                /**
                 * (UNCLEAR IF SB operator convretion - but we used to just inherit form BinaryOutputStream
                 *  and that was like this but stronger)
                 */
                nonvirtual  operator BinaryOutputStream () const;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prjudice about how much more is available.
                 *
                 *  \pre not empty()
                 */
                nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd);

            public:
                /**
                 * Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                 * Writes always succeed fully or throw.
                 *
                 *  \pre not empty()
                 */
                nonvirtual  void    Write (const Byte* start, const Byte* end);
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryInputOutputStream.inl"
