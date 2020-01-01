/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif

#include "FStreamSupport.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;

namespace {
    /**
     *  fstream::open () sets failbit on failed open. But most other operations set 'badbit' when things
     *  go wrong (like failed writes).
     */
    constexpr auto kErrBitsToTreatAsExceptions_ = ifstream::badbit | ifstream::failbit;
}

/*
 ********************************************************************************
 ********************** iostream::OpenInputFileStream ***************************
 ********************************************************************************
 */
ifstream& Streams::iostream::OpenInputFileStream (ifstream* ifStream, const String& fileName, ios_base::openmode mode)
{
    RequireNotNull (ifStream);
    ifStream->exceptions (kErrBitsToTreatAsExceptions_); // so throws on failed open
    ifStream->open (fileName.AsSDKString ().c_str (), mode | ios_base::in);
    Assert (ifStream->is_open ());
    return *ifStream;
}

ifstream& Streams::iostream::OpenInputFileStream (ifstream& tmpIFStream, const String& fileName, ios_base::openmode mode)
{
    OpenInputFileStream (&tmpIFStream, fileName, mode);
    return tmpIFStream;
}

/*
 ********************************************************************************
 ****************** StreamUtils::OpenOutputFileStream ***************************
 ********************************************************************************
 */
ofstream& Streams::iostream::OpenOutputFileStream (ofstream* ofStream, const String& fileName, ios_base::openmode mode)
{
    RequireNotNull (ofStream);
    ofStream->exceptions (kErrBitsToTreatAsExceptions_); // so throws on failed open
    ofStream->open (fileName.AsSDKString ().c_str (), mode | ios_base::out);
    Assert (ofStream->is_open ());
    return *ofStream;
}

ofstream& Streams::iostream::OpenOutputFileStream (ofstream& tmpOfStream, const String& fileName, ios_base::openmode mode)
{
    OpenOutputFileStream (&tmpOfStream, fileName, mode);
    return tmpOfStream;
}
