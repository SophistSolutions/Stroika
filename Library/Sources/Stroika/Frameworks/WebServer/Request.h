/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Containers/Mapping.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"
#include    "../../Foundation/Memory/Optional.h"
#include    "../../Foundation/IO/Network/SocketAddress.h"
#include    "../../Foundation/IO/Network/URL.h"
#include    "../../Foundation/Streams/InputStream.h"


/*
 * TODO:
 *      @todo   Redo fHeaders as Stroika Association (not Mapping, cuz things like Set-Cookie headers can appear more than once).
 *
 *      @todo   Think out if these should be Copy By Value or reference, and about thread safety. For now avoid by saying not
 *              copyable, but still mus tthink out thread safety
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Mapping;
            using   DataExchange::InternetMediaType;


            /**
             *  Maybe associated TextStream, and maybe readline method goes here
             *  Quicky impl. Need to improve this significantly. Probably/possibly hide the fInptuStream and other public
             *
             *  For now assume externally sycnhonized
             */
            class  Request : private Debug::AssertExternallySynchronizedLock {
            public:
                Request () = delete;
                Request (const Request&) = delete;
                Request (Request&&) = default;
                Request (const Streams::InputStream<Memory::Byte>& inStream, const Memory::Optional<IO::Network::SocketAddress>& peerAddress = Memory::Optional<IO::Network::SocketAddress> {});

            public:
                nonvirtual  const Request& operator= (const Request&) = delete;

            public:
                // Quicky impl. Need to improve this significantly.
                // Can call multiple times - but first time it blcoks fetching data
                nonvirtual  Memory::BLOB    GetBody ();

            public:
                nonvirtual  String  GetHTTPVersion () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                    return fHTTPVersion;
                }

            public:
                nonvirtual  String  GetHTTPMethod () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                    return fMethod;
                }

            public:
                nonvirtual  IO::Network::URL    GetURL () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                    return fURL;
                }

            public:
                Mapping<String, String> GetHeaders () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec { *this };

                    return fHeaders;
                }

            public:
                // unclear if this SB const?
                Streams::InputStream<Memory::Byte>  GetInputStream ()
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };

                    return fInputStream;
                }

            public:
                // SOON TO BE PRIVATE
                Streams::InputStream<Memory::Byte>      fInputStream;

            public:
                // SOON TO BE PRIVATE
                String                                  fHTTPVersion;
                // SOON TO BE PRIVATE
                String                                  fMethod;
                // SOON TO BE PRIVATE
                IO::Network::URL                        fURL;
                // SOON TO BE PRIVATE
                Mapping<String, String>                 fHeaders;

            public:
                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual  String  ToString () const;

            private:
                Memory::Optional<Memory::BLOB>          fBody_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Request.inl"

#endif  /*_Stroika_Framework_WebServer_Request_h_*/
