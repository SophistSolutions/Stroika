/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"OperationNotSupportedException.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;





/*
 ********************************************************************************
 ************************ OperationNotSupportedException ************************
 ********************************************************************************
 */
OperationNotSupportedException::OperationNotSupportedException ()
	: StringException (L"Operation Not Supported")
	, fOperationName_ ()
{
}

OperationNotSupportedException::OperationNotSupportedException (const String& operationName)
	: StringException (L"Operation '" + operationName.As<wstring> () + L"' Not Supported")
	, fOperationName_ (operationName)
{
}

Characters::String	OperationNotSupportedException::GetOperationName () const
{
	return fOperationName_;
}
