/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_h_
#define	_Stroia_Foundation_Execution_Exceptions_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<string>
#include	<stdexcept>

#include	"../Configuration/Common.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileBusyException.h"
#include	"../IO/FileFormatException.h"

#include	"StringException.h"

#if		qPlatform_Windows
	#include	"Platform/Windows/Exception.h"
	#include	"Platform/Windows/HRESULTErrorException.h"
	#include	"Platform/Windows/StructuredException.h"
#endif



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Characters::TString;



			// Throw this when an error has already been reported - so that it isn't reported again
			class	SilentException {
			};

			// mostly treat the same as SilentException
			class	UserCanceledException : public SilentException {
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



			/*
			 * Utility to call a Trace message (hopefully an appropriate one) for an exception being thrown... But this function is also specailized to
			 * do call D::DoThrow() for several types - which CAN translate the kind of exception throw. For example, for Platoform:Windows::Execption - 
			 * ERROR_OUTOFMEMORY is translated to std::bad_alloc ().
			 *
			 * ONLY the first variation (with no traceMessage) is template specailized. The overloads which take an extra message are JUST for convenience,
			 * and vector through the 1-arg overload - so as to get is specialization.
			 */
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<typename T>
				void	_NoReturn_	DoThrow (const T& e2Throw, const char* traceMsg);
			template	<typename T>
				void	_NoReturn_ 	DoThrow (const T& e2Throw, const wchar_t* traceMsg);



			template	<>
				void	_NoReturn_	DoThrow (const IO::FileFormatException& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const SilentException& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const UserCanceledException& e2Throw);


			// Just a regular C++ rethrow, but with a DbgTrace message...
			void	_NoReturn_	DoReThrow ();
			void	_NoReturn_	DoReThrow (const char* traceMsg);
			void	_NoReturn_	DoReThrow (const wchar_t* traceMsg);



		#if		qPlatform_Windows
			void	ThrowIfFalseGetLastError (bool test);
			void	ThrowIfFalseGetLastError (BOOL test);
			void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
			void	ThrowIfShellExecError (HINSTANCE r);
		#endif

			template	<typename E>
				void	ThrowIfNull (const void* p, const E& e = E ());
			void	ThrowIfNull (const void* p);


			#define	IgnoreExceptionsForCall(theCode)		try {theCode;} catch (...) {}



			#if		qPlatform_Windows
				// these map invalid parameters etc to Execution (with assertions and appropriate
				// logging)
				void	RegisterDefaultHandler_invalid_parameter ();
				void	RegisterDefaultHandler_pure_function_call ();
			#endif

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

