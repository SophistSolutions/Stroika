/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Realtime_h__
#define	__Realtime_h__	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<climits>

#if		defined(_WIN32)
#include	<Windows.h>
#endif

#include	"../Characters/StringUtils.h"
#include	"../Configuration/Basics.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {



	float	GetTickCount ();

		}
	}
}





#endif	/*__Realtime_h__*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Realtime.inl"
