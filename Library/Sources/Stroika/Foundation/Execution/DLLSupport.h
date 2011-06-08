/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_DLLSupport_h_
#define	_Stroia_Foundation_Execution_DLLSupport_h_	1

#include	"../StroikaPreComp.h"

#if		defined(_WIN32)
	#include	<tchar.h>
	#include	<Windows.h>
#endif

#include	"../Characters/StringUtils.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Characters::tstring;

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
#endif	/*_Stroia_Foundation_Execution_DLLSupport_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"DLLSupport.inl"

