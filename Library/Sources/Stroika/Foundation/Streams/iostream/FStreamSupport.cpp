/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"../../Execution/Exceptions.h"
#include	"../../Execution/ErrNoException.h"
#include	"../../IO/FileAccessException.h"
#include	"../../IO/FileBusyException.h"
#include	"../../IO/FileFormatException.h"

#include	"FStreamSupport.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;
using	namespace	Stroika::Foundation::Streams::iostream;
using	namespace	Stroika::Foundation::IO;




/*
 * Stuff  INSIDE try section raises exceptions. Catch and rethow SOME binding in a new filename (if none was known).
 * Otehr exceptions just ignore (so they auto-propagate)
 */
#define		CATCH_REBIND_FILENAMES_HELPER_(USEFILENAME)	\
	catch (const FileBusyException& e) {	\
		if (e.fFileName.empty ()) {\
			Execution::DoThrow (FileBusyException (USEFILENAME));\
		}\
		Execution::DoReThrow ();\
	}\
	catch (const FileAccessException& e) {	\
		if (e.fFileName.empty ()) {\
			Execution::DoThrow (FileAccessException (USEFILENAME, e.fFileAccessMode));\
		}\
		Execution::DoReThrow ();\
	}\
	catch (const FileFormatException& e) {	\
		if (e.fFileName.empty ()) {\
			Execution::DoThrow (FileFormatException (USEFILENAME));\
		}\
		Execution::DoReThrow ();\
	}\





/*
 ********************************************************************************
 ********************** iostream::OpenInputFileStream ***************************
 ********************************************************************************
 */
void	Streams::iostream::OpenInputFileStream (ifstream* ifStream, const TString& fileName, ios_base::openmode _Mode)
{
	RequireNotNull (ifStream);
	try {
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
	CATCH_REBIND_FILENAMES_HELPER_(fileName);
}

ifstream&	Streams::iostream::OpenInputFileStream (ifstream& tmpIFStream,
								const TString& fileName,
								ios_base::openmode _Mode
					)
{
	OpenInputFileStream (&tmpIFStream, fileName, _Mode);
	return tmpIFStream;
}






/*
 ********************************************************************************
 ****************** StreamUtils::OpenOutputFileStream ***************************
 ********************************************************************************
 */
void	Streams::iostream::OpenOutputFileStream (ofstream* ofStream, const TString& fileName, ios_base::openmode _Mode)
{
	RequireNotNull (ofStream);
	try {
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
	CATCH_REBIND_FILENAMES_HELPER_(fileName);
}

ofstream&	Streams::iostream::OpenOutputFileStream (ofstream& tmpOfStream,
							const TString& fileName,
							ios_base::openmode _Mode
				)
{
	OpenOutputFileStream (&tmpOfStream, fileName, _Mode);
	return tmpOfStream;
}


