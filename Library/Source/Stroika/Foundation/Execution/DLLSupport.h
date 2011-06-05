/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_DLLSupport_h
#define	_DLLSupport_h	1

#include	"../StroikaPreComp.h"

#if		defined(_WIN32)
	#include	<tchar.h>
	#include	<Windows.h>
#endif

#include	"../StringUtils.h"


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
#endif	/*_DLLSupport_h*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"DLLSupport.inl"

