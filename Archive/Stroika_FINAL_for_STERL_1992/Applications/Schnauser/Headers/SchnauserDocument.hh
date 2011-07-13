/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SchnauserDocument__
#define	__SchnauserDocument__
/*
 * $Header: /fuji/lewis/RCS/SchnauserDocument.hh,v 1.2 1992/09/01 17:40:39 sterling Exp $
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
 *	$Log: SchnauserDocument.hh,v $
 *		Revision 1.2  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *		
 *
 *
 */

#include	"Document.hh"

#include	"SchnauserConfig.hh"


class	ProjectWindow;
class	SchnauserDocument : public FileBasedDocument {
	public:
		SchnauserDocument ();
		virtual ~SchnauserDocument ();

		nonvirtual	void		DoNewState ();
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount) const;
		override	FileOffset	EstimatedFileSize ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		static	Exception	sUnknownFileFormat;		// could indicate corrupt file too.

	private:
		ProjectWindow*	fWindow;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SchnauserDocument__*/

