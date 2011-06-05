/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__StroiaFoundationConfigurationBasics_h__
#define	__StroiaFoundationConfigurationBasics_h__	1

#include	<cstddef>
#include	<cstdint>


namespace	Stroika {	
	namespace	Foundation {

		using	namespace	std;

		namespace	Configuration {

			typedef	unsigned char	Byte;

			#define	Nil		NULL

			#define	and	&&
			#define	or	||
			#define	not	!

			#define	nonvirtual
			#define override		virtual

			#define	NEltsOf(X)		(sizeof(X)/sizeof(X[0]))
			#define	StartOfArray(X)	(&X[0])
			#define	EndOfArray(X)	(&X[NEltsOf(X)])


			/*
			 *	The StandaredC++ mechanism of commenting out unused parameters isn't good enuf
			 *	in the case where the parameters might be used conditionally. This hack is
			 *	to shutup compiler warnings in those cases.
			 */
			#ifndef	Arg_Unused
			#define	Arg_Unused(x)	((void) &x)
			#endif



			double	nan ();

			template	<typename	ENUM>
				ENUM	Inc (typename ENUM e);

		}

		using	Configuration::Byte;
	}
}


#endif	/*__StroiaFoundationConfigurationBasics_h__*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Basics.inl"


