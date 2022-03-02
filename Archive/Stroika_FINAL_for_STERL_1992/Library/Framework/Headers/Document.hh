/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Document__
#define	__Document__

/*
 * $Header: /fuji/lewis/RCS/Document.hh,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Document.hh,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/16  05:30:43  lewis
 *		Keep a PathName always in FileBasedDocument - not PathName*. Use
 *		PathName::kBad to deal with untitled case.
 *
 *		Revision 1.4  1992/07/14  19:46:38  lewis
 *		Use off_t instead of FileOffset since latter deleted from File.hh.
 *
 *		Revision 1.2  1992/07/02  03:49:29  lewis
 *		Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.9  1992/05/19  10:18:46  sterling
 *		made dowrite const
 *
 *		Revision 1.8  92/03/26  17:11:47  17:11:47  sterling (Sterling Wight)
 *		added default version for document
 *		
 *		Revision 1.7  1992/02/18  03:52:36  lewis
 *		Had to change type name of Version to SaveableVersion to avoid name conflict.
 *
 *
 */

#include	"Bag.hh"
#include	"Exception.hh"
#include	"File.hh"

#include	"Saveable.hh"
#include	"Window.hh"


#if		!qRealTemplatesAvailable
	typedef	class Document*	DocumentPtr;
	Declare (Iterator, DocumentPtr);
	Declare (Collection, DocumentPtr);
	Declare (AbBag, DocumentPtr);
	Declare (Array, DocumentPtr);
	Declare (Bag_Array, DocumentPtr);
	Declare (Bag, DocumentPtr);
#endif



class	Document : public WindowController, public virtual Changeable {
	public:
		Document ();
		virtual ~Document ();

		virtual	String	GetPrintName ()			=	Nil;

		virtual	void	TryToClose ()			=	Nil;
		virtual	void	Close ();
		virtual	Boolean	WouldCloseDocument (Window& aWindow);

		override	void	HandleClose (Window& ofWindow);

		static	Iterator(DocumentPtr)*	MakeDocumentIterator ();

	private:
		static	Bag(DocumentPtr)	sDocuments;
};


class	DocumentDirtier : public Command {
	public:
		DocumentDirtier (Document& document);

		override	void	DoIt ();
		override	void	UnDoIt ();

	private:
		Document&	fDocument;
};




extern	Exception	gUserCancelCloseException;

class	FileBasedDocument : public Document, public Saveable {
	public:
		FileBasedDocument (SaveableVersion maxVersion = 1);
		virtual ~FileBasedDocument ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		/*
		 * Routines to save/read and access filename.
		 * These are NOT ususally overridden.
		 *
		 * PathName::kBad means no name set/Untitled.
		 */
		nonvirtual	const PathName&	GetPathName () const;
		virtual		void			SetPathName (const PathName& pathName);

		virtual		void		Save ();
		virtual		void		SaveAs (const PathName& pathName);
		virtual		void		Read ();
		virtual		void		ReadFrom (const PathName& pathName);

		virtual		off_t		EstimatedFileSize ()			=	Nil;	// for pre-flighting

		/*
		 * Put up dialog for user, and allow him to choose a document name.  These routines
		 * generate a failure on user cancels.
		 */
		virtual	void	GenerateDefaultName (String& name);
		virtual	void	ChooseNewDocument ();
		virtual	void	ChooseOldDocument ();

		override	String	GetPrintName ();
		override	void	TryToClose ();

		nonvirtual	Boolean	GetKeepsFileOpen () const;
		nonvirtual	void	SetKeepsFileOpen (Boolean keepsFileOpen);

#if		qMacOS
		// mac 'finder creator & type' of files created
		nonvirtual	Boolean	GetOpenResourceFork () const;
		nonvirtual	void	SetOpenResourceFork (Boolean openResourceFork);
		nonvirtual	short	GetMacResFileRefNum () const;	// only legal when the file is open (assertion error otherwise)
															// these times include in the context of a DoRead/DoWrite,
															// and after the file has first been created when fKeepsFileOpen is True
#endif	/*qMacOS*/

#if		qMacOS
		// mac 'finder creator & type' of files created
		nonvirtual	UInt32	GetFileCreator () const;
		nonvirtual	void	SetFileCreator (UInt32 creator);
		nonvirtual	UInt32	GetFileType () const;
		nonvirtual	void	SetFileType (UInt32 fileType);
#endif	/*qMacOS*/

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

	private:
#if		qMacOS
		Boolean		fOpenResourceFork;
		short		fMacResFileRefNum;
#endif	/*qMacOS*/
#if		qMacOS
		UInt32		fFileCreator;
		UInt32		fFileType;
#endif	/*qMacOS*/
		String		fPrintName;
		PathName	fFilePathName;
		Boolean		fKeepsFileOpen;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Document__*/

