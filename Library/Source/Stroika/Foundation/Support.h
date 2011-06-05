/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Support_h__
#define	__Support_h__	1

#include	"StroikaPreComp.h"

#include	"StroikaConfig.h"

#include	<cstddef>



namespace	Stroika {	
	namespace	Foundation {


using	namespace	std;

typedef	unsigned char	Byte;


/*
 *	Assume the define _DEBUG is used throughout the code to indicate DEBUG mode (assertions on). Assure NDEBUG flag
 *	is set consistently (even if its not explicitly checked).
 */
#if		defined(_DEBUG) && defined (NDEBUG)
#error	CONFLICT - ONE OR THE OTHER
#endif
#if		!defined(_DEBUG) && !defined (NDEBUG)
#error	DEFINE - ONE OR THE OTHER
#endif


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


// We should automate detecing this, but I don't know any portable way todo so at compile time - just runtime.
#define	qIsLittleEndian		1
#define	qIsBigEndian		0





namespace	R4LLib {

	float	GetTickCount ();
	double	nan ();

	template	<typename	ENUM>
		ENUM	Inc (typename ENUM e);



	// For a contiguous container (such as a vector or basic_string) - find the pointers to the start/end of the container
	template	<typename CONTAINER>
		typename	CONTAINER::value_type*	Start (CONTAINER& c);
	template	<typename CONTAINER>
		typename	typename const CONTAINER::value_type*	Start (const CONTAINER& c);
	template	<typename CONTAINER>
		typename	CONTAINER::value_type*	End (CONTAINER& c);
	template	<typename CONTAINER>
		typename	typename const CONTAINER::value_type*	End (const CONTAINER& c);

	template	<typename	CONTAINER>
		void	ReserveSpeedTweekAdd1 (typename CONTAINER& c, size_t kMinChunk = 10);
	template	<typename	CONTAINER>
		void	ReserveSpeedTweekAddN (typename CONTAINER& c, size_t n, size_t kMinChunk = 10);

}

	}
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Support.inl"


#endif	/*__Support_h__*/
