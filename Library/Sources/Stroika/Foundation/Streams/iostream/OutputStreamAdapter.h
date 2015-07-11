/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_OutputStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <ostream>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../OutputStream.h"



/**
 *  \file
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                namespace  OutputStreamAdapterSupport {
                    template    <typename ELEMENT_TYPE>
                    struct  TraitsType {
                        using   IStreamType = basic_ostream<ELEMENT_TYPE>;
                    };
                    template    <>
                    struct  TraitsType<Memory::Byte> {
                        using   OStreamType = ostream;
                    };
                    template    <>
                    struct  TraitsType<Characters::Character> {
                        using   OStreamType = wostream;
                    };
                }


                /**
                 *      @todo OBSOLETE DOCS
                 *
                 *  OutputStreamAdapter creates a BinaryOutputSteam wrapper
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
                 *      OutputStreamAdapter is not necessarily thread safe.
                 *      Its roughly as safe as the underlying ostream implementation, except
                 *      that we call read, followed by gcount () - which could be a race.
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS = OutputStreamAdapterSupport::TraitsType<ELEMENT_TYPE>>
                class   OutputStreamAdapter : public OutputStream<ELEMENT_TYPE> {
                public:
                    using   OStreamType = typename TRAITS::OStreamType;

                public:
                    OutputStreamAdapter (OStreamType& originalStream);

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

#include    "OutputStreamAdapter.inl"

#endif  /*_Stroika_Foundation_Streams_iostream_OutputStreamAdapter_h_*/
