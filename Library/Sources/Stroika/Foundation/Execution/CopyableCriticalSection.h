/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_CopyableCriticalSection_h_
#define	_Stroika_Foundation_Execution_CopyableCriticalSection_h_	1

#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Configuration/Common.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			class	CopyableCriticalSection {
				private:
					Memory::RefCntPtr<CriticalSection>	fCritSec;
				public:
					CopyableCriticalSection ();

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();
					nonvirtual	operator CRITICAL_SECTION& ();
			};


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_CopyableCriticalSection_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CopyableCriticalSection.inl"
