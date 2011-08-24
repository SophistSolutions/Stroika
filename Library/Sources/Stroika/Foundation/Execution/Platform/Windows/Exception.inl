/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_
#define	_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	Platform {
				namespace	Windows {

				//	class	Exception
					inline	Exception::Exception (DWORD error)
						: fError (error)
						{
						}
					inline	Exception::operator DWORD () const
						{
							return fError;
						}
					inline	TString	Exception::LookupMessage () const
						{
							return LookupMessage (fError);
						}

				}
			}
		}
	}
}

#endif	/*_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_*/
