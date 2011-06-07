/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Realtime_h__
#define	__Realtime_h__	1

#include	"../StroikaPreComp.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {


			typedef	double	TickCountType;

			TickCountType	GetTickCount ();

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
