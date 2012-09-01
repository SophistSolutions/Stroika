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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  \brief  BinaryInputOutputStream ...
             *
             */
            class   BinaryInputOutputStream : public Streams::BinaryStream {
            protected:
                /**
                 * IRep_ arg - MUST inherit from BOTH Streams::BinaryInputStream::_IRep AND Streams::BinaryOutputStream::_IRep.
                 */
                BinaryInputOutputStream (const Streams::BinaryStream::_SharedIRep& rep);

            protected:
                typedef BinaryInputStream::_SharedIRep      _SharedInputIRep;
                typedef BinaryOutputStream::_SharedIRep     _SharedOutputIRep;

            public:
                /**
                 *
                 */
                nonvirtual  _SharedInputIRep GetInputRep () const;

            public:
                /**
                 *
                 */
                nonvirtual  _SharedOutputIRep GetOutputRep () const;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prjudice about how much more is available.
                 */
                nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd);

            public:
                /**
                 * pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                 * Writes always succeed fully or throw.
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
