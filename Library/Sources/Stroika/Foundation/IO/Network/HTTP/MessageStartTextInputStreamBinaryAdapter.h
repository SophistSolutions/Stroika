/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter {

    /**
     * This can be used to create a readable stream of strings (readline) - which
     * consume a BinaryStream (pre-seeked to the right place) - as a series of characters.
     *
     * This code respects the format defined in http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
     * and doesn't read more than needed (no pre-buffering).
     * This lack of pre-buffering (peeking) is critical so that the underlying binary stream
     * only reads the minimal amount needed and will be placed at the right position when handed
     * to the next process to interpret.
     *
     *  Though this InputStream::Ptr<Character> is seekable, it only supports seeking backwards over materials
     *  already read. It never allows seeking past its last read point (throws not supported).
     *
     *  This adpater defines 0 seek offset as the point at which its constructed. And then you can seek to any locaiton
     *  0 .. up to the max point ever read (with Read).
     *
     *  \par TODO
     *      @todo   Must rewrite this to properly handle other than ASCII characterset on incoming headers
     *              and URL lines - https://stroika.atlassian.net/browse/STK-969
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    // class MessageStartTextInputStreamBinaryAdapter : public Streams::InputStream<Characters::Character> {
    // public:
    class Ptr;

    //   public:
    /**
        */
    enum ToStringFormat {
        eAsBytes,
        eAsString,

        eDEFAULT = eAsString
    };

    //  public:
    /**
         */
    Ptr New (const Streams::InputStream::Ptr<byte>& src);

    // private:
    class Rep_;
    //  };

    /**
     *  Ptr is a copyable smart pointer to a MessageStartTextInputStreamBinaryAdapter.
     */
    class Ptr : public Streams::InputStream::Ptr<Characters::Character> {
    private:
        using inherited = typename Streams::InputStream::Ptr<Characters::Character>;

    public:
        Ptr ()           = default;
        Ptr (const Ptr&) = default;
        Ptr (const shared_ptr<Streams::InputStream::IRep<Characters::Character>>& from);

    public:
        /*
         *  Do low level non-blocking reads to assure all the bytes (but not a single byte more) is read in and available to be read in a blocking
         *  fasion.
         *
         *  Return false if not available, and return true if all read in. This automatically resets the Read seek pointer to 0.
         *
         *  \note - this returns true EVEN if the header is incomplete on EOF - so the caller can use this as a test to see if its time to try to read the header).
         */
        nonvirtual bool AssureHeaderSectionAvailable ();

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString (ToStringFormat format = ToStringFormat::eDEFAULT) const;
    };

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_*/
