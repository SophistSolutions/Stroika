/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapEditorDocument__
#define	__PixelMapEditorDocument__
/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorDocument.hh,v 1.3 1992/09/01 17:36:41 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PixelMapEditorDocument.hh,v $
 *		Revision 1.3  1992/09/01  17:36:41  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/05/20  00:29:01  lewis
 *		Fix DoWrite_ changed to const.
 *
 *		
 *
 *
 */

#include	"Document.hh"

class	PixelMapEditorWindow;
class	PixelMapEditorDocument : public FileBasedDocument {
	public:
		PixelMapEditorDocument ();
		virtual ~PixelMapEditorDocument ();

		nonvirtual	void		DoNewState ();
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount) const;
		override	off_t		EstimatedFileSize ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		static	Exception	sUnknownFileFormat;		// could indicate corrupt file too.

	private:
		PixelMapEditorWindow*	fWindow;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PixelMapEditorDocument__*/

