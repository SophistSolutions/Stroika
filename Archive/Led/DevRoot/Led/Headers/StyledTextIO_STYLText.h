/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_STYLText_h__
#define	__StyledTextIO_STYLText_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO_STYLText.h,v 2.7 2002/05/06 21:33:37 lewis Exp $
 */


/*
@MODULE:	StyledTextIO_STYLText
@DESCRIPTION:
		<p>Support MacOS resource based STYL text.</p>
 */

/*
 * Changes:
 *	$Log: StyledTextIO_STYLText.h,v $
 *	Revision 2.7  2002/05/06 21:33:37  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.6  2001/11/27 00:29:46  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/10/20 13:38:57  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.4  2001/10/17 20:42:53  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.3  2001/08/28 18:43:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2000/04/16 13:58:54  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.1  1999/12/18 03:56:46  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *
 *	<========== CODE MOVED HERE FROM StyledTextIO.h ==========>
 *
 *
 *
 *
 */

#include	"StyledTextIO.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif




#if		qMacOS
/*
@CLASS:			StyledTextIOReader_STYLText
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:	<p><b>MacOS ONLY</b></p>
*/
class	StyledTextIOReader_STYLText : public StyledTextIOReader {
	public:
		StyledTextIOReader_STYLText (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Read ();
		override	bool	QuickLookAppearsToBeRightFormat ();
};
#endif







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif


#endif	/*__StyledTextIO_STYLText_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
