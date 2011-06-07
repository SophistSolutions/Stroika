/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_DLLSupport_inl
#define	_Stroia_Foundation_Execution_DLLSupport_inl	1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

	//	class	DLLLoader
		inline	DLLLoader::operator HMODULE ()
			{
				EnsureNotNil (fModule);
				return fModule;
			}
		inline	FARPROC	DLLLoader::GetProcAddress (__in LPCSTR lpProcName) const
			{
				AssertNotNil (fModule);
				RequireNotNil (lpProcName);
				return ::GetProcAddress (fModule, lpProcName);
			}

		}
	}
}
#endif	/*_Stroia_Foundation_Execution_DLLSupport_inl*/
