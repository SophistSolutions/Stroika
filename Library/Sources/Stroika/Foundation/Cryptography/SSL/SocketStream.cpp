/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/OpenSSL/SocketStream.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "SocketStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ************************* Cryptography::SSL::SocketStream **********************
 ********************************************************************************
 */
auto Cryptography::SSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ClientContext::Options& o) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return OpenSSL::SocketStream::New (sd, OpenSSL::ClientContext::Options{o});
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
auto Cryptography::SSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ServerContext::Options& o) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return OpenSSL::SocketStream::New (sd, OpenSSL::ServerContext::Options{o});
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
