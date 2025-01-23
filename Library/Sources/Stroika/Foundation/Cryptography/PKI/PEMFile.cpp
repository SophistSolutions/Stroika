/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PEMFile.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Streams/ToText.h"

#include "PEMFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::PKI;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ****************************** PKI::PEMFile::Ptr *******************************
 ********************************************************************************
 */
Characters::String PEMFile::Ptr::ToString () const
{
    return ToText::Reader::New (get ()->GetData ()).ReadAll ();
}

/*
 ********************************************************************************
 ********************************* PKI::PEMFile *********************************
 ********************************************************************************
 */
auto PKI::PEMFile::New (const filesystem::path& pemFile) -> Ptr
{
    return New (IO::FileSystem::FileInputStream::New (pemFile).ReadAll ());
}
auto PKI::PEMFile::New (const Memory::BLOB& pemData) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::Providers::OpenSSL::PEMFile::New (pemData);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
auto PKI::PEMFile::New (const Sequence<EntryType>& entries) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::Providers::OpenSSL::PEMFile::New (entries);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
