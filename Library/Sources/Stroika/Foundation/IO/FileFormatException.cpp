/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Characters/StringUtils.h"
#include	"../Characters/Format.h"

#include	"FileFormatException.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;



/*
 ********************************************************************************
 **************************** IO::FileFormatException ***************************
 ********************************************************************************
 */
namespace	{
	wstring	mkMessage_ (const TString& fileName)
		{
			if (fileName.empty ()) {
				return Characters::Format (L"Invalid file format");
			}
			else {
				return Characters::Format (L"Invalid file format: '%.200s'", Characters::LimitLength (Characters::TString2Wide (fileName), 100, false).c_str ());
			}
		}
}

FileFormatException::FileFormatException (const TString& fileName)
	: StringException (mkMessage_ (fileName))
	, fFileName_ (fileName)
{
}


