/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"../../Execution/Exceptions.h"
#include	"../../Execution/ErrNoException.h"

#include	"FStreamSupport.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;
using	namespace	Stroika::Foundation::Streams::iostream;






/*
 ********************************************************************************
 ********************** iostream::OpenInputFileStream ***************************
 ********************************************************************************
 */
void	Streams::iostream::OpenInputFileStream (ifstream* ifStream, const TString& fileName, ios_base::openmode _Mode)
{
	RequireNotNull (ifStream);
	ifStream->open (fileName.c_str (), _Mode);
	if (!(*ifStream)) {
		#if		qPlatform_Windows
			Execution::ThrowIfNotERROR_SUCCESS (::GetLastError ());
		#elif	qPlatform_POSIX
			Execution::ThrowIfError_errno_t ();
			AssertNotReached ();// errno sb set
		#else
			AssertNotImplemented ();
		#endif
	}
}

ifstream&	Streams::iostream::OpenInputFileStream (ifstream& ifStream, const TString& fileName, ios_base::openmode _Mode)
{
	OpenInputFileStream (&ifStream, fileName, _Mode);
	return ifStream;
}





/*
 ********************************************************************************
 ****************** StreamUtils::OpenOutputFileStream ***************************
 ********************************************************************************
 */
void	Streams::iostream::OpenOutputFileStream (ofstream* ofStream, const TString& fileName, ios_base::openmode _Mode)
{
	RequireNotNull (ofStream);
	ofStream->open (fileName.c_str (), _Mode);
	if (!(*ofStream)) {
		#if		qPlatform_Windows
			Execution::ThrowIfNotERROR_SUCCESS (::GetLastError ());
		#elif	qPlatform_POSIX
			Execution::ThrowIfError_errno_t ();
			AssertNotReached ();// errno sb set
		#else
			AssertNotImplemented ();
		#endif
	}
}

ofstream&	Streams::iostream::OpenOutputFileStream (ofstream& ofStream, const TString& fileName, ios_base::openmode _Mode)
{
	OpenOutputFileStream (&ofStream, fileName, _Mode);
	return ofStream;
}


