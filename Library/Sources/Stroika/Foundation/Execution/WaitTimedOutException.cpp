/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"WaitTimedOutException.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



/*
 ********************************************************************************
 ***************************** WaitTimedOutException ****************************
 ********************************************************************************
 */
WaitTimedOutException::WaitTimedOutException ()
	: StringException (L"WAIT timed out")
{
}

