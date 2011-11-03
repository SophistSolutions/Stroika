/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Led_SpellCheckEngine.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {


/*
 ********************************************************************************
 ******************************** SpellCheckEngine ******************************
 ********************************************************************************
 */
SpellCheckEngine::~SpellCheckEngine ()
{
}

#if		qDebug
/*
@METHOD:		SpellCheckEngine::Invariant_
@DESCRIPTION:	<p>Default implementaiton for calls to @'SpellCheckEngine::Invariant'. Overridden in subclasses
		to check validity of SpellCheck engine.</p>
*/
void	SpellCheckEngine::Invariant_ () const
{
}
#endif



		}
	}
}






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


