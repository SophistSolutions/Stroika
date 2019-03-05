/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
using namespace Stroika::Foundation::IO;

/*
 * Stuff  INSIDE try section raises exceptions. Catch and rethow SOME binding in a new filename (if none was known).
 * Other exceptions just ignore (so they auto-propagate)
 */
#define CATCH_REBIND_FILENAMES_HELPER_(USEFILENAME)

/*
 ********************************************************************************
 ********************** iostream::OpenInputFileStream ***************************
 ********************************************************************************
 */
ifstream& Streams::iostream::OpenInputFileStream (ifstream* ifStream, const String& fileName, ios_base::openmode mode)
{
    RequireNotNull (ifStream);
    ifStream->exceptions (ifstream::badbit); // so throws on failed open
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
    ofStream->exceptions (ifstream::badbit); // so throws on failed open
    ofStream->open (fileName.AsSDKString ().c_str (), mode | ios_base::out);
    Assert (ofStream->is_open ());
    return *ofStream;
}

ofstream& Streams::iostream::OpenOutputFileStream (ofstream& tmpOfStream, const String& fileName, ios_base::openmode mode)
{
    OpenOutputFileStream (&tmpOfStream, fileName, mode);
    return tmpOfStream;
}
