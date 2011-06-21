/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_inl_
#define	_Stroia_Foundation_Execution_Exceptions_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

		//	class	Win32ErrorException
			inline	Win32ErrorException::Win32ErrorException (DWORD error):
				fError (error)
				{
				}
			inline	Win32ErrorException::operator DWORD () const
				{
					return fError;
				}
			inline	TString	Win32ErrorException::LookupMessage () const
				{
					return LookupMessage (fError);
				}


		//	class	Win32StructuredException
			inline	Win32StructuredException::Win32StructuredException (unsigned int seCode):
				fSECode (seCode)
				{
				}
			inline	Win32StructuredException::operator unsigned int () const
				{
					return fSECode;
				}
			inline	TString	Win32StructuredException::LookupMessage () const
				{
					return LookupMessage (fSECode);
				}




		//	class	HRESULTErrorException
			inline	HRESULTErrorException::HRESULTErrorException (HRESULT hresult):
				fHResult (hresult)
				{
				}
			inline	HRESULTErrorException::operator HRESULT () const
				{
					return fHResult;
				}
			inline	TString	HRESULTErrorException::LookupMessage () const
				{
					return LookupMessage (fHResult);
				}


	
		//	class	errno_ErrorException
			inline	errno_ErrorException::operator errno_t () const
				{
					return fError;
				}
			inline	TString	errno_ErrorException::LookupMessage () const
				{
					return LookupMessage (fError);
				}



		//	class	FileBusyException
			inline	FileBusyException::FileBusyException (const TString& fileName):
				fFileName (fileName)
				{
				}

		//	class	FileFormatException
			inline	FileFormatException::FileFormatException (const TString& fileName):
				fFileName (fileName)
				{
				}


			template	<typename T>
				inline	__declspec(noreturn)	void	DoThrow (const T& e2Throw)
					{
						DbgTrace ("Throwing exception: %s", typeid (T).name ());
						throw e2Throw;
					}
			template	<typename T>
				inline	__declspec(noreturn)	void	DoThrow (const T& e2Throw, const char* traceMsg)
					{
						DbgTrace ("%s", traceMsg);
						throw e2Throw;
					}
			template	<typename T>
				inline	__declspec(noreturn)	void	DoThrow (const T& e2Throw, const wchar_t* traceMsg)
					{
						DbgTrace (L"%s", traceMsg);
						throw e2Throw;
					}
			inline	__declspec(noreturn)	void	DoReThrow ()
				{
					DbgTrace ("DoReThrow");
					throw;
				}
			inline	__declspec(noreturn)	void	DoReThrow (const char* traceMsg)
				{
					DbgTrace ("DoReThrow: %s", traceMsg);
					throw;
				}
			inline	__declspec(noreturn)	void	DoReThrow (const wchar_t* traceMsg)
				{
					DbgTrace (L"DoReThrow: %s", traceMsg);
					throw;
				}


			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const Win32ErrorException& e2Throw)
					{
						DbgTrace ("Throwing Win32ErrorException: DWORD = 0x%x", static_cast<DWORD> (e2Throw));
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const StringException& e2Throw)
					{
						DbgTrace (L"Throwing StringException: '%s'", static_cast<wstring> (e2Throw).substr (0, 20).c_str ());
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const Win32StructuredException& e2Throw)
					{
						DbgTrace ("Throwing Win32StructuredException: fSECode = 0x%x", static_cast<int> (e2Throw));
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const HRESULTErrorException& e2Throw)
					{
						DbgTrace ("Throwing HRESULTErrorException: HRESULT = 0x%x", static_cast<HRESULT> (e2Throw));
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const FileFormatException& e2Throw)
					{
						DbgTrace (_T ("Throwing FileFormatException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const SilentException& e2Throw)
					{
						DbgTrace (_T ("Throwing SilentException"));
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const UserCanceledException& e2Throw)
					{
						DbgTrace (_T ("Throwing UserCanceledException"));
						throw e2Throw;
					}
			template	<>
				inline	__declspec(noreturn)	void	DoThrow (const FileBusyException& e2Throw)
					{
						DbgTrace (_T ("Throwing FileBusyException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}



			inline	void	ThrowIfFalseGetLastError (bool test)
				{
					if (not test) {
						Win32ErrorException::DoThrow (::GetLastError ());
					}
				}
			inline	void	ThrowIfFalseGetLastError (BOOL test)
				{
					if (not test) {
						Win32ErrorException::DoThrow (::GetLastError ());
					}
				}
			inline	void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
				{
					if (win32ErrCode != ERROR_SUCCESS) {
						Win32ErrorException::DoThrow (win32ErrCode);
					}
				}
			inline	void	ThrowIfErrorHRESULT (HRESULT hr)
				{
					if (not SUCCEEDED (hr)) {
						DoThrow (HRESULTErrorException (hr));
					}
				}
			inline	void	ThrowIfError_errno_t (errno_t e)
				{
					if (e != 0) {
						errno_ErrorException::DoThrow (e);
					}
				}
			inline	void	ThrowIfNull (const void* p)
				{
					if (p == NULL) {
						DoThrow (bad_alloc (), _T ("ThrowIfNull (NULL) - throwing bad_alloc"));
					}
				}
			template	<typename E>
				inline	void	ThrowIfNull (const void* p, const E& e)
					{
						if (p == NULL) {
							DoThrow (e, _T ("ThrowIfNull (NULL,X) - throwing X"));
						}
					}
			template<>
				inline	void	ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
					{
						ThrowIfNull (p, HRESULTErrorException (hr));
					}



		}
	}
}

#endif	/*_Stroia_Foundation_Execution_Exceptions_inl_*/
