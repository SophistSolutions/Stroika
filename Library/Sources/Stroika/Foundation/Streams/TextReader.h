/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_h_
#define _Stroika_Foundation_Streams_TextReader_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "InputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *      @todo   DOCUMENT why we put ReadLine etc in InputStream, instead of here. Gist of why - though more
 *              logical here - requires no state - and so more flexible there. May reconsider.
 *              -- LGP 2015-07-06
 *
 *      @todo   Cruddy, draft (but technically I think correct) implementation. Really need something
 *              smarter integrated with CodePage stuff - and Binary-Stream-based CodePage API, with
 *              an extra 'state' object to allow for pushing bytes in and getting none, and then one,
 *              or perhaps a pull API, where you pull (BinartInputStream) arg - or lambda - to
 *              pull text from.
 *
 *              BETTER YET - USE codecvt_utf8<> code in TextOutputStreamBinaryAdapter
 *
 *      @todo   BUGGY with seek fucntionality. We ASSUME we always get called to read with the source
 *              offset at zero. We dont even know that the source is seekable, but we
 *              certainly dont know that!
 *
 *      @todo   Ultimately - do several implementations upon construction, depending on whether
 *              or not the source is Seekable. Maybe also take configuration (CTOR) params to
 *              indicate whether or not caching is desired (required to implement seekability on
 *              non-seekable sources).
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;


            /**
             *  @todo CLEANUP DOCS - EXPLAIN WHY TAKES BIN OR TEXT STREAM - PASSTHROUGH TEXTSTREAM AND MAP/ENCODE BINSTREAM.
             *
             *  WAS CALLED TextInputStreamBinaryAdapter
             */
            class   TextReader : public InputStream<Character> {
            private:
                using   inherited = InputStream<Character>;

            public:
                TextReader (const InputStream<Memory::Byte>& src);
                TextReader (const InputStream<Character>& src);

            private:
                class   Rep_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextReader.inl"

#endif  /*_Stroika_Foundation_Streams_TextReader_h_*/
