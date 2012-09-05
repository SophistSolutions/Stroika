/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Sources/NetLedIt.java,v 1.1 2004/01/01 04:20:34 lewis Exp $
 *
 * Changes:
 *	$Log: NetLedIt.java,v $
 *	Revision 1.1  2004/01/01 04:20:34  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.14  2002/05/06 21:35:01  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.13  2001/11/27 00:33:02  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.12  2001/09/26 15:53:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.11  2001/08/30 00:43:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.10  2001/05/30 15:26:00  lewis
 *	SPR#0945- Added GetVersionNumber/GetShortVersionString () APIs, and testing
 *	code to demo them, and docs in the ref manual
 *	
 *	Revision 1.9  2001/02/12 15:25:17  lewis
 *	SPR#0854- added java Automation 'GetHeight' method - to compute
 *	pixelheight of a given range of text
 *	
 *	Revision 1.8  2000/10/05 02:42:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.7  2000/10/05 02:41:02  lewis
 *	(fix earlier bad message) - really added SetDirty() method
 *	
 *	Revision 1.5  2000/09/28 19:50:37  lewis
 *	Finished stubs for (ActiveLedIt! based) Java API for NetLedIt!.
 *	
 *	Revision 1.4  2000/09/28 13:56:17  lewis
 *	Got about 1/3 of the ActiveLedIt! OLE Automation API into NetLedIt!
 *	(on pc = mim testing - about to test on mac)
 *	
 *	Revision 1.3  2000/09/22 21:33:42  lewis
 *	lose doit crap from sample. Put in - for testing private PrintToStdOut
 *	helper. Lose old crap from sample code
 *	
 *
 *
 */

import	netscape.plugin.Plugin;


/*
 *	Note - This API is very closely patterned after the ActiveLedIt! 3.0b3 OLE Automation API. At least so at the time of its
 *	creation. The principle difference is that Java (as near as I can tell) supports no properties, so I've had replace those
 *	with Get/Set-methods. Also - as near as I can tell - there are no enums, and no unsigned ints. Those were just mapped to 'int'.
 *			 -- LGP 2000-09-28.
 */
class	NetLedIt	extends	Plugin {
	public native int		GetVersionNumber ();
	public native String	GetShortVersionString ();

	public native boolean	GetDirty ();
	public native void		SetDirty (boolean dirty);
	
	final	static	int	eNoScrollBar			=	0;
	final	static	int	eShowScrollBar			=	1;
	final	static	int	eShowScrollbarIfNeeded	=	2;
	public native int		GetHasVerticalScrollBar ();
	public native void		SetHasVerticalScrollBar (int scrollBarFlag);
	public native int		GetHasHorizontalScrollBar ();
	public native void		SetHasHorizontalScrollBar (int scrollBarFlag);

	public native int		GetMaxUndoLevel ();
	public native void		SetMaxUndoLevel (int maxUndoLevel);

	public native boolean	GetCanUndo ();
	public native boolean	GetCanRedo ();

	public native String	GetText ();
    public native void		SetText (String text);

	public native String	GetTextCRLF ();
    public native void		SetTextCRLF (String textCRLF);

	public native String	GetTextRTF ();
    public native void		SetTextRTF (String rtfText);

	public native String	GetTextHTML ();
    public native void		SetTextHTML (String rtfText);

	/*
	 *	Not sure IF/HOW TODO
	 *					 public native XXXX	GetTextBitmap ();
	 */

	public native  int		GetLength ();

	public native int		GetMaxLength ();
    public native void		SetMaxLength (int maxLength);

	public native boolean	GetSupportContextMenu ();
    public native void		SetSupportContextMenu (boolean supportContextMenu);

	public native boolean	GetSmartCutAndPaste ();
    public native void		SetSmartCutAndPaste (boolean smartCutAndPaste);

	public native boolean	GetWrapToWindow ();
    public native void		SetWrapToWindow (boolean wrapToWindow);

	public native boolean	GetShowParagraphGlyphs ();
    public native void		SetShowParagraphGlyphs (boolean showParagraphGlyphs);

	public native boolean	GetShowTabGlyphs ();
    public native void		SetShowTabGlyphs (boolean showTabGlyphs);

	public native boolean	GetShowSpaceGlyphs ();
    public native void		SetShowSpaceGlyphs (boolean showSpaceGlyphs);

	public native boolean	GetShowHidableText ();
    public native void		SetShowHidableText (boolean showHidableText);

	public native int		GetHidableTextColor ();
    public native void		SetHidableTextColor (int hidableTextColor);

	public native boolean	GetHidableTextColored ();
    public native void		SetHidableTextColored (boolean hidableTextColored);

	public native int		GetSelStart ();
    public native void		SetSelStart (int selStart);

	public native int		GetSelLength ();
    public native void		SetSelLength (int selLength);

	public native String	GetSelText ();
    public native void		SetSelText (String selText);

	public native String	GetSelTextRTF ();
    public native void		SetSelTextRTF (String selTextRTF);

	public native String	GetSelTextHTML ();
    public native void		SetSelTextHTML (String selTextHTML);

	public native int		GetSelColor ();
    public native void		SetSelColor (int selColor);

	public native String	GetSelFontFace ();
    public native void		SetSelFontFace (String selFontFace);

	public native int		GetSelFontSize ();
    public native void		SetSelFontSize (int selFontSize);

	public native int		GetSelBold ();
    public native void		SetSelBold (int selBold);

	public native int		GetSelItalic ();
    public native void		SetSelItalic (int selItalic);

	/*
	 *	Dont support for now - since not on Mac & Windows. Fix in a later version of Led/NetLedIt!
	 *					 long	SelStrikeThru
	 */

	public native int		GetSelUnderline ();
    public native void		SetSelUnderline (int selUnderline);


	final	static	int	eLeftJustification			=	0;
	final	static	int	eRightJustification			=	1;
	final	static	int	eCenterJustification		=	2;
	final	static	int	eFullJustification			=	3;
	final	static	int	eNoCommonJustification		=	4;
	public native int		GetSelJustification ();
    public native void		SetSelJustification (int selJustification);

	public native int		GetSelHidable ();
    public native void		SetSelHidable (int selHidable);


	public native void		AboutBox ();

	public native void		LoadFile (String fileName);
	public native void		SaveFile (String fileName);
	public native void		SaveFileCRLF (String fileName);
	public native void		SaveFileRTF (String fileName);
	public native void		SaveFileHTML (String fileName);

	public native void		Refresh ();
	public native void		ScrollToSelection ();

	public native void		Undo ();
	public native void		Redo ();
	public native void		CommitUndo ();

	public native void		LaunchFindDialog ();
	public native int		Find (int searchFrom, String findText, boolean wrapSearch, boolean wholeWordSearch, boolean caseSensativeSearch);

	public native int		GetHeight (int from, int to);




//PRIVATE  - maybe only tmp hack for debugging, and so I remember how todo this stuff... LGP 2000-09-22
	void	PrintToStdOut (String text)
		{
			System.out.print (text + "\n");
		}
}
