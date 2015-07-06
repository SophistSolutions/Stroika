/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "../../../Streams/BinaryInputStream.h"
#include    "../../../Streams/TextInputStream.h"



/**
 *  \file
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {


                    /**
                     * This can be used to create a readable stream of strings (readline) - which
                     * consume a BinaryStream (pre-seeked to the right place) - as a series of characters.
                     *
                     * This code respects the format defined in http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
                     * and doesnt read more than needed (no pre-buffering).
                     * This lack of pre-buffering (peeking) is critical so that the underlying binary stream
                     * only reads the minimal amount needed and will be placed at the right position when handed
                     * to the next process to interpret.
                     *
                     *  Though this TextInputStream is seekable, it only supports seeking backwards over materials
                     *  already read. It never allows seeking past its last read point (throws not supported).
                     *
                     *  This adpater defines 0 seek offset as the point at which its constructed. And then you can seek to any locaiton
                     *  0 .. up to the max point ever read (with Read).
                     */
                    class   MessageStartTextInputStreamBinaryAdapter : public Streams::TextInputStream {
                    public:
                        MessageStartTextInputStreamBinaryAdapter (const Streams::InputStream<Memory::Byte>& src);
                    private:
                        class   Rep_;
                    };


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_*/
