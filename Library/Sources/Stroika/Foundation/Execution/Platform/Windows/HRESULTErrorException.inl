/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_
#define	_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

		//	class	HRESULTErrorException
			inline	Platform::Windows::HRESULTErrorException::HRESULTErrorException (HRESULT hresult):
				fHResult (hresult)
				{
				}
			inline	Platform::Windows::HRESULTErrorException::operator HRESULT () const
				{
					return fHResult;
				}
			inline	TString	Platform::Windows::HRESULTErrorException::LookupMessage () const
				{
					return LookupMessage (fHResult);
				}

		}
	}
}

#endif	/*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_*/
