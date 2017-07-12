/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_
#define _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_ 1

#include "../../StroikaPreComp.h"

#include <ostream>

#include "../../Configuration/Common.h"

#include "../OutputStream.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                namespace OutputStreamFromStdOStreamSupport {
                    template <typename ELEMENT_TYPE>
                    struct TraitsType {
                        using IStreamType = basic_ostream<ELEMENT_TYPE>;
                    };
                    template <>
                    struct TraitsType<Memory::Byte> {
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
                 *  \note   The lifetime of the underlying created (shared_ptr) Stream must be >= the lifetime of the argument std::ostream
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 *              It is also up to the caller to assure no references to or calls to that ostream
                 *              be made from another thread. However, no data is cached in this class - it just
                 *              delegates, so calls CAN be made the the underlying ostream - so long as not
                 *              concurrently.
                 */
                template <typename ELEMENT_TYPE, typename TRAITS = OutputStreamFromStdOStreamSupport::TraitsType<ELEMENT_TYPE>>
                class OutputStreamFromStdOStream : public OutputStream<ELEMENT_TYPE> {
                public:
                    using OStreamType = typename TRAITS::OStreamType;

                public:
                    /**
                     *  Default seekability should be determined automatically, but for now, I cannot figure out how...
                     *
                     *  \par Example Usage
                     *      \code
                     *          stringstream                                  s;
                     *          OutputStreamFromStdOStream<Memory::Byte>::Ptr so = OutputStreamFromStdOStream<Memory::Byte>{ s };
                     *          const char                                    kData_[] = "ddasdf3294234";
                     *          so.Write (reinterpret_cast<const Byte*> (std::begin (kData_)), reinterpret_cast<const Byte*> (std::begin (kData_)) + strlen (kData_));
                     *          VerifyTestResult (s.str () == kData_);
                     *      \endcode
                     */
                    OutputStreamFromStdOStream () = delete;
                    OutputStreamFromStdOStream (OStreamType& originalStream);
                    OutputStreamFromStdOStream (const OutputStreamFromStdOStream&) = delete;

                public:
                    class Ptr;

                public:
                    /**
                     *  You can construct, but really not use an OutputStreamFromStdOStream object. Convert
                     *  it to a Ptr - to be able to use it.
                     */
                    nonvirtual operator Ptr () const;

                private:
                    class Rep_;

                private:
                    shared_ptr<Rep_> fRep_;
                };

                /**
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
                     *          OutputStreamFromStdOStream<Memory::Byte>::Ptr so = OutputStreamFromStdOStream<Memory::Byte>{ s };
                     *          const char                                    kData_[] = "ddasdf3294234";
                     *          so.Write (reinterpret_cast<const Byte*> (std::begin (kData_)), reinterpret_cast<const Byte*> (std::begin (kData_)) + strlen (kData_));
                     *          VerifyTestResult (s.str () == kData_);
                     *      \endcode
                     */
                    Ptr ()                = default;
                    Ptr (const Ptr& from) = default;

                private:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                    nonvirtual Ptr& operator                   = (const OutputStreamFromStdOStream& rhs);

                private:
                    friend class OutputStreamFromStdOStream;
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

#include "OutputStreamFromStdOStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_h_*/
