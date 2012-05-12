/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Characters/StringUtils.h"
#include	"../Characters/Format.h"

#include	"FileAccessException.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;



/*
 ********************************************************************************
 **************************** IO::FileAccessException ***************************
 ********************************************************************************
 */
namespace	{
	wstring	mkMessage_ (const TString& fileName, FileAccessMode fileAccessMode)
		{
			wstring	message;
			if ((fileAccessMode & eRead_FAM) and (fileAccessMode & eWrite_FAM)) {
				message = L"Cannot read from or write to file";
			}
			else if (fileAccessMode & eRead_FAM) {
				message = L"Cannot read from file";
			}
			else if (fileAccessMode & eWrite_FAM) {
				message = L"Cannot write to file";
			}
			else {
				message = L"Access failure for file";
			}
			if (not fileName.empty ()) {
				message = Characters::Format (L"%s: '%.200s'", message.c_str (), Characters::LimitLength (Characters::TString2Wide (fileName), 100, false).c_str ());
			}
			return message;
		}
}
FileAccessException::FileAccessException (const TString& fileName, FileAccessMode fileAccessMode)
	: StringException (mkMessage_ (fileName, fileAccessMode))
	, fFileName_ (fileName)
	, fFileAccessMode_ (fileAccessMode)
{
}

