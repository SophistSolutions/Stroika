/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../IO/FileBusyException.h"

#include "FStreamSupport.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;
using namespace Stroika::Foundation::IO;

/*
 * Stuff  INSIDE try section raises exceptions. Catch and rethow SOME binding in a new filename (if none was known).
 * Other exceptions just ignore (so they auto-propagate)
 */
#define CATCH_REBIND_FILENAMES_HELPER_(USEFILENAME)             \
    catch (const FileBusyException& e)                          \
    {                                                           \
        if (e.GetFileName ().empty ()) {                        \
            Execution::Throw (FileBusyException (USEFILENAME)); \
        }                                                       \
        Execution::ReThrow ();                                  \
    }

/*
 ********************************************************************************
 ********************** iostream::OpenInputFileStream ***************************
 ********************************************************************************
 */
ifstream& Streams::iostream::OpenInputFileStream (ifstream* ifStream, const String& fileName, ios_base::openmode mode)
{
    RequireNotNull (ifStream);
    try {
        ifStream->open (fileName.AsSDKString ().c_str (), mode | ios_base::in);
        if (!(*ifStream)) {
#if qPlatform_Windows
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::GetLastError ());
#elif qPlatform_POSIX
            Execution::ThrowPOSIXErrNo ();
            AssertNotReached (); // errno sb set but not documented so not sure how best to handle?
#else
            AssertNotImplemented ();
#endif
        }
        return *ifStream;
    }
    CATCH_REBIND_FILENAMES_HELPER_ (fileName);
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
    try {
        ofStream->open (fileName.AsSDKString ().c_str (), mode | ios_base::out);
        if (!(*ofStream)) {
#if qPlatform_Windows
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::GetLastError ());
#elif qPlatform_POSIX
            Execution::ThrowPOSIXErrNo ();
            AssertNotReached (); // errno sb set, but not documented to be guaranteed so not sure how best to handle this?
#else
            AssertNotImplemented ();
#endif
        }
        return *ofStream;
    }
    CATCH_REBIND_FILENAMES_HELPER_ (fileName);
}

ofstream& Streams::iostream::OpenOutputFileStream (ofstream& tmpOfStream, const String& fileName, ios_base::openmode mode)
{
    OpenOutputFileStream (&tmpOfStream, fileName, mode);
    return tmpOfStream;
}
