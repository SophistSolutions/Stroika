/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PEMFile.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#endif
#include "Stroika/Foundation/Streams/TextReader.h"

#include "PEMFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::PKI;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ********************** Cryptography::PKI::PEMFile::Ptr *************************
 ********************************************************************************
 */
Characters::String PEMFile::Ptr::ToString () const
{
    return TextReader::New (get ()->GetData ()).ReadAll ();
}

/*
 ********************************************************************************
 **************************** Cryptography::PKI::PEMFile ************************
 ********************************************************************************
 */
auto Cryptography::PKI::PEMFile::New (const Memory::BLOB& pemData) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::OpenSSL::PEMFile::New (pemData);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
auto Cryptography::PKI::PEMFile::New (const Sequence<EntryType>& entries) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::OpenSSL::PEMFile::New (entries);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
