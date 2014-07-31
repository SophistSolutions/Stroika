/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */

#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../Characters/CString/Utilities.h"
#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Memory/BLOB.h"

#include    "Digest/Algorithm/MD5.h"
#include    "Hash/Adapter.h"

#include    "MD5.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Memory;


string  Cryptography::ComputeMD5Digest (const Byte* s, const Byte* e)
{
    Require (s == e or s != nullptr);
    Require (s == e or e != nullptr);
    using   USE_HASHER_     =   Digest::Digester<Digest::Result128BitType, Digest::Algorithm::MD5>;
    return Hash::Adapter<USE_HASHER_, Memory::BLOB, string> (Memory::BLOB (s, e));
}

string  Cryptography::ComputeMD5Digest (const vector<Byte>& b)
{
    return ComputeMD5Digest (Containers::Start (b), Containers::End (b));
}

