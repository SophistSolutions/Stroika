/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"BadFormatException.h"




using	namespace	Stroika::Foundation;


/*
 ********************************************************************************
 ****************** DataExchangeFormat::BadFormatException **********************
 ********************************************************************************
 */
namespace	{
	wstring	mkMessage_ ()
		{
			return L"Badly formatted input";
		}
	wstring	mkMessage_ (const wstring& details)
		{
			return details.empty ()? mkMessage_ (): details;
		}
}
DataExchangeFormat::BadFormatException::BadFormatException ()
	: StringException (mkMessage_ ())
	, fDetails_ ()
	, fLineNumber ()
	, fColumnNumber ()
	, fFileOffset ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const wstring& details)
	: StringException (mkMessage_ (details))
	, fDetails_ (details)
	, fLineNumber ()
	, fColumnNumber ()
	, fFileOffset ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const wstring& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
	: StringException (mkMessage_ (details))
	, fDetails_ (details)
	, fLineNumber (lineNumber)
	, fColumnNumber (columnNumber)
	, fFileOffset (fileOffset)
{
}

void	DataExchangeFormat::BadFormatException::GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const
{
	if (lineNum != nullptr) {
		*lineNum = fLineNumber;
	}
	if (colNumber != nullptr) {
		*colNumber = fColumnNumber;
	}
	if (fileOffset != nullptr) {
		*fileOffset = fFileOffset;
	}
}


