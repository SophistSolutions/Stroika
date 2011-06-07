/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"WaitAbandonedException.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




/*
 ********************************************************************************
 ************************ WaitAbandonedException ********************************
 ********************************************************************************
 */
WaitAbandonedException::WaitAbandonedException ()
	: StringException (L"WAIT on object abandoned because the owning thread terminated")
{
}
