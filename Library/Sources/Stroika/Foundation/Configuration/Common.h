/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Basics_h_
#define	_Stroika_Foundation_Configuration_Basics_h_	1

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


			template	<typename	ENUM>
				ENUM	Inc (typename ENUM e);

		}

		using	Configuration::Byte;
	}
}


#endif	/*_Stroika_Foundation_Configuration_Basics_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


