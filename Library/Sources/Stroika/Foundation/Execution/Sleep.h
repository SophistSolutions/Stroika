/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Sleep_h_
#define	_Stroika_Foundation_Execution_Sleep_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Time/RealTime.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			// MAIN reason to use this - is it sets the 'alertable' flag on the sleep, so the QueueUserAPC () stuff works!
			// which allows SimpleThread::Stop () to work properly...
			// -- LGP 2009-04-28
			void	Sleep (Time::DurationSecondsType seconds2Wait);

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Sleep_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Sleep.inl"
