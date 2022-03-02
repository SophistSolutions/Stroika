/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextEditorDocument__
#define	__TextEditorDocument__
/*
 * $Header: /fuji/lewis/RCS/TextEditorDocument.hh,v 1.2 1992/07/21 18:40:11 sterling Exp $
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
 *	$Log: TextEditorDocument.hh,v $
 *		Revision 1.2  1992/07/21  18:40:11  sterling
 *		reflect File changes
 *
 *		Revision 1.1  1992/06/20  18:37:23  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/05/19  11:12:31  sterling
 *		DoWrite now const
 *
 *		Revision 1.1  92/03/26  17:57:08  17:57:08  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */

#include	"Document.hh"

class	TextEditorDocument : public FileBasedDocument {
	public:
		TextEditorDocument ();
		virtual ~TextEditorDocument ();

		nonvirtual	void		DoNewState ();
	
		override	off_t	EstimatedFileSize ();

	protected:
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount) const;

	private:
		class	TextEditorWindow*	fTEWin;
};



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__TextEditorDocument__*/

