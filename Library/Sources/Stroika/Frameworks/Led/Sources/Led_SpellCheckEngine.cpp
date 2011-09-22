/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /home/lewis/share/RFLWork_CVSRoot/DevRoot/ThirdPartyLibs/Led/LedSrc/Sources/Led_SpellCheckEngine.cpp,v 1.3 2009-05-31 15:31:38 Lewis Exp $
 *
 * Changes:
 *	$Log: Led_SpellCheckEngine.cpp,v $
 *	Revision 1.3  2009-05-31 15:31:38  Lewis
 *	*
 *	*********************     HealthFrameWorks Server 1.0      **************************
 *	*
 *
 *	Revision 1.2  2006/10/25 00:34:42  lewis
 *	*
 *	**************************         HealthFrame 2.1         **************************
 *	*
 *	
 *	Revision 1.1  2006/08/31 19:39:35  lewis
 *	copied from latest (3.1b2x) Led code - but renamed to Led_ file names and fixed MINOR issues to get to compile with HF/Newer Visual Studio
 *	
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

#include	"Led_SpellCheckEngine.h"




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


