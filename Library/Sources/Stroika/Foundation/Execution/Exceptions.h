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

//#include	"StringException.h"

#if		qPlatform_Windows
	#include	"Platform/Windows/Exception.h"
	#include	"Platform/Windows/HRESULTErrorException.h"
	#include	"Platform/Windows/StructuredException.h"
#endif


/*
 *	TODO:
 *		o	Probably get rid of this, or refactor it alot more.
 */



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
				void	_NoReturn_	DoThrow (const SilentException& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const UserCanceledException& e2Throw);


			// Just a regular C++ rethrow, but with a DbgTrace message...
			void	_NoReturn_	DoReThrow ();
			void	_NoReturn_	DoReThrow (const char* traceMsg);
			void	_NoReturn_	DoReThrow (const wchar_t* traceMsg);


			template	<typename E>
				void	ThrowIfNull (const void* p, const E& e = E ());
			void	ThrowIfNull (const void* p);


			#define	IgnoreExceptionsForCall(theCode)		try {theCode;} catch (...) {}


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

