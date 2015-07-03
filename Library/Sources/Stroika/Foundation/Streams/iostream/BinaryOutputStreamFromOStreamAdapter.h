/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <ostream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../BinaryOutputStream.h"
#include    "../Seekable.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   Redo as template so handles wchar_t basic_stream too.
 *
 *      @todo   Consider mixing in Seekable here. Would be easy.
 *
 *      @todo   Consider thread-safety. For now, a non-standard choice?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                /**
                 *  BinaryOutputStreamFromOStreamAdapter creates a BinaryOutputSteam wrapper
                 *  on an existing std::ostream object. It is required (but un-enforced)
                 *  that the caller assure the lifetime of the original (argument) ostream
                 *  is longer than the lifetiem of this created BinaryOutputStream (smart pointer).
                 *
                 *  It is also up to the caller to assure no references to or calls to that ostream
                 *  be made from another thread. However, no data is cached in this class - it just
                 *  delegates, so calls CAN be made the the underlying ostream - so long as not
                 *  concurrently.
                 *
                 *  \note   \em Thread-Safety
                 *      BinaryOutputStreamFromOStreamAdapter is not necessarily thread safe.
                 *      Its roughly as safe as the underlying ostream implementation, except
                 *      that we call read, followed by gcount () - which could be a race.
                 */
                class   BinaryOutputStreamFromOStreamAdapter : public BinaryOutputStream {
                private:
                    class   Rep_;

                public:
                    BinaryOutputStreamFromOStreamAdapter (ostream& originalStream);
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_*/
