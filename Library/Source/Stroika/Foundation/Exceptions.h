/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Exceptions_h__
#define	__Exceptions_h__	1

#include	"StroikaPreComp.h"

#include	<exception>
#include	<string>
#include	<Windows.h>

#include	"StringUtils.h"
#include	"Debug/Trace.h"
#include	"Support.h"



namespace	Stroika {	
	namespace	Foundation {


namespace	Exceptions {


	using	StringUtils::tstring;


	// Throw this when an error has already been reported - so that it isn't reported again
	class	SilentException {
	};

	// mostly treat the same as SilentException
	class	UserCanceledException : public SilentException {
	};


	class	Win32ErrorException {
		public:
			Win32ErrorException (DWORD error);

			operator DWORD () const;

		public:
			// throw Win32ErrorException () - if error is a real error, and map SOME (like #8 to bad_alloc) - but ALWAYS throw
			// someting, regardless of error#
			static	void	DoThrow (DWORD error);

		public:
			static	tstring	LookupMessage (DWORD hr);
			nonvirtual	tstring	LookupMessage () const;

		private:
			DWORD	fError;

		public:
			static	const	DWORD	kERROR_INTERNET_TIMEOUT						=	12002;
			static	const	DWORD	kERROR_INTERNET_INVALID_URL					=	12005;
			static	const	DWORD	kERROR_INTERNET_UNRECOGNIZED_SCHEME			=	12006;
			static	const	DWORD	kERROR_INTERNET_NAME_NOT_RESOLVED			=	12007;
			static	const	DWORD	kERROR_INTERNET_PROTOCOL_NOT_FOUND			=	12008;
			static	const	DWORD	kERROR_INTERNET_CANNOT_CONNECT				=	12029;
	};




	class	Win32StructuredException {
		private:
			unsigned int fSECode;

		public:
			explicit Win32StructuredException (unsigned int n);
			operator unsigned int () const;

		public:
			static	tstring	LookupMessage (unsigned int n);
			nonvirtual	tstring	LookupMessage () const;

		public:
			static	void	RegisterHandler ();
		private:
			static	void	trans_func (unsigned int u, EXCEPTION_POINTERS* pExp);
	};


	class	HRESULTErrorException {
		public:
			HRESULTErrorException (HRESULT hresult);

			operator HRESULT () const;

		public:
			static	tstring	LookupMessage (HRESULT hr);
			nonvirtual	tstring	LookupMessage () const;

		private:
			HRESULT	fHResult;
	};


	class	StringException {
		public:
			StringException (const wstring& reasonForError);

			operator wstring () const;

		private:
			wstring	fError;
	};

	class	errno_ErrorException : public StringException {
		public:
			errno_ErrorException (errno_t e);

			operator errno_t () const;

		public:
			static	tstring	LookupMessage (errno_t e);
			nonvirtual	tstring	LookupMessage () const;

		public:
			// throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
			// as good a fit.
			static	void	DoThrow (DWORD error);

		private:
			errno_t	fError;
	};



	class	RequiredComponentMissingException : public StringException {
		public:
			static	const	wchar_t	kJava[];
			static	const	wchar_t	kPDFViewer[];
			static	const	wchar_t	kPDFOCXViewer[];
		public:
			RequiredComponentMissingException (const wstring& component);

		public:
			wstring	fComponent;
	};

	class	RequiredComponentVersionMismatchException : public StringException {
		public:
			RequiredComponentVersionMismatchException (const wstring& component, const wstring& requiredVersion = wstring ());
	};

	class	FeatureNotSupportedInThisVersionException : public StringException {
		public:
			FeatureNotSupportedInThisVersionException (const wstring& feature);
		public:
			wstring	fFeature;
	};

	class	FileBusyException {
		public:
			FileBusyException (const tstring& fileName = tstring ());

		public:
			tstring	fFileName;
	};

	class	FileFormatException {
		public:
			FileFormatException (const tstring& fileName);

		public:
			tstring	fFileName;
	};



	// Utility to call a Trace message (hopefully an appropriate one) for an exception being thrown...
	template	<typename T>
		__declspec(noreturn)	void	DoThrow (const T& e2Throw);
	template	<typename T>
		__declspec(noreturn)	void	DoThrow (const T& e2Throw, const char* traceMsg);
	template	<typename T>
		__declspec(noreturn)	void	DoThrow (const T& e2Throw, const wchar_t* traceMsg);

	// Just a regular C++ rethrow, but with a DbgTrace message...
	__declspec(noreturn)	void	DoReThrow ();
	__declspec(noreturn)	void	DoReThrow (const char* traceMsg);
	__declspec(noreturn)	void	DoReThrow (const wchar_t* traceMsg);



	void	ThrowIfFalseGetLastError (bool test);
	void	ThrowIfFalseGetLastError (BOOL test);
	void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
	void	ThrowIfErrorHRESULT (HRESULT hr);
	void	ThrowIfError_errno_t (errno_t e = errno);
	void	ThrowIfShellExecError (HINSTANCE r);

	template	<typename E>
		void	ThrowIfNull (const void* p, const E& e = E ());
	template<>
		void	ThrowIfNull (const void* p, const HRESULT& hr);
	void	ThrowIfNull (const void* p);


	#define	IgnoreExceptionsForCall(theCode)		try {theCode;} catch (...) {}


	#define	CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION()\
		catch (HRESULT hr) {\
			return hr;\
		}\
		catch (const Win32ErrorException& we) {\
			return (HRESULT_FROM_WIN32 (we));\
		}\
		catch (const HRESULTErrorException& h) {\
			return static_cast<HRESULT> (h);\
		}\
		catch (const bad_alloc&) {\
			return E_OUTOFMEMORY;\
		}\
		catch (...) {\
			return DISP_E_EXCEPTION;\
		}\



	/*
	 *	Utility class to support catch and re-throw of exceptions.
	 */
	struct	CatchAndCaptureExceptionHelper {
		public:
			CatchAndCaptureExceptionHelper ();

		public:
			// This function need not be run, but CAN be run to allow subclasses to specify additional 
			struct	Callback {
				virtual	void	DoItInsideCatcher () = 0;
			};
			virtual	void	DoRunWithCatchRePropagate (Callback* callback);

		public:
			virtual	bool	AnyExceptionCaught () const;
			virtual	void	RethrowIfAnyCaught () const;

		public:
			auto_ptr<HRESULTErrorException>					fHRESULTErrorException;
			auto_ptr<Win32ErrorException>					fWin32ErrorException;
			auto_ptr<RequiredComponentMissingException>		fRequiredComponentMissingException;
			auto_ptr<StringException>						fStringException;
			auto_ptr<FileFormatException>					fFileFormatException;
			auto_ptr<FileBusyException>						fFileBusyException;
			auto_ptr<SilentException>						fSilentException;
	};


	#define	CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
		catch (const Exceptions::RequiredComponentMissingException& e) {\
			(CE).fRequiredComponentMissingException = auto_ptr<Exceptions::RequiredComponentMissingException> (DEBUG_NEW Exceptions::RequiredComponentMissingException (e));\
		}\
		catch (const Exceptions::StringException& e) {\
			(CE).fStringException = auto_ptr<Exceptions::StringException> (DEBUG_NEW Exceptions::StringException (e));\
		}\
		catch (const Exceptions::HRESULTErrorException& e) {\
			(CE).fHRESULTErrorException = auto_ptr<Exceptions::HRESULTErrorException> (DEBUG_NEW Exceptions::HRESULTErrorException (e));\
		}\
		catch (const Exceptions::Win32ErrorException& e) {\
			(CE).fWin32ErrorException = auto_ptr<Exceptions::Win32ErrorException> (DEBUG_NEW Exceptions::Win32ErrorException (e));\
		}\
		catch (const Exceptions::FileFormatException& e) {\
			(CE).fFileFormatException = auto_ptr<Exceptions::FileFormatException> (DEBUG_NEW Exceptions::FileFormatException (e));\
		}\
		catch (const Exceptions::FileBusyException& e) {\
			(CE).fFileBusyException = auto_ptr<Exceptions::FileBusyException> (DEBUG_NEW Exceptions::FileBusyException (e));\
		}\
		catch (const Exceptions::SilentException& e) {\
			(CE).fSilentException = auto_ptr<Exceptions::SilentException> (DEBUG_NEW Exceptions::SilentException (e));\
		}\
		catch (...) {\
			(CE).fStringException = auto_ptr<Exceptions::StringException> (DEBUG_NEW Exceptions::StringException (L"Unknown Exception"));\
		}\



		// these map invalid parameters etc to exceptions (with assertions and appropriate
		// logging)
		void	RegisterDefaultHandler_invalid_parameter ();
		void	RegisterDefaultHandler_pure_function_call ();

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

namespace	Exceptions {
//	class	Win32ErrorException
	inline	Win32ErrorException::Win32ErrorException (DWORD error):
		fError (error)
		{
		}
	inline	Win32ErrorException::operator DWORD () const
		{
			return fError;
		}
	inline	tstring	Win32ErrorException::LookupMessage () const
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
	inline	tstring	Win32StructuredException::LookupMessage () const
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
	inline	tstring	HRESULTErrorException::LookupMessage () const
		{
			return LookupMessage (fHResult);
		}


	
//	class	errno_ErrorException
	inline	errno_ErrorException::operator errno_t () const
		{
			return fError;
		}
	inline	tstring	errno_ErrorException::LookupMessage () const
		{
			return LookupMessage (fError);
		}




//	class	StringException
	inline	StringException::StringException (const wstring& reasonForError):
		fError (reasonForError)
		{
		}
	inline	StringException::operator wstring () const
		{
			return fError;
		}

		
//	class	FileBusyException
	inline	FileBusyException::FileBusyException (const tstring& fileName):
		fFileName (fileName)
		{
		}

//	class	FileFormatException
	inline	FileFormatException::FileFormatException (const tstring& fileName):
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


#endif	/*__Exceptions_h__*/
