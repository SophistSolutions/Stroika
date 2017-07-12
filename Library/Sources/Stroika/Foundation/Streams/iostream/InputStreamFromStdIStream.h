/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_
#define _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_ 1

#include "../../StroikaPreComp.h"

#include <istream>

#include "../../Configuration/Common.h"

#include "../InputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   names iostream:: InputStreamFromStdIStream = reason is cuz we want to also have InputStreamAsStdIStream –
 *              something that IS an iostream (PITA to write)
 *
 *      @todo   Better handle failbit / eofbit etc. Not correct, but maybe OK for now... Not sure right answer.
 *
 *      @todo   IMPLEMENT InputStreamFromStdIStream and OutputStreamFromStdOStream
 *              see http://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream
 *              http://www.math.utah.edu/docs/info/iostream_5.html
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                namespace InputStreamFromStdIStreamSupport {
                    template <typename ELEMENT_TYPE>
                    struct TraitsType {
                        using IStreamType = basic_istream<ELEMENT_TYPE>;
                    };
                    template <>
                    struct TraitsType<Memory::Byte> {
                        using IStreamType = istream;
                    };
                    template <>
                    struct TraitsType<Characters::Character> {
                        using IStreamType = wistream;
                    };
                }

                /**
                 *  InputStreamFromStdIStream wraps an argument std::istream or std::wistream or std::basic_istream<> as a Stroika InputStream object
                 *
                 *  \note   The lifetime of the underlying created (shared_ptr) Stream must be >= the lifetime of the argument std::istream
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 *              It is also up to the caller to assure no references to or calls to that istream
                 *              be made from another thread. However, no data is cached in this class - it just
                 *              delegates, so calls CAN be made the the underlying istream - so long as not
                 *              concurrently.
                 *
                 *  \par Example Usage
                 *      \code
                 *      stringstream tmpStrm;
                 *      tmpStrm << "some xml";
                 *      XML::SAXParse (InputStreamFromStdIStream<Memory::Byte> (tmpStrm), MyCallback ());
                 *      \endcode
                 */
                template <typename ELEMENT_TYPE, typename TRAITS = InputStreamFromStdIStreamSupport::TraitsType<ELEMENT_TYPE>>
                class InputStreamFromStdIStream : public InputStream<ELEMENT_TYPE> {
                public:
                    using IStreamType = typename TRAITS::IStreamType;

                public:
                    enum SeekableFlag {
                        eSeekable,
                        eNotSeekable
                    };

                public:
                    /**
                     *
                     *  Default seekability should be determined automatically, but for now, I cannot figure out how...
                     *  \par Example Usage
                     *      \code
                     *          stringstream tmpStrm;
                     *          WriteTextStream_ (newDocXML, tmpStrm);
                     *          return InputStream<Memory::Byte>::Ptr{ InputStreamFromStdIStream<Memory::Byte> (tmpStrm) }.ReadAll ();
                     *      \endcode
                     *
                     *  \par Example Usage
                     *      \code
                     *          stringstream tmpStrm;
                     *          WriteTextStream_ (newDocXML, tmpStrm);
                     *          MyCallback myCallback;
                     *          XML::SAXParse (InputStreamFromStdIStream<Memory::Byte> (tmpStrm), myCallback);
                     *      \endcode
                     */
                    InputStreamFromStdIStream ()                                 = delete;
                    InputStreamFromStdIStream (const InputStreamFromStdIStream&) = delete;
                    InputStreamFromStdIStream (IStreamType& originalStream);
                    InputStreamFromStdIStream (IStreamType& originalStream, SeekableFlag seekable);

                public:
                    class Ptr;

                public:
                    /**
                     *  You can construct, but really not use an InputStreamFromStdIStream object. Convert
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
                class InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Ptr : public InputStream<ELEMENT_TYPE>::Ptr {
                private:
                    using inherited = typename InputStream<ELEMENT_TYPE>::Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          stringstream tmpStrm;
                     *          WriteTextStream_ (newDocXML, tmpStrm);
                     *          return InputStream<Memory::Byte>::Ptr{ InputStreamFromStdIStream<Memory::Byte> (tmpStrm) }.ReadAll ();
                     *      \endcode
                     */
                    Ptr ()                = default;
                    Ptr (const Ptr& from) = default;

                private:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                    nonvirtual Ptr& operator                   = (const InputStreamFromStdIStream& rhs);

                private:
                    friend class InputStreamFromStdIStream;
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
#include "InputStreamFromStdIStream.inl"

#endif /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_*/
