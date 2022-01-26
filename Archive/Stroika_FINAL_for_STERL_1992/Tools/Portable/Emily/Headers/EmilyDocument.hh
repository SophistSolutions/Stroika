/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__EmilyDocument__
#define	__EmilyDocument__

/*
 * $Header: /fuji/lewis/RCS/EmilyDocument.hh,v 1.6 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyDocument.hh,v $
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.10  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 *
 *
 */

#include	"Document.hh"

class	EmilyWindow;
class	Printer;

class	EmilyDocument : public FileBasedDocument {
	public:
		EmilyDocument ();
		virtual ~EmilyDocument ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		override	void		ChooseOldDocument ();
		nonvirtual	void		DoNewState ();
		override	void		Save ();
		override	off_t		EstimatedFileSize ();

		nonvirtual	void			EditModeChanged (Boolean newEditMode);
		nonvirtual	EmilyWindow&	GetWindow ();

		nonvirtual	PathName	GetHeaderFilePathName ();
		nonvirtual	PathName	GetSourceFilePathName ();

	protected:
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount) const;
				
	private:
		EmilyWindow*	fEmilyWindow;
		Printer*		fPrinter;
};


#if		!qRealTemplatesAvailable
	typedef	class	EmilyDocument*	EmDcPtr;
	#define	EmilyDocumentPtr	EmDcPtr
	
	Declare (Iterator, EmDcPtr);
	Declare (Collection, EmDcPtr);
	Declare (AbSequence, EmDcPtr);
	Declare (Array, EmDcPtr);
	Declare (Sequence_Array, EmDcPtr);
	Declare (Sequence, EmDcPtr);
#endif

#endif	/* __EmilyDocument__ */
