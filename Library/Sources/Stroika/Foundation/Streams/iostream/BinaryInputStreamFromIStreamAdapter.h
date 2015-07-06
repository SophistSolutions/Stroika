/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../BinaryInputStream.h"



/**
 *  \file
 *
 * TODO:
 *
 *      @todo   Redo as template so handles wchar_t basic_stream too.
 *
 *      @todo   Support seek
 *
 *      @todo   Consider thread-safety. For now, a non-standard choice?
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
                class   BinaryInputStreamFromIStreamAdapter : public BinaryInputStream<> {
                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                public:
                    BinaryInputStreamFromIStreamAdapter (istream& originalStream);
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

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_*/
