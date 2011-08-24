/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_
#define	_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_	1

#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Configuration/Common.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Characters::TString;

			namespace	Platform {
				namespace	Windows {

					class	HRESULTErrorException {
						public:
							HRESULTErrorException (HRESULT hresult);

							operator HRESULT () const;

						public:
							static	TString	LookupMessage (HRESULT hr);
							nonvirtual	TString	LookupMessage () const;

						private:
							HRESULT	fHResult;
					};

				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"HRESULTErrorException.inl"
