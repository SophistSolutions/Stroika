/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SpellCheckEngine.cpp,v 2.2 2003/03/21 14:51:40 lewis Exp $
 *
 * Changes:
 *	$Log: SpellCheckEngine.cpp,v $
 *	Revision 2.2  2003/03/21 14:51:40  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.1  2002/09/09 03:08:08  lewis
 *	SPR#1093- first draft of SpellCheckEngine/SpellCheckEngine_Basic
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"SpellCheckEngine.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif


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



#if		qLedUsesNamespaces
}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


