/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryTiedStreams_h_
#define _Stroika_Foundation_Streams_BinaryTiedStreams_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "BinaryInputStream.h"
#include    "BinaryOutputStream.h"
#include    "BinaryInputOutputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *          @todo   Do a better job TIEING these together. For example assert about no seekability? Or ???
 *                  somehow better handle calls to common seek interface? Maybe make seek private/issekable, so you
 *                  have to cast to right subobject...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


//@todo - REPALCE THIS WITH RELAXED DEFINITION OF BinaryInputOutputStream - not necesarily so closely related


#if 1
            using BinaryTiedStreams = BinaryInputOutputStream;
#else


            /**
             *  \brief  BinaryTiedStreams are logically two closely related streams - one input and one output
             *
             *  A BinaryTiedStreams is really two separate streams - tied together into one object. The typical
             *  use-case for a BinaryTiedStream is a communications channel with another process. So for example,
             *  pipes, or a TCP-socket-stream (IO::Network::SocketStream) would be 'tied'.
             *
             *  BinaryTiedStreams are generally not seekable, but if you want to seek, you must first cast to
             *  the appropriate subobject (either the BinaryInputStream or BinaryOutputStream to get the right
             *  stream object, and then seek on that.
             *
             *  Note that this is significantly different from a BinaryInputOutputStream (which represents
             *  a single stream that can both be written two and read from).
             *
             *  The input and output streams MAY shared a common shared_ptr<> rep, but are not required to.
             *
             *  Note - this interpreation of the word "TIED" is perhaps misleading different from the meaning used in
             *  std::iostream!
             */
            class   BinaryTiedStreams : public Streams::BinaryInputStream, public Streams::BinaryOutputStream {
            protected:
                /**
                 * BinaryStream::_SharedIRep arg - MUST inherit from BOTH Streams::BinaryInputStream::_IRep AND Streams::BinaryOutputStream::_IRep.
                 *
                 *  \pre RequireMember (inOutRep.get (), BinaryInputStream::_IRep);
                 *  \pre RequireMember (inOutRep.get (), BinaryOutputStream::_IRep);
                 */
                BinaryTiedStreams (const BinaryStream::_SharedIRep& inOutRep);
                /**
                 * Can provide any two (logically related) streams.
                 */
                BinaryTiedStreams (const BinaryInputStream& in, const BinaryOutputStream& out);
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryTiedStreams.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryTiedStreams_h_*/

