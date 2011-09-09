/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_CopyableCriticalSection_h_
#define	_Stroika_Foundation_Execution_CopyableCriticalSection_h_	1

#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"

#include	"CriticalSection.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			class	CopyableCriticalSection {
				private:
					Memory::SharedPtr<CriticalSection>	fCritSec;
				public:
					CopyableCriticalSection ();

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();
				#if		qUseThreads_WindowsNative
					nonvirtual	operator CRITICAL_SECTION& ();
				#endif
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
