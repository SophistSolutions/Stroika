/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ToSeekableInputStream_h_
#define _Stroika_Foundation_Streams_ToSeekableInputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Streams::ToSeekableInputStream {

    using InputStream::Ptr;

    /**
     *  Not all input streams are seekable, but throwing a little memory at it, its easy to assure they are all
     *  seekable.
     * 
     *  That's what this utility does: maps the given input stream into a functionally identical one, except possibly
     *  adding seekability.
     * 
     *  If the argument stream is already seekable, New () just returns its argument (so perhaps a misnomer but I thought better
     *  to follow factory pattern).
     * 
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<byte> in = ToSeekableInputStream::New<byte> (existingInputStream);
     *      \endcode
     * 
     *  A socket is the canonical stream you might want this for: there is nowhere to seek back TO, because
     *  the bytes are gone once read (IO::Network::SocketStream::IsSeekable () returns false; so does
     *  InputStreamFromStdIStream constructed eNotSeekable - over cin, say). Wrapping one lets code that
     *  must read the same input twice do so:
     * 
     *  \par Example Usage
     *      \code
     *          SocketStream::Ptr      socketStream = SocketStream::New (connectionSocket);
     *          InputStream::Ptr<byte> in          = ToSeekableInputStream::New<byte> (socketStream);
     *          DoFirstPass (in);
     *          in.Seek (0);        // legal only because of the wrapper; socketStream itself cannot seek
     *          DoSecondPass (in);  // sees the very same bytes again, served out of the cache
     *      \endcode
     * 
     *  That two-pass shape is why this exists: see Providers::LibXML2::Provider::SAXParse (), which needs
     *  it when asked to both validate against a schema and report parse events, because that reads twice.
     * 
     *  \note this helper does not require it be given the input stream at SeekOffset 0, but for pretty obvious reasons
     *        it cannot produce a stream that permits seeking backwards from where it starts. This is checked via assertions.
     * 
     *  \note this helper may be problematic with very large streams, as it caches the stream in memory as it reads, and would
     *        eventually run out.
     * 
     *  \note ELEMENT_TYPE is effectively limited to byte today: SeekRead () accumulates into the cache through
     *        a hardcoded 'byte' buffer, so instantiating this for any other element type will not compile.
     */
    template <typename ELEMENT_TYPE>
    auto New (const Ptr<ELEMENT_TYPE>& in) -> Ptr<ELEMENT_TYPE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ToSeekableInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ToSeekableInputStream_h_*/
