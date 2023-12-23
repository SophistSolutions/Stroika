/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-606 - Implement StdIStreamFromInputStream and StdOStreamFromOutputStream classes
 *
 *      @todo   Better handle failbit / eofbit etc. Not correct, but maybe OK for now... Not sure right answer.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probbaly be made more efficent in sync form - using direct mutex
 *
 */

namespace Stroika::Foundation::Streams::iostream ::InputStreamFromStdIStream {

    template <typename ELEMENT_TYPE>
    struct TraitsType {
        using IStreamType = basic_istream<ELEMENT_TYPE>;
    };
    template <>
    struct TraitsType<byte> {
        using IStreamType = istream;
    };
    template <>
    struct TraitsType<Characters::Character> {
        using IStreamType = wistream;
    };

    template <typename ELEMENT_TYPE>
    using Ptr = typename InputStream<ELEMENT_TYPE>::Ptr;

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
     */

    enum class SeekableFlag {
        eSeekable,
        eNotSeekable
    };
    static constexpr SeekableFlag eSeekable    = SeekableFlag::eSeekable;
    static constexpr SeekableFlag eNotSeekable = SeekableFlag::eNotSeekable;

    /**
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
    template <typename ELEMENT_TYPE, typename TRAITS = TraitsType<ELEMENT_TYPE>>
    Ptr<ELEMENT_TYPE> New (typename TraitsType<ELEMENT_TYPE>::IStreamType& originalStream);
    template <typename ELEMENT_TYPE, typename TRAITS = TraitsType<ELEMENT_TYPE>>
    Ptr<ELEMENT_TYPE> New (typename TraitsType<ELEMENT_TYPE>::IStreamType& originalStream, SeekableFlag seekable);
    template <typename ELEMENT_TYPE, typename TRAITS = TraitsType<ELEMENT_TYPE>>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, typename TraitsType<ELEMENT_TYPE>::IStreamType& originalStream);
    template <typename ELEMENT_TYPE, typename TRAITS = TraitsType<ELEMENT_TYPE>>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized               internallySynchronized,
                           typename TraitsType<ELEMENT_TYPE>::IStreamType& originalStream, SeekableFlag seekable);

    template <typename ELEMENT_TYPE, typename TRAITS = TraitsType<ELEMENT_TYPE>>
    class Rep_;
#if 0
        //tmphack disable
    private:
        using InternalSyncRep_ =
            InternallySynchronizedInputStream<ELEMENT_TYPE, InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>, typename InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Rep_>;
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStreamFromStdIStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_*/
