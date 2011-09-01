/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_CriticalSection_h_
#define	_Stroika_Foundation_Execution_CriticalSection_h_	1

#include	"../StroikaPreComp.h"

#define	qUseWindowsNativeThreadSupport	qPlatform_Windows

#if		qUseWindowsNativeThreadSupport
	#include	<windows.h>
//#else
//	typedef	int	CRITICAL_SECTION;
#endif

#include	"../Configuration/Common.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			class	CriticalSection {
				public:
					CriticalSection () throw ();
					~CriticalSection ();

				private:
					CriticalSection (const CriticalSection&);					// not allowed
					const CriticalSection& operator= (const CriticalSection&);	// not allowed

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();

				#if		qUseWindowsNativeThreadSupport
					operator CRITICAL_SECTION& ();

				private:
					CRITICAL_SECTION fCritSec;
				#endif
			};




			// enter  in CTOR and LEAVE in DTOR
			template	<typename LOCKTYPE>
				class	AutoCriticalSectionT {
					public:
						explicit AutoCriticalSectionT (LOCKTYPE& critSec);
						~AutoCriticalSectionT ();

					private:
						LOCKTYPE&	fCritSec;
				};
			typedef	AutoCriticalSectionT<CriticalSection>	AutoCriticalSection;


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_CriticalSection_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CriticalSection.inl"
