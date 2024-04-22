/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_
#define _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ostream>

#include "Stroika/Foundation/Configuration/Common.h"

#include "../OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-606 - Implement StdIStreamFromInputStream and StdOStreamFromOutputStream classes
 */

namespace Stroika::Foundation::Streams::iostream::OutputStreamFromStdOStream {

    template <typename ELEMENT_TYPE>
    using Ptr = typename OutputStream::Ptr<ELEMENT_TYPE>;

    /**
     *  OutputStreamFromStdOStream wraps an argument std::ostream or std::wostream or std::basic_ostream<> as a Stroika OutputStream object
     *
     *      \note   OutputStreamFromStdOStream ::Close () does not call close on the owned basic_ostream, because there is no such stdC++ method (though filestream has one)
     *
     *  Default seekability should be determined automatically, but for now, I cannot figure out how...
     *
     *  \par Example Usage
     *      \code
     *          stringstream                                  s;
     *          OutputStreamFromStdOStream<byte>::Ptr so = OutputStreamFromStdOStream<byte>::New (s);
     *          const char                                    kData_[] = "ddasdf3294234";
     *          so.Write (reinterpret_cast<const byte*> (std::begin (kData_)), reinterpret_cast<const byte*> (std::begin (kData_)) + strlen (kData_));
     *          EXPECT_TRUE (s.str () == kData_);
     *      \endcode
     *
     *  \note   The lifetime of the underlying created (shared_ptr) Stream must be <= the lifetime of the argument std::ostream
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *              It is also up to the caller to assure no references to or calls to that ostream
     *              be made from another thread. However, no data is cached in this class - it just
     *              delegates, so calls CAN be made the the underlying ostream - so long as not
     *              concurrently.
     *
     *              If you pass in eInternallySynchronized, the internal rep is internally synchronized, but you still must assure
     *              no other threads access the OStreamType object.
     */
    template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& originalStream)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, wchar_t>));
    template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized                                     internallySynchronized,
                           basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& originalStream)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, wchar_t>));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "OutputStreamFromStdOStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_*/
