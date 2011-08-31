/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"Event.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




/*
 ********************************************************************************
 ************************************** Event ***********************************
 ********************************************************************************
 */
#if		qTrack_ThreadUtils_HandleCounts
uint32_t	Event::sCurAllocatedHandleCount		=	0;
#endif
