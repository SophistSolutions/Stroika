/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_DLLSupport_h
#define	_DLLSupport_h	1

#include	"StroikaPreComp.h"

#include	<tchar.h>
#include	<Windows.h>

#include	<string>
#include	<vector>

#include	"Debug/Assertions.h"
#include	"StringUtils.h"


namespace	Stroika {	
	namespace	Foundation {



namespace	DLLSupport {


		using	StringUtils::tstring;

	class	DLLLoader {
		public:
			DLLLoader (const TCHAR* dllName);
			DLLLoader (const TCHAR* dllName, const vector<tstring>& searchPath);
			~DLLLoader ();

		public:
			operator HMODULE ();

		public:
			nonvirtual	FARPROC	GetProcAddress (__in LPCSTR lpProcName) const;

		private:
			HMODULE	fModule;
	};
}

	}
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
	//	class	DLLSupport::DLLLoader
		inline	DLLSupport::DLLLoader::operator HMODULE ()
			{
				EnsureNotNil (fModule);
				return fModule;
			}
		inline	FARPROC	DLLSupport::DLLLoader::GetProcAddress (__in LPCSTR lpProcName) const
			{
				AssertNotNil (fModule);
				RequireNotNil (lpProcName);
				return ::GetProcAddress (fModule, lpProcName);
			}
	}
}

#endif	/*_DLLSupport_h*/
