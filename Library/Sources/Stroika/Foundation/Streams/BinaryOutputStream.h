/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"

#include    "OutputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              (maybe close/flush ignored).
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Add abiiliy to SetEOF (); You can SEEK, but if you seek backwards, and start writing - that doesnt change EOF. EOF
 *              remains fixed as max written to. DODUCMNET THIS (for text and binary) - and provide a SetEOF() method
 *              (maybe just for seekable streams)? Maybe add rule that SetEOF () can only go backwards (shorten). Then call
 *              PullBackEOF() or RestrictEOF() or RemovePast(); OR ResetEOFToCurrentPosiiton(). Later maybe best API.
 *
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where
 *              it was a PROPERTY OF THE CLASS (or alternate API) where writes timeout after
 *              a certain point.
 */



#pragma message ("Warning: BinaryOutputStream FILE DEPRECATED - use OutputStream<Byte>")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Memory::Byte;

            using BinaryOutputStream = OutputStream<Byte>;




        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BinaryOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_h_*/
