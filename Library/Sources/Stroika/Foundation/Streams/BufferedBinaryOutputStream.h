/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "BinaryOutputStream.h"



/**
 *  \file
 *
 *      @todo   make threadsafe
 *
 *      @todo   Think out Seekable. Probably MUST mixin Seekable, and properly handle
 *              (vai throw OperaitonNotSupported) if one constructs a BufferedBinaryOutputStream
 *              and tries to SEEK wihout the undelrying stream being seekable.
 *
 *      @todo   BufferedBinaryOutputStream NOW must properly support SEEKABLE! if arg
 *              is seekable, we must override seek methods, and forward them, and adjust buffer as appropriate.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  A BufferedBinaryOutputStream wraps an argument stream
             *  (which must have lifetime > this BufferedBinaryOutputStream) and will buffer up writes to it.
             *
             *      \note   If you fail to Flush() this object before it is destroyed, exceptions in flushing
             *              the data may be suppressed.
             */
            class   BufferedBinaryOutputStream : public BinaryOutputStream {
            private:
                typedef BinaryOutputStream  inherited;

            private:
                class   IRep_;

            public:
                BufferedBinaryOutputStream (const BinaryOutputStream& realOut);

            public:
                nonvirtual  size_t  GetBufferSize () const;
                nonvirtual  void    SetBufferSize (size_t bufSize);

            public:
                /**
                 *  Throws away all data about to be written (buffered). Once this is called,
                 *  its illegal to call Flush or another write
                 */
                nonvirtual  void    Abort ();

            public:
                /**
                 */
                nonvirtual  void    Flush ();
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedBinaryOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_*/
