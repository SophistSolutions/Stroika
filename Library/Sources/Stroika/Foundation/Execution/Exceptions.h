/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_h_
#define	_Stroia_Foundation_Execution_Exceptions_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<string>
#if		defined(_WIN32)
	#include	<Windows.h>
#endif

#include	"../Characters/StringUtils.h"
#include	"../Configuration/Basics.h"
#include	"../Debug/Trace.h"



namespace	Stroika {	
	namespace	Foundation {


namespace	Execution {


	using	Characters::tstring;


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
		catch (const Execution::RequiredComponentMissingException& e) {\
			(CE).fRequiredComponentMissingException = auto_ptr<Execution::RequiredComponentMissingException> (DEBUG_NEW Execution::RequiredComponentMissingException (e));\
		}\
		catch (const Execution::StringException& e) {\
			(CE).fStringException = auto_ptr<Execution::StringException> (DEBUG_NEW Execution::StringException (e));\
		}\
		catch (const Execution::HRESULTErrorException& e) {\
			(CE).fHRESULTErrorException = auto_ptr<Execution::HRESULTErrorException> (DEBUG_NEW Execution::HRESULTErrorException (e));\
		}\
		catch (const Execution::Win32ErrorException& e) {\
			(CE).fWin32ErrorException = auto_ptr<Execution::Win32ErrorException> (DEBUG_NEW Execution::Win32ErrorException (e));\
		}\
		catch (const Execution::FileFormatException& e) {\
			(CE).fFileFormatException = auto_ptr<Execution::FileFormatException> (DEBUG_NEW Execution::FileFormatException (e));\
		}\
		catch (const Execution::FileBusyException& e) {\
			(CE).fFileBusyException = auto_ptr<Execution::FileBusyException> (DEBUG_NEW Execution::FileBusyException (e));\
		}\
		catch (const Execution::SilentException& e) {\
			(CE).fSilentException = auto_ptr<Execution::SilentException> (DEBUG_NEW Execution::SilentException (e));\
		}\
		catch (...) {\
			(CE).fStringException = auto_ptr<Execution::StringException> (DEBUG_NEW Execution::StringException (L"Unknown Exception"));\
		}\



		// these map invalid parameters etc to Execution (with assertions and appropriate
		// logging)
		void	RegisterDefaultHandler_invalid_parameter ();
		void	RegisterDefaultHandler_pure_function_call ();

}



	}
}
#endif	/*_Stroia_Foundation_Execution_Exceptions_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Exceptions.inl"

