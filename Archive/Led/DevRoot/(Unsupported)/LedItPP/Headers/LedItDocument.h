/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItDocument__
#define	__LedItDocument__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/LedItDocument.h,v 2.18 2000/04/03 19:09:27 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItDocument.h,v $
 *	Revision 2.18  2000/04/03 19:09:27  lewis
 *	fHidableTextDatabase support
 *	
 *	Revision 2.17  1999/12/21 21:05:26  lewis
 *	Split StyledTextIO into separate files
 *	
 *	Revision 2.16  1999/07/13 22:39:20  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds that were only needed for these old compilers
 *	
 *	Revision 2.15  1998/10/30 15:03:00  lewis
 *	Use vector - not Led_Array
 *	
 *	Revision 2.14  1998/05/05  00:36:07  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1998/04/08  02:34:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/12/24  04:41:30  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1997/09/29  17:55:37  lewis
 *	Use WordProcessor now.
 *	Use new StyleDatabase API, and include ParagraphDatabase.
 *
 *	Revision 2.10  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.9  1997/07/14  01:23:43  lewis
 *	Renamed TextStore_ to TextStore etc..
 *
 *	Revision 2.8  1997/01/10  03:43:54  lewis
 *	throw specifiers
 *
 *	Revision 2.7  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.6  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/10/31  01:03:50  lewis
 *	Add fHTMLInfo so we keep track of hidden/removed HTML info on read of html file for later output.
 *	And a few other minor changes.
 *
 *	Revision 2.4  1996/09/30  15:07:06  lewis
 *	GetDocumentWindows() list.
 *
 *	Revision 2.3  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/05/05  14:53:37  lewis
 *	OnSaveACopyAsCommand ()? CLeanups?
 *
 *	Revision 2.1  1996/04/18  16:02:54  lewis
 *	define PurgeUnneededMemory () method.
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<LSingleDoc.h>

#include	"Command.h"
#include	"ChunkedArrayTextStore.h"
#include	"StyledTextImager.h"
#include	"StyledTextIO.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_RTF.h"
#include	"WordProcessor.h"

#include	"LedItConfig.h"






class	LedItView;

class	LedItDocument : public LSingleDoc, public MarkerOwner {
	public:
		LedItDocument (LCommander *inSuper, Led_FileFormat format);
		~LedItDocument();

	// Call exactly once (inFileSpec==NULL for new window)
	public:
		nonvirtual	void	BuildDocWindow (const FSSpec* inFileSpec);

	public:
		static	const vector<LWindow*>&	GetDocumentWindows ();

	public:
		override	Boolean	ObeyCommand (CommandT	inCommand, void* ioParam);
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
												Char16& outMark, Str255 outName
											);

		nonvirtual	void	OnSaveACopyAsCommand ();

	public:
		override	Boolean		IsModified ();

		override	Boolean	AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt);

		override	void		DoAESave (FSSpec &inFileSpec, OSType inFileType);
		override	void		DoSave ();
		override	void		DoRevert ();
		override	void		DoPrint ();

	public:
		nonvirtual	void	PurgeUnneededMemory ();

	private:
		nonvirtual	void	DoReadCode ();

	public:
		nonvirtual	TextStore&			GetTextStore ();
		nonvirtual	CommandHandler&		GetCommandHandler ();
	protected:
		ChunkedArrayTextStore						fTextStore;
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
		WordProcessor::ParagraphDatabasePtr			fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr		fHidableTextDatabase;
		MultiLevelUndoCommandHandler				fCommandHandler;


	public:
		override	void		DidUpdateText (const UpdateInfo& updateInfo) throw ();
		override	TextStore*	PeekAtTextStore () const;

	protected:
		nonvirtual	void	DoSaveHelper ();

	private:
		Led_FileFormat	fFileFormat;
		HTMLInfo		fHTMLInfo;
		LedItView*		fTextView;

		nonvirtual	void		NameNewDoc ();
		nonvirtual	void		OpenFile (const FSSpec &inFileSpec);
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	TextStore&	LedItDocument::GetTextStore ()
		{
			return fTextStore;
		}
	inline	CommandHandler&	LedItDocument::GetCommandHandler ()
		{
			return fCommandHandler;
		}


#endif	/*__LedItDocument__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

