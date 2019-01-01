/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_
#define _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_ 1

#include "../../StroikaPreComp.h"

#include <ostream>

#include "../../Configuration/Common.h"

#include "../InternallySynchronizedOutputStream.h"
#include "../OutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-606 - Implement StdIStreamFromInputStream and StdOStreamFromOutputStream classes
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probbaly be made more efficent in sync form - using direct mutex
 */

namespace Stroika::Foundation::Streams::iostream {

    namespace OutputStreamFromStdOStreamSupport {
        template <typename ELEMENT_TYPE>
        struct TraitsType {
            using IStreamType = basic_ostream<ELEMENT_TYPE>;
        };
        template <>
        struct TraitsType<byte> {
            using OStreamType = ostream;
        };
        template <>
        struct TraitsType<Characters::Character> {
            using OStreamType = wostream;
        };
    }

    /**
     *  OutputStreamFromStdOStream wraps an argument std::ostream or std::wostream or std::basic_ostream<> as a Stroika OutputStream object
     *
     *      \note   OutputStreamFromStdOStream ::Close () does not call close on the owned basic_ostream, because there is no such stdC++ method (though filestream has one)
     */
    template <typename ELEMENT_TYPE, typename TRAITS = OutputStreamFromStdOStreamSupport::TraitsType<ELEMENT_TYPE>>
    class OutputStreamFromStdOStream : public OutputStream<ELEMENT_TYPE> {
    public:
        using OStreamType = typename TRAITS::OStreamType;

    public:
        OutputStreamFromStdOStream ()                                  = delete;
        OutputStreamFromStdOStream (const OutputStreamFromStdOStream&) = delete;

    public:
        class Ptr;

    public:
        /**
         *  Default seekability should be determined automatically, but for now, I cannot figure out how...
         *
         *  \par Example Usage
         *      \code
         *          stringstream                                  s;
         *          OutputStreamFromStdOStream<byte>::Ptr so = OutputStreamFromStdOStream<byte>::New (s);
         *          const char                                    kData_[] = "ddasdf3294234";
         *          so.Write (reinterpret_cast<const byte*> (std::begin (kData_)), reinterpret_cast<const byte*> (std::begin (kData_)) + strlen (kData_));
         *          VerifyTestResult (s.str () == kData_);
         *      \endcode
         *
         *  \note   The lifetime of the underlying created (shared_ptr) Stream must be >= the lifetime of the argument std::ostream
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#Rep-Inside-Ptr-Must-Be-Externally-Synchronized">Rep-Inside-Ptr-Must-Be-Externally-Synchronized</a>
         *              It is also up to the caller to assure no references to or calls to that ostream
         *              be made from another thread. However, no data is cached in this class - it just
         *              delegates, so calls CAN be made the the underlying ostream - so long as not
         *              concurrently.
         *
         *              If you pass in eInternallySynchronized, the internal rep is internally synchronized, but you still must assure
         *              no other threads access the OStreamType object.
         */
        static Ptr New (OStreamType& originalStream);
        static Ptr New (Execution::InternallySynchronized internallySynchronized, OStreamType& originalStream);

    private:
        class Rep_;

    private:
        using InternalSyncRep_ = InternallySynchronizedOutputStream<ELEMENT_TYPE, OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>, typename OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Rep_>;
    };

    /**
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE, typename TRAITS>
    class OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Ptr : public OutputStream<ELEMENT_TYPE>::Ptr {
    private:
        using inherited = typename OutputStream<ELEMENT_TYPE>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          stringstream                                  s;
         *          OutputStreamFromStdOStream<byte>::Ptr so = OutputStreamFromStdOStream<byte>::New (s);
         *          const char                                    kData_[] = "ddasdf3294234";
         *          so.Write (reinterpret_cast<const byte*> (std::begin (kData_)), reinterpret_cast<const byte*> (std::begin (kData_)) + strlen (kData_));
         *          VerifyTestResult (s.str () == kData_);
         *      \endcode
         */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;

    protected:
        Ptr (const shared_ptr<Rep_>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;

    private:
        friend class OutputStreamFromStdOStream;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "OutputStreamFromStdOStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_*/
