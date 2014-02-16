/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"
#include    "TextInputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
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


            /**
             *
             */
            class   TextInputStreamBinaryAdapter : public TextInputStream {
            public:
                TextInputStreamBinaryAdapter (const BinaryInputStream& src);
            private:
                class   IRep_;
            private:
                shared_ptr<_IRep> mk_ (const BinaryInputStream& src);
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
