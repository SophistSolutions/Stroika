/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../BinaryInputStream.h"
#include    "../Seekable.h"



/*
 * TODO:
 *          o   Consider mixing in Seekable here. Would be easy
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                /**
                 *  BinaryInputStreamFromIStreamAdapter creates a BinaryInputSteam wrapper
                 *  on an existing std::istream object. It is required (but un-enforced)
                 *  that the caller assure the lifetime of the original (argument) istream
                 *  is longer than the lifetiem of this created BinaryInputStream (smart pointer).
                 *
                 *  It is also up to the caller to assure no references to or calls to that istream
                 *  be made from another thread. However, no date is cached in this class - it just
                 *  delegates, so calls CAN be made the the underlying istream - so long as not
                 *  concurrently.
                 *
                 *  \note   \em Thread-Safety
                 *      BinaryInputStreamFromIStreamAdapter is not necessarily thread safe.
                 *      Its roughly as safe as the underlying istream implementation, except
                 *      that we call read, followed by gcount () - which could be a race.
                 */
                class   BinaryInputStreamFromIStreamAdapter : public BinaryInputStream {
                private:
                    class   IRep_;
                public:
                    BinaryInputStreamFromIStreamAdapter (istream& originalStream);
                };


            }
        }
    }
}

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
