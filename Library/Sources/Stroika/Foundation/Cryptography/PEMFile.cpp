/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "PEMFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ******************************** Cryptography::PEMFile *************************
 ********************************************************************************
 */
Characters::String PEMFile::ToString () const
{
    return TextReader::New (fData).ReadAll ();
}