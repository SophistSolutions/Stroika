/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_ErrNoException_h_
#define	_Stroia_Foundation_Execution_ErrNoException_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<cerrno>

#include	"StringException.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Characters::TString;


			#if		!qCompilerAndStdLib_Supports_errno_t
				typedef	int	errno_t;
			#endif

			class	errno_ErrorException : public StringException {
				public:
					explicit errno_ErrorException (errno_t e);

					operator errno_t () const;

				public:
					static	TString	LookupMessage (errno_t e);
					nonvirtual	TString	LookupMessage () const;

				public:
					// throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
					// as good a fit.
					static	_NoReturn_	void	DoThrow (errno_t error);

				private:
					errno_t	fError;
			};


			// Just pre-declare DoThrow><> template here so we can specailize
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<>
				void	_NoReturn_	DoThrow (const errno_ErrorException& e2Throw);

			void	ThrowIfError_errno_t (errno_t e = errno);

		}
	}
}
#endif	/*_Stroia_Foundation_Execution_ErrNoException_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ErrNoException.inl"

