/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItDocument__
#define	__LedLineItDocument__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Headers/LedLineItDocument.h,v 2.13 2003/12/13 02:00:06 lewis Exp $
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
 *	$Log: LedLineItDocument.h,v $
 *	Revision 2.13  2003/12/13 02:00:06  lewis
 *	small changes so uses new spellcheck code (and stores engine in applicaiton object)
 *	
 *	Revision 2.12  2003/06/04 03:08:26  lewis
 *	SPR#1513: spellcheck engine support
 *	
 *	Revision 2.11  2002/05/06 21:31:26  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.10  2001/11/27 00:28:26  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.9  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.7  1999/07/13 22:39:20  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 2.6  1998/10/30 20:24:11  lewis
 *	use vector<> instead of Led_Array
 *	
 *	Revision 2.5  1998/05/05  00:39:39  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/12/24  04:52:10  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/07/27  16:03:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/07/14  14:44:28  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.1  1997/01/20  05:45:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:51:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	<LSingleDoc.h>

#include	"Command.h"
#include	"ChunkedArrayTextStore.h"

#include	"LedLineItConfig.h"




class	LedLineItView;

class	LedLineItDocument : public LSingleDoc, public MarkerOwner {
	public:
		LedLineItDocument (LCommander *inSuper);
		~LedLineItDocument();

	// Call exactly once (inFileSpec==NULL for new window)
	public:
		nonvirtual	void	BuildDocWindow (const FSSpec* inFileSpec);

	public:
		static	const vector<LWindow*>&	GetDocumentWindows ();

	public:
		override	Boolean	ObeyCommand (CommandT	inCommand, void* ioParam);
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
												UInt16& outMark, Str255 outName
											);

		nonvirtual	void	OnSaveACopyAsCommand ();

	public:
		override	Boolean		IsModified ();

//		override	Boolean	AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt);

		override	void		DoAESave (FSSpec &inFileSpec, OSType inFileType);
		override	void		DoSave ();
		override	void		DoRevert ();
		override	void		DoPrint ();

	public:
		nonvirtual	void	PurgeUnneededMemory ();

	private:
		nonvirtual	void	DoReadCode ();

	public:
		nonvirtual	TextStore&										GetTextStore ();
		nonvirtual	CommandHandler&									GetCommandHandler ();
	protected:
		ChunkedArrayTextStore							fTextStore;
		MultiLevelUndoCommandHandler					fCommandHandler;


	public:
		override	void		DidUpdateText (const UpdateInfo& updateInfo);
		override	TextStore*	PeekAtTextStore () const;

	protected:
		nonvirtual	void	DoSaveHelper ();

	private:
		LedLineItView*	fTextView;

		nonvirtual	void		NameNewDoc ();
		nonvirtual	void		OpenFile (const FSSpec &inFileSpec);
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	TextStore&										LedLineItDocument::GetTextStore ()
		{
			return fTextStore;
		}
	inline	CommandHandler&	LedLineItDocument::GetCommandHandler ()
		{
			return fCommandHandler;
		}


#endif	/*__LedLineItDocument__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

