/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_
#define _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>

#include    "../../Configuration/Common.h"

#include    "../InputStream.h"



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



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                namespace   InputStreamFromStdIStreamSupport {
                    template    <typename ELEMENT_TYPE>
                    struct  TraitsType {
                        using   IStreamType = basic_istream<ELEMENT_TYPE>;
                    };
                    template    <>
                    struct  TraitsType<Memory::Byte> {
                        using   IStreamType = istream;
                    };
                    template    <>
                    struct  TraitsType<Characters::Character> {
                        using   IStreamType = wistream;
                    };
                }


                /**
                 *  InputStreamFromStdIStream creates a InputStream<ELEMENT_TYPE> wrapper
                 *  on an existing basic_istream<CHARTYPE (e.g. std::istream) object.
                 *  It is required (but un-enforced) that the caller assure the lifetime of the original (argument)
                 *  istream is longer than the lifetiem of this created InputStream (smart pointer).
                 *
                 *  It is also up to the caller to assure no references to or calls to that istream
                 *  be made from another thread. However, no data is cached in this class - it just
                 *  delegates, so calls CAN be made the the underlying istream - so long as not
                 *  concurrently.
                 *
                 *  \note   \em Thread-Safety
                 *      InputStreamFromStdIStream must be externally synchonized, except for purely read-only operations.
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS = InputStreamFromStdIStreamSupport::TraitsType<ELEMENT_TYPE>>
                class   InputStreamFromStdIStream : public InputStream<ELEMENT_TYPE> {
                public:
                    using   IStreamType = typename TRAITS::IStreamType;

                public:
                    enum SeekableFlag { eSeekable, eNotSeekable };

                public:
                    /**
                     *  Default seekability should be determined automatically, but for now, I cannot figure out how...
                     */
                    InputStreamFromStdIStream (IStreamType& originalStream);
                    InputStreamFromStdIStream (IStreamType& originalStream, SeekableFlag seekable);

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
#include    "InputStreamFromStdIStream.inl"

#endif  /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_h_*/
