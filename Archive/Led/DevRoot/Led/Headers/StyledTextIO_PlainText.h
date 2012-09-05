/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_PlainText_h__
#define	__StyledTextIO_PlainText_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO_PlainText.h,v 2.7 2002/05/06 21:33:36 lewis Exp $
 */


/*
@MODULE:	StyledTextIO_PlainText
@DESCRIPTION:
		<p>Support reading/writing plain text to/from word-processing Src/Sink streams.</p>
 */

/*
 * Changes:
 *	$Log: StyledTextIO_PlainText.h,v $
 *	Revision 2.7  2002/05/06 21:33:36  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.6  2001/11/27 00:29:46  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/10/20 13:38:56  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.4  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.2  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
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




/*
@CLASS:			StyledTextIOReader_PlainText
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:
*/
class	StyledTextIOReader_PlainText : public StyledTextIOReader {
	public:
		StyledTextIOReader_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Read ();
		override	bool	QuickLookAppearsToBeRightFormat ();
};




/*
@CLASS:			StyledTextIOWriter_PlainText
@BASES:			@'StyledTextIOWriter'
@DESCRIPTION:
*/
class	StyledTextIOWriter_PlainText : public StyledTextIOWriter {
	public:
		StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Write ();
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif


#endif	/*__StyledTextIO_PlainText_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
