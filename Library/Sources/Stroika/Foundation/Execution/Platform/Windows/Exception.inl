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

					inline	void	ThrowIfFalseGetLastError (bool test)
						{
							if (not test) {
								Exception::DoThrow (::GetLastError ());
							}
						}
					inline	void	ThrowIfFalseGetLastError (BOOL test)
						{
							if (not test) {
								Exception::DoThrow (::GetLastError ());
							}
						}
					inline	void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
						{
							if (win32ErrCode != ERROR_SUCCESS) {
								Exception::DoThrow (win32ErrCode);
							}
						}
				}
			}


			// Re-declare so we can specialize (real declaration is in Execution/Excpetions.h)
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<>
				inline	void	_NoReturn_	DoThrow (const Platform::Windows::Exception& e2Throw)
					{
						// Go directly through class DoThrow() since that may remap to differnt kinds of exceptions, and already has trace messages
						Platform::Windows::Exception::DoThrow (e2Throw);
					}
		}
	}
}

#endif	/*_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_*/
