/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Streams/InputStream.h"

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                namespace HTTP {

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
                     *  Though this InputStream<Character>::Ptr is seekable, it only supports seeking backwards over materials
                     *  already read. It never allows seeking past its last read point (throws not supported).
                     *
                     *  This adpater defines 0 seek offset as the point at which its constructed. And then you can seek to any locaiton
                     *  0 .. up to the max point ever read (with Read).
                     */
                    class MessageStartTextInputStreamBinaryAdapter : public Streams::InputStream<Characters::Character> {
                    public:
                        class Ptr;

                    public:
                        /**
                        */
                        enum ToStringFormat {
                            eAsBytes,
                            eAsString,

                            eDEFAULT = eAsString
                        };

                    public:
                        /**
                        */
                        static Ptr New (const Streams::InputStream<Memory::Byte>::Ptr& src);

                    private:
                        class Rep_;
                    };

                    /**
                     *  Ptr is a copyable smart pointer to a MessageStartTextInputStreamBinaryAdapter.
                     */
                    class MessageStartTextInputStreamBinaryAdapter::Ptr : public Streams::InputStream<Characters::Character>::Ptr {
                    private:
                        using inherited = typename InputStream<Characters::Character>::Ptr;

                    public:
                        Ptr ()           = default;
                        Ptr (const Ptr&) = default;

                    protected:
                        Ptr (const shared_ptr<InputStream<Characters::Character>::_IRep>& from);

                    public:
                        /**
                         *  @see Characters::ToString ()
                         */
                        nonvirtual Characters::String ToString (ToStringFormat format = ToStringFormat::eDEFAULT) const;

                    private:
                        friend class MessageStartTextInputStreamBinaryAdapter;
                    };
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_HTTP_MessageStartTextInputStreamBinaryAdapter_h_*/
