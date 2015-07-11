/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_InputStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../InputStream.h"



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
                 *      @todo OBSOLETE DOCS
                 *
                 *  InputStreamAdapter creates a BinaryInputSteam wrapper
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
                 *      InputStreamAdapter is not necessarily thread safe.
                 *      Its roughly as safe as the underlying istream implementation, except
                 *      that we call read, followed by gcount () - which could be a race.
                 */
                template    <typename   ELEMENT_TYPE>
                class   InputStreamAdapter : public InputStream<ELEMENT_TYPE> {
                public:
                    InputStreamAdapter (istream& originalStream);

                private:
                    class   Rep_;
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
#include    "InputStreamAdapter.inl"

#endif  /*_Stroika_Foundation_Streams_iostream_InputStreamAdapter_h_*/
