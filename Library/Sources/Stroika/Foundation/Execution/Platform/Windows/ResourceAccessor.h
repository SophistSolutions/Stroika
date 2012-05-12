/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_Resources_h_
#define	_Stroika_Foundation_Execution_Platform_Windows_Resources_h_	1

#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	<vector>

#include	"../../../Configuration/Common.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	Platform {
				namespace	Windows {

				using	namespace	Stroika::Foundation::Configuration;

				class	ResourceAccessor {
					public:
						ResourceAccessor (HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);

					public:
						nonvirtual	bool			GetFound () const;
						nonvirtual	const Byte*		GetDataStart () const;
						nonvirtual	const Byte*		GetDataEnd () const;
						nonvirtual	vector<Byte>	GetData () const;

					private:
						const Byte*	fDataStart;
						const Byte*	fDataEnd;
				};

				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Platform_Windows_Resources_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ResourceAccessor.inl"
