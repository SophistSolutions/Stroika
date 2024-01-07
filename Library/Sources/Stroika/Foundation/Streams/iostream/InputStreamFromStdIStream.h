/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_
#define _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_ 1

#include "../../StroikaPreComp.h"

#include <istream>

#include "../../Configuration/Common.h"

#include "../InputStream.h"
#include "../InternallySynchronizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-606 - Implement StdIStreamFromInputStream and StdOStreamFromOutputStream classes
 *
 *      @todo   Better handle failbit / eofbit etc. Not correct, but maybe OK for now... Not sure right answer.
 *
 */

namespace Stroika::Foundation::Streams::iostream::InputStreamFromStdIStream {

    using InputStream::Ptr;

    /**
     *  InputStreamFromStdIStream wraps an argument std::istream or std::wistream or std::basic_istream<> as a Stroika InputStream object
     *
     *  \par Example Usage
     *      \code
     *          stringstream tmpStrm;
     *          tmpStrm << "some xml";
     *          XML::SAXParse (InputStreamFromStdIStream::New<byte> (tmpStrm), MyCallback{});
     *      \endcode
     *
     *      \note   InputStreamFromStdIStream ::Close () does not call close on the owned basic_istream, because there is no such stdC++ method (though filestream has one)
     *
     *
     *  Default seekability should be determined automatically, but for now, I cannot figure out how...
     *  \par Example Usage
     *      \code
     *          stringstream tmpStrm;
     *          WriteTextStream_ (newDocXML, tmpStrm);
     *          return InputStreamFromStdIStream::New<byte>(tmpStrm).ReadAll ();
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          stringstream tmpStrm;
     *          WriteTextStream_ (newDocXML, tmpStrm);
     *          MyCallback myCallback;
     *          XML::SAXParse (InputStreamFromStdIStream::New<byte> (tmpStrm), myCallback);
     *      \endcode
     *
     *  \note   The lifetime of the underlying created (shared_ptr) Stream must be >= the lifetime of the argument std::istream
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *              It is also up to the caller to assure no references to or calls to that istream
     *              be made from another thread. However, no data is cached in this class - it just
     *              delegates, so calls CAN be made the the underlying istream - so long as not
     *              concurrently.
     *
     *              If you pass in eInternallySynchronized, the internal rep is internally synchronized, but you still must assure
     *              no other threads access the IStreamType object.
     */
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>));
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>));
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized                                     internallySynchronized,
                           basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>));
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized                                     internallySynchronized,
                           basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable)
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStreamFromStdIStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_*/
