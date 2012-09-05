/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_MIMETextEnriched_h__
#define	__StyledTextIO_MIMETextEnriched_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO_MIMETextEnriched.h,v 2.6 2002/05/06 21:33:36 lewis Exp $
 */


/*
@MODULE:	StyledTextIO_MIMETextEnriched
@DESCRIPTION:
 */

/*
 * Changes:
 *	$Log: StyledTextIO_MIMETextEnriched.h,v $
 *	Revision 2.6  2002/05/06 21:33:36  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.5  2001/11/27 00:29:45  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.2  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.1  1999/12/18 03:56:46  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *
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
@CLASS:			StyledTextIOReader_MIMETextEnriched
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:
*/
class	StyledTextIOReader_MIMETextEnriched : public StyledTextIOReader {
	public:
		StyledTextIOReader_MIMETextEnriched (SrcStream* srcStream, SinkStream* sinkStream);

	public:
		override	void	Read ();
		override	bool	QuickLookAppearsToBeRightFormat ();

	private:
		nonvirtual	void	SkipWhitespace ();
		nonvirtual	void	SkipOneLine ();
		nonvirtual	bool	ScanFor (const char* matchMe, bool ignoreCase = true);
		nonvirtual	bool	LookingAt (const char* matchMe, bool ignoreCase = true);

	private:
		nonvirtual	Led_FontSpecification	GetAdjustedCurrentFontSpec () const;

		int		fBoldMode;			// non-zero means YES - can be nested
		int		fItalicMode;		// ""
		int		fUnderlineMode;		// ""
		int		fFixedWidthMode;	// ""
		int		fFontSizeAdjust;	// ""
		int		fNoFillMode;		// ""
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



#if		qLedUsesNamespaces
}
#endif


#endif	/*__StyledTextIO_MIMETextEnriched_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
