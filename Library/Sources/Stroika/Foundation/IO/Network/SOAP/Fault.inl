/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::SOAP {
}

/*
 ********************************************************************************
 ******************************** DEPRECATED ************************************
 ********************************************************************************
 *
 *  Kept only so code written for older Stroika versions keeps compiling - each says what replaces it.
 */
namespace Stroika::Foundation::IO::Network::SOAP {
    [[deprecated ("Since Stroika v3.0d25 use Frameworks::WebService::SOAP::DeSerialize (BLOB, Fault*)")]] optional<Fault>
    Deserialize_Fault (const Streams::InputStream::Ptr<byte>& from);
}
