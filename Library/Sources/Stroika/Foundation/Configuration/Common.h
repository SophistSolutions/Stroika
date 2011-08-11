/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Common_h_
#define	_Stroika_Foundation_Configuration_Common_h_	1

#include	<cstddef>
#include	<cstdint>
#include	<ciso646>


namespace	Stroika {
	namespace	Foundation {

		using	namespace	std;

		namespace	Configuration {

			typedef	unsigned char	Byte;

			#define	Nil		NULL

			#define	nonvirtual
			#define override		virtual

			#define	NEltsOf(X)		(sizeof((X))/sizeof((X)[0]))
			#define	StartOfArray(X)	(&(X)[0])
			#define	EndOfArray(X)	(&(X)[NEltsOf(X)])


			/*
			 *	The StandaredC++ mechanism of commenting out unused parameters isn't good enuf
			 *	in the case where the parameters might be used conditionally. This hack is
			 *	to shutup compiler warnings in those cases.
			 */
			#ifndef	Arg_Unused
			#define	Arg_Unused(x)	((void) &x)
			#endif




			/*
			 *	Sometimes its handy to mark a function as not actually returning (because of throws or other reasons)
			 *	This can allow the compiler to occasionally better optimize, but mostly avoid spurrious warnings.
			 */
			#if defined(_MSC_VER)
				#define	_NoReturn_	__declspec(noreturn)
			#elif defined (__GNUG__ )
				#define	_NoReturn_	__attribute__((noreturn))
			#else
				#define	_NoReturn_
			#endif




			/*
			 *	Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
			 * trace macros).
			 */
			#if defined(_MSC_VER)
				#define	_NoOp_	__noop
			#else
				#define	_NoOp_
			#endif


			template	<typename	ENUM>
				ENUM	Inc (ENUM e);

		}

		using	Configuration::Byte;
	}
}


#endif	/*_Stroika_Foundation_Configuration_Common_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


