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
            class   BinaryInputOutputStream : public Streams::BinaryInputStream, public Streams::BinaryOutputStream {
            protected:
                /**
                 * IRep_ arg - MUST inherit from BOTH Streams::BinaryInputStream::_IRep AND Streams::BinaryOutputStream::_IRep.
                 */
                BinaryInputOutputStream (const Streams::BinaryStream::_SharedIRep& rep);
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
