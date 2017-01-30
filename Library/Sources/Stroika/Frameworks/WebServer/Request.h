/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../Foundation/IO/Network/SocketAddress.h"
#include "../../Foundation/IO/Network/URL.h"
#include "../../Foundation/Memory/Optional.h"
#include "../../Foundation/Streams/InputStream.h"

/*
 * TODO:
 *      @todo   Redo fHeaders as Stroika Association (not Mapping, cuz things like Set-Cookie headers can appear more than once).
 *
 *      @todo   Think out if these should be Copy By Value or reference, and about thread safety. For now avoid by saying not
 *              copyable, but still mus tthink out thread safety
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;
            using Characters::String;
            using Containers::Mapping;
            using DataExchange::InternetMediaType;

            /**
             *  Maybe associated TextStream, and maybe readline method goes here
             *  Quicky impl. Need to improve this significantly. Probably/possibly hide the fInptuStream and other public
             *
             *  For now assume externally sycnhonized
             */
            class Request : private Debug::AssertExternallySynchronizedLock {
            public:
                Request ()               = delete;
                Request (const Request&) = delete;
                Request (Request&&)      = default;
                Request (const Streams::InputStream<Memory::Byte>& inStream);

            public:
                nonvirtual const Request& operator= (const Request&) = delete;

            public:
                // Quicky impl. Need to improve this significantly.
                // Can call multiple times - but first time it blcoks fetching data
                nonvirtual Memory::BLOB GetBody ();

            public:
                /**
                 */
                nonvirtual String GetHTTPVersion () const;

            public:
                /**
                 */
                nonvirtual String GetHTTPMethod () const;

            public:
                /**
                 */
                nonvirtual void SetHTTPMethod (const String& method);

            public:
                /**
                 */
                nonvirtual IO::Network::URL GetURL () const;

            public:
                /**
                 */
                nonvirtual void SetURL (const IO::Network::URL& url);

            public:
                /**
                 */
                nonvirtual Mapping<String, String> GetHeaders () const;

            public:
                /**
                 *  Return the unsigned integer value of the Content-Length header.
                 *
                 *  \note - this does not imply having read the body, and nor is it updated to reflect the body size read after its been read.
                 */
                nonvirtual Memory::Optional<uint64_t> GetContentLength () const;

            public:
                /**
                 */
                nonvirtual void SetHeaders (const Mapping<String, String>& headers);

            public:
                /**
                 */
                nonvirtual void AddHeader (const String& headerName, const String& value);

            public:
                /**
                 *  @todo unclear if this SB const?
                 */
                nonvirtual Streams::InputStream<Memory::Byte> GetInputStream ();

            private:
                // SOON TO BE PRIVATE
                Streams::InputStream<Memory::Byte> fInputStream_;

            private:
                String           fHTTPVersion_;
                String           fMethod_;
                IO::Network::URL fURL_;
                Mapping<String, String> fHeaders_;

            public:
                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;

            private:
                Memory::Optional<Memory::BLOB> fBody_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Request.inl"

#endif /*_Stroika_Framework_WebServer_Request_h_*/
