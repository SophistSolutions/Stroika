/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	____Led_GDI_h___
#define	____Led_GDI_h___	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/LedGDI.h,v 2.143 2003/12/29 15:45:17 lewis Exp $
 */


/*
@MODULE:	LedGDI
@DESCRIPTION:
		<p>Graphix Library support. This code encapsulates access to underlying GDI for modularity sake.
	For historical reasons - as of Led 3.0 - there is still some direct access to underlying GDI in parts of Led
	but that will gradually (hopefully right after 3.0) be eliminated.</p>
		<p>Also - no class library dependencies (ie no MFC dependencies). So we can build MFC-Free Win32 apps more
	easily.</p>
 */


/*
 * Changes:
 *	$Log: LedGDI.h,v $
 *	Revision 2.143  2003/12/29 15:45:17  lewis
 *	Added Led_Bitmap::LoadBitmap and auto_gdi_ptr class
 *	
 *	Revision 2.142  2003/05/29 14:50:23  lewis
 *	SPR#1510: dont set clips/restore clips for metafile DCs - doesn't work.
 *	
 *	Revision 2.141  2003/05/28 16:56:48  lewis
 *	added CenterRectInRect() function
 *	
 *	Revision 2.140  2003/05/17 21:42:36  lewis
 *	minor tweeks to recent DIB changes to get compiling (safely) on Mac
 *	
 *	Revision 2.139  2003/05/17 21:21:07  lewis
 *	SPR#1489: DIB fixes. Only REAL bug fixes was Led_GetDIBPalletByteCount for case of biCompression==BI_BITFIELDS. That was what caused the images to look slightly messed up. But also - added code for Non-30-version DIBs from DibLook sample code
 *	
 *	Revision 2.138  2003/05/01 20:03:46  lewis
 *	LED_TWIPS::operator -=
 *	
 *	Revision 2.137  2003/04/18 17:01:33  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 2.136  2003/04/10 15:53:26  lewis
 *	(vaguely related to SPR#1329)Added Intersection (const Led_IncrementalFontSpecification&,const Led_IncrementalFontSpecification&) function
 *	
 *	Revision 2.135  2003/04/01 18:28:22  lewis
 *	Added Intersection() function which is a synonym for operator*(RECT,RECT)
 *	
 *	Revision 2.134  2003/03/03 16:46:58  lewis
 *	SPR#1318 - fix OffscreenBitmap code for WIN32 to properly cache the BM if its size doesn't change
 *	
 *	Revision 2.133  2003/02/26 22:19:56  lewis
 *	added Led_Tablet_::FrameRectangle helper
 *	
 *	Revision 2.132  2003/02/24 14:26:17  lewis
 *	fix missing qWindows on some windows specific code
 *	
 *	Revision 2.131  2003/02/14 20:40:14  lewis
 *	SPR#1303 - qLed_CharacterSet is now obsolete. Instead - use qSingleByteCharacters, qMultiByteCharacters, or qWideCharacters
 *	
 *	Revision 2.130  2003/02/13 15:26:32  lewis
 *	SPR#1271 - more minor cleanups to the WIN32 RecolorHelper code - keep it assocaited with teh Led_Tablet_ and not a static (construct a bit more - but get really big cache guys cleaned out if not needed). Also lots of other minor cleanups to the code.
 *	
 *	Revision 2.129  2003/02/12 01:57:37  lewis
 *	SPR#1271- Lots of code cleanups to the recoloring (Hilight) code. Deleted tons of approaches
 *	that didn't pan out (not fast enuf). Still more cleanup todo, but its close
 *	
 *	Revision 2.128  2003/02/07 01:05:52  lewis
 *	added Distance/Distance_Squared metric functions for colors (help for SPR#1271)
 *	
 *	Revision 2.127  2003/02/05 22:38:44  lewis
 *	SPR#1271- Fixed ::GetSysColor() args - SB COLOR_HILIGHTTEXT instead of COLOR_CAPTION_TEXT etc.
 *	Also - get rid of PER-Led_Tablet instance of RecolorHelper. Instead keep one global instance.
 *	
 *	Revision 2.126  2003/02/05 17:58:27  lewis
 *	SPR#1271 - added RecolorHelper for Win32 HilightARectangle code. Still more work todo (its too slow)
 *	
 *	Revision 2.125  2003/02/04 21:39:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.124  2003/01/30 17:45:53  lewis
 *	add Led_FontObject::GetObject ()
 *	
 *	Revision 2.123  2003/01/29 19:15:04  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.122  2003/01/28 22:47:16  lewis
 *	add Led_TWIPS::kPoint and Led_TWIPS::kInch
 *	
 *	Revision 2.121  2003/01/17 00:39:31  lewis
 *	add require valid rect for Led_Region::CTOR
 *	
 *	Revision 2.120  2003/01/16 14:12:30  lewis
 *	use the spelling 'hilight' instead of 'hilite' a bit more consistently
 *	
 *	Revision 2.119  2003/01/11 19:28:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.118  2002/12/08 16:11:40  lewis
 *	SPR#1202 - Move Win32CharSetToCodePage () from LedGDI.h to CodePage.h and various related cleanups
 *	to that code
 *	
 *	Revision 2.117  2002/12/03 15:48:56  lewis
 *	SPR#1191- use new TextDirection enum defined in LedGDI.h. Get rid of ScriptRunElement::fLength.
 *	dd direction arg to Led_Tablet_::TabbedTextOut () and use that to call (WIN32)Led_Tablet_::SetTextAlign().
 *	Hopefully this fixes display on Win9x of Arabic text
 *	
 *	Revision 2.116  2002/12/03 14:28:46  lewis
 *	SPR#1190 - changed name for (TextImager/Led_Tablet_)::HilightARectangle to HilightArea
 *	
 *	Revision 2.115  2002/11/04 01:03:25  lewis
 *	fix syntax error (typename) whcih made gcc barf
 *	
 *	Revision 2.114  2002/11/01 18:34:49  lewis
 *	change calls to LPtoDP, GetWindowOrgEx and GetViewportOrgEx from using m_hDC to using m_hAttribDC as
 *	part of SPR#1169. Bug not totally fixed by this - but at least no more assert errors
 *	
 *	Revision 2.113  2002/11/01 13:40:37  lewis
 *	fix typo- had typedef instead of typename
 *	
 *	Revision 2.112  2002/11/01 13:11:55  lewis
 *	cleanup some warnings - use 'typename'
 *	
 *	Revision 2.111  2002/10/31 03:24:45  lewis
 *	Work around qNestedTemplateCTORInTemplateBug for GCC and MSVC60
 *	
 *	Revision 2.110  2002/10/30 22:40:51  lewis
 *	minor cleaups to CvtToFromTwips stuff and Rect_Base<>
 *	
 *	Revision 2.109  2002/10/30 15:19:15  lewis
 *	fix Rect_Base<> so can be used for Led_TWIPS (initialize). Added new Led_Tablet_::CvtFromTWIPS overloads
 *	to handle Led_Point/Rect <-> Led_TWIPS_Point etc (SPR#1157)
 *	
 *	Revision 2.108  2002/10/30 13:29:02  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed)
 *	to make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them in
 *	tons of places - but still TONS more required. Leave that battle for another day. I'm not even SURE its a
 *	good idea. Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin
 *	with with all this nonsese)
 *	
 *	Revision 2.107  2002/10/30 01:29:31  lewis
 *	SPR#1155 - use Point_Base template for Led_TWIPS_Point
 *	
 *	Revision 2.106  2002/10/30 00:19:19  lewis
 *	SPR#1155- Use new Point_Base<> template
 *	
 *	Revision 2.105  2002/10/23 21:27:50  lewis
 *	Fix less<Led_Color> specialization for the case where qLedUsesNamespaces is false (gcc currently)
 *	
 *	Revision 2.104  2002/10/23 20:42:05  lewis
 *	because of bugs with MSVC6- dont say using namespace Led - and instead be specific about Led::Led_Color -
 *	less<Led_Color> declaration
 *	
 *	Revision 2.103  2002/10/21 12:41:00  lewis
 *	support less<Led_Color> template so you can easily create a set<> template (as in SPR#1135). Chose NOT to define
 *	operator< since that really doesn't make much sense for a color
 *	
 *	Revision 2.102  2002/10/08 15:49:16  lewis
 *	when constructing a Led_Region whcih is empty (but not 0,0,0,0) Win32 creates a 0,0,0,0 region.
 *	Treat this as OK in Led_Region CTOR (ie dont assert out)
 *	
 *	Revision 2.101  2002/09/18 04:00:46  lewis
 *	added operator + / - for Led_TWIPS
 *	
 *	Revision 2.100  2002/09/11 04:16:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.99  2002/05/06 21:33:25  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.98  2002/05/01 22:04:31  lewis
 *	qSilenceAnnoyingCompilerWarnings to avoid warning from VC.Net
 *	
 *	Revision 2.97  2002/04/22 21:43:05  lewis
 *	added Led_GetDIBImageRowByteCount() helper as part of fix SPR#0811
 *	
 *	Revision 2.96  2001/11/27 00:29:38  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.95  2001/10/20 13:38:54  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.94  2001/10/19 00:08:46  lewis
 *	DocComments & cleanups
 *	
 *	Revision 2.93  2001/10/17 21:56:06  lewis
 *	DocComment cleanups
 *	
 *	Revision 2.92  2001/10/17 20:42:49  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.91  2001/09/26 22:12:43  lewis
 *	SPR#1049- fix qInternalErrorWithStaticRegionDeclaredInFunction
 *	
 *	Revision 2.90  2001/09/26 15:41:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.89  2001/09/24 14:24:54  lewis
 *	SPR#1042- major cleanups to Led_Region code (not fully backward compatable. New CTORs and
 *	much more portable. New class OffscreenTablet::OT (turned out to not be needed but could
 *	be in the future). Led_Tablet_::ClipNarrowAndRestore added. New Led_Tablet_::Get/SetClip code.
 *	None of this is used much, but could be in a future release. Lots of new operator overloads
 *	(e.g. operator* for intersection, etc...)
 *	
 *	Revision 2.88  2001/09/12 16:05:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.87  2001/09/12 15:59:34  lewis
 *	get compiling on linux
 *	
 *	Revision 2.86  2001/09/12 14:53:29  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo
 *	Led_Region support for MacOS so now more like with PC (no operator conversion to macregion
 *	and auto constructs OSRegion except when called with region arg - in whcih case we dont OWN
 *	region and dont delete it on DTOR
 *	
 *	Revision 2.85  2001/09/11 22:29:14  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.84  2001/09/08 16:08:50  lewis
 *	SPR#1017- added EnsureRectInRect/EnsureRectOnScreen utility routines (to better specify
 *	window sizes at app startup)
 *	
 *	Revision 2.83  2001/08/30 01:25:43  lewis
 *	cleanup (portable) Led_Region code.
 *	
 *	Revision 2.82  2001/08/29 23:00:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.81  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.80  2001/07/31 15:07:18  lewis
 *	use static_cast<> instead of old C++ cast in a few places
 *	
 *	Revision 2.79  2001/07/19 21:05:20  lewis
 *	fix some small sytnax quirks only complained about by MWERKS compiler
 *	
 *	Revision 2.78  2001/07/19 19:53:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.77  2001/07/19 02:21:44  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.76  2001/07/12 22:40:51  lewis
 *	(related to SPR#0906)- Move ListStyle type definition here so can be used in RTF module
 *	
 *	Revision 2.75  2001/06/05 14:18:09  lewis
 *	SPR#0905- use resolution of 100 instead of 72/75 for xwindows. And fix CVTToFROMTWIPS etc code
 *	to not hardwrie res - but grab from Led_GDIGlobals (on mac/XWin, on Win already did better and
 *	grabbed from tablet)
 *	
 *	Revision 2.74  2001/06/04 16:50:38  lewis
 *	move Led_Tablet_::CTOR/DTOR to .cpp file. SPR#0929- use new map<> to keep track (cache) several XFontStruct
 *	objects (XWindows) - instead of just one, and fixed bug where if we had to do name mapping, wasn't being
 *	cached at all (times new roman).
 *	
 *	Revision 2.73  2001/05/29 22:59:48  lewis
 *	added new color names (kLimeGreen/Fusia/Aqua) to rationalize with names we use elsewhere. Fix what
 *	WordPRocessor cmdnames/color coordination. Sync with 'Web Design in a Nutshell' - HTML reference
 *	name/values. Part of SPR#0943
 *	
 *	Revision 2.72  2001/05/26 18:34:30  lewis
 *	Added AsLedRect/AsXRect() helpers.
 *	
 *	Revision 2.71  2001/05/24 15:48:02  lewis
 *	SPR#0929- a little work on X-Windows drawing speed (very simple caching - must redo much better)
 *	
 *	Revision 2.70  2001/05/22 21:32:36  lewis
 *	make Led_Tablet_::ParseFontName () public, so it can be used by new Led_FontSpecification::SetFromOSRep() -
 *	all X-Windows only routines
 *	
 *	Revision 2.69  2001/05/18 20:59:19  lewis
 *	SPR#0922 added Led_Tablet_::ScrollBitsAndInvalRevealed
 *	
 *	Revision 2.68  2001/05/16 15:59:19  lewis
 *	support op== for font object/XWindows, and fUnderline support as well. ALso X(X&) ledfont ctor
 *	
 *	Revision 2.67  2001/05/12 22:30:47  lewis
 *	SPR#0915- got OffscreenTablet stuff mostly working (seems OK but slow) on XWindows
 *	
 *	Revision 2.66  2001/05/11 23:39:33  lewis
 *	SPR#0915- Added class OffscreenTablet to abstract away common code from MultiRowTextImager/SimpleTextImager
 *	to handle offscreen bitmap drawing. Much cleanedup (HOPEFULLY not broken).
 *	
 *	Revision 2.65  2001/05/11 19:38:25  lewis
 *	fix memleak from some calls to XListFonts (didnt always call XFreeFontNames). SPR#0791 progress - now
 *	handle italics for XWindows. SPR#0914- cache XWindows fontname mappings for speed.
 *	
 *	Revision 2.64  2001/05/08 21:40:15  lewis
 *	use Led_SDK_String instead of string for args to Led_Tablet_::ParseFontName etc. XWindows fBold/fItalic
 *	font attributes :1 size.
 *	
 *	Revision 2.63  2001/05/07 22:22:23  lewis
 *	SPR#0888- finishing touches on font support (well - at least on getting it usable for xwindows).
 *	Did FontMatcher(BestMatch). Other small cleanups.
 *	
 *	Revision 2.62  2001/05/07 16:45:07  lewis
 *	SPR#0888. CLeaned up Led_FontMetrics stuff (esp for X-windows - but also for Mac/Win). New portable
 *	Led_Tablet_::GetFontMetrics method. Now Led_FontMEtrics for x-windows produces good values.
 *	
 *	Revision 2.61  2001/05/05 12:28:12  lewis
 *	SPR#0888 - lots of work on X-Windows font details. FontSize/bold/italics mostly supported. Fixed serious
 *	bug(s) with Led_Tablet_::MeasureText () - wrong ptr arg to XTextWidth - was lucky it worked as well as it did
 *	
 *	Revision 2.60  2001/05/05 09:36:49  lewis
 *	added a few more predefined colors (by calling GetPaletteEntries(DEFAULT_PALETTE))- kTeal, etc
 *	
 *	Revision 2.59  2001/05/04 20:44:24  lewis
 *	SPR#0888- a lot of work on getting X fonts code working right.
 *	
 *	Revision 2.58  2001/05/04 17:38:25  lewis
 *	change API for HilightARectangle_SolidHelper - now takes fore/back hilgiht/normal colors (so in
 *	principle can exchange them all
 *	
 *	Revision 2.57  2001/04/30 22:32:20  lewis
 *	SPR#0886- added HilightARectangle_SolidHelper from TextImager class
 *	
 *	Revision 2.56  2001/04/30 21:11:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.55  2001/04/30 20:59:33  lewis
 *	move Win32CharSetToCodePage() from private to public (in .h file). Losely related to SPR#0891
 *	
 *	Revision 2.54  2001/04/30 19:16:33  lewis
 *	implement MoveTo/LineTo Led_Tablet_ APIs for XWindows
 *	
 *	Revision 2.53  2001/04/27 15:23:30  lewis
 *	hacks to get things sort of working for qXWindows (font hack)
 *	
 *	Revision 2.52  2001/04/26 20:55:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2001/04/25 22:43:25  lewis
 *	many changes - losely related to SPR#0879- much better support for XWindows. fGC/fColormap now fields of Led_Tablet_
 *	and auto-grabbed from window (not passed in as args)
 *	
 *	Revision 2.50  2001/04/23 16:38:45  lewis
 *	SPR#0876- Moved some utilities from TextImager to LedGDI
 *	
 *	Revision 2.49  2001/04/23 15:57:16  lewis
 *	Lose qDoTrapCaching- flag - since 68K mac so old/unimportant
 *	
 *	Revision 2.48  2001/04/18 23:53:11  lewis
 *	SPR#0874- fix bug with Led_Tablet::CvtToTWIPSV () etc. which caused tabs to not work properly in LedLineItMFC
 *	
 *	Revision 2.47  2001/04/14 16:49:47  lewis
 *	SPR#0868- Use LPtoDP/DPtoLP in CvtTWIPS routines - to fix printing problem
 *	
 *	Revision 2.46  2001/01/03 14:47:43  Lewis
 *	Support compiling with STRICT off(default for BorlandC++)
 *	
 *	Revision 2.45  2000/11/17 17:18:42  lewis
 *	get rid of a couple innocuous compiler warnings Remo complained about (he compiles with different warning settings)
 *	
 *	Revision 2.44  2000/10/16 00:14:27  lewis
 *	Added Led_CenterWindowInParent() helper
 *	
 *	Revision 2.43  2000/09/25 21:56:27  lewis
 *	Led_MacPortAndClipRegionEtcSaver now saves RGBColor as well
 *	
 *	Revision 2.42  2000/09/25 00:42:44  lewis
 *	lose Led_Tablet_ (CGrafPort* gp) overload cuz causes ambiguity when you pass NULL
 *	
 *	Revision 2.41  2000/09/05 23:25:30  lewis
 *	cache the XFontStruct in the Led_Tablet. Tried to see if we could get rid of the fFOntID crap - but some wierd
 *	X-bug pretents it. DEbug later.(XWINDOWS)
 *	
 *	Revision 2.40  2000/09/05 21:25:32  lewis
 *	add Prelim version of Get/SetFont methods for Led_Tablet on X-Windows
 *	
 *	Revision 2.39  2000/09/01 00:53:00  lewis
 *	small tweeks to get compiling under XWindows
 *	
 *	Revision 2.38  2000/09/01 00:26:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.37  2000/08/31 23:09:53  lewis
 *	more Led_Pen fixups to compile on mac
 *	
 *	Revision 2.36  2000/08/31 21:52:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/08/31 20:45:20  lewis
 *	fix a little more of the Led_Pen/MacOS code - but still not done
 *	
 *	Revision 2.34  2000/08/31 20:04:29  lewis
 *	Led_Pen (prelim) and Led_GDI_Obj_Selector support for qMacOS
 *	
 *	Revision 2.33  2000/08/28 13:08:24  lewis
 *	Added operator* (Led_Color lhs, float factor)/operator/operator+
 *	
 *	Revision 2.32  2000/08/13 03:35:25  lewis
 *	Added Led_Tablet_::MoveTo/Led_Tablet_::LineTo
 *	
 *	Revision 2.31  2000/07/09 04:30:45  lewis
 *	Added Led_CloneDB function
 *	
 *	Revision 2.30  2000/07/08 17:21:13  lewis
 *	minor Win32 GDI helper fixups (asserts etc)
 *	
 *	Revision 2.29  2000/06/13 20:03:04  lewis
 *	cleanups - and get compiling on GCC/Linux. And change Led_TWIPS to use a long - instead of an 'int'
 *	
 *	Revision 2.28  2000/06/13 19:54:52  lewis
 *	get compiling on GCC/linux
 *	
 *	Revision 2.27  2000/06/13 16:38:12  lewis
 *	SPR#0781- Lose Led_CvtFromTWIPSV () global functions etc
 *	
 *	Revision 2.26  2000/06/13 16:25:15  lewis
 *	Added Led_CvtScreenPixelsToTWIPSV ETC family of functions (See spr#0786). And changed Led_TWIPS to be based
 *	on signed in - not unsigned int
 *	
 *	Revision 2.25  2000/06/13 15:15:31  lewis
 *	Added Led_Tablet_::CvtToTWIPSH/Led_Tablet_::CvtToTWIPSV methods
 *	
 *	Revision 2.24  2000/06/13 14:12:02  lewis
 *	cleanup - and make Led_Tablet_::CvtFromTWIPSXXX() arg a Led_TWIPS - instead of int
 *	
 *	Revision 2.23  2000/06/13 00:55:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  2000/06/12 22:49:01  lewis
 *	SPR#0767- more work converting stuff (\fi\ri\li\margX etc) to TWIPS - instead of pixels. Tested a snapshot
 *	at this point - and LedItMFC seemed to work OK (thats all I've tested so far)
 *	
 *	Revision 2.21  2000/06/12 19:59:36  lewis
 *	Led_LineSpacing default CTOR - and map some sizes to eSingleSpace ETC. And op==/!= for Led_LineSpacing
 *	
 *	Revision 2.20  2000/06/12 17:37:19  lewis
 *	Added Led_LineSpacing - part of SPR#0760
 *	
 *	Revision 2.19  2000/06/12 16:18:25  lewis
 *	SPR#0780- Use new Led_Tablet_ common class for MacOS too. And start to move stuff like GDI_X stuff to be methods
 *	of that class and Led_Cvt global functions too
 *	
 *	Revision 2.18  2000/04/16 13:58:54  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.17  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.16  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.15  2000/04/04 15:11:40  lewis
 *	Added Led_InstalledFonts () helper class to share some code among the various apps - and make
 *	filtering by various contraints options
 *	
 *	Revision 2.14  2000/03/29 05:15:16  lewis
 *	experimental new qUseNewIMECode IME fixes. Must test at LEC. Basicly - use newer (documented) versions of
 *	the APIs since the old ones seem to sometimes mysteriously fail on Win2k
 *	
 *	Revision 2.13  2000/03/14 00:43:36  lewis
 *	fix typo
 *	
 *	Revision 2.12  2000/03/14 00:05:01  lewis
 *	SPR#0713- make Led_CvtToTWIPS () etc use SIGNED int - not UNSIGNED (Led_Coordinate vs. Led_Distance)
 *	
 *	Revision 2.11  2000/03/08 13:19:42  lewis
 *	Preliminary qXWindows hacks - so we can get HelloWorld displayed
 *	
 *	Revision 2.10  1999/12/24 02:19:46  lewis
 *	add OWNSDC attribute of Win32 Led_Tablet_ class
 *	
 *	Revision 2.9  1999/12/21 20:51:51  lewis
 *	Added Led_DIBFromHBITMAP () - for SPR# 0672
 *	
 *	Revision 2.8  1999/12/14 21:24:29  lewis
 *	Add Led_IME::GetCompositionResultStringW()
 *	
 *	Revision 2.7  1999/12/14 20:02:44  lewis
 *	Moved Led_IME code from LedSupport to LedGDI
 *	
 *	Revision 2.6  1999/12/14 18:01:17  lewis
 *	Add preliminary qXWindows support
 *	
 *	Revision 2.5  1999/12/09 03:19:11  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char
 *	
 *	Revision 2.4  1999/11/29 20:46:24  lewis
 *	Added Led_GetCurrentGDIPort () MacOnly wrapper (part of spr#0634). Call this instead of
 *	peeking at QDGlobals variable
 *	
 *	Revision 2.3  1999/11/28 17:09:03  lewis
 *	fix typo in Led_Region::CombineRgn() method - wrong args - caused D&D to silently fail
 *	
 *	Revision 2.2  1999/11/28 16:49:09  lewis
 *	allow (optional) HDC arg to (Win32) Led_Tablet_
 *	
 *	Revision 2.1  1999/11/15 21:20:32  lewis
 *	Moved some stuff here from LedSupport - in prep for X-Windows port, and to allow building for
 *	Win32 with no MFC dependencies. Re-implemented Led_Tablet (for Win32) ETC - - mimicing MFC versions -
 *	but not calling them - so that can build for Win32 not including MFC
 *	
 *
 *
 *
 *	<========== CODE MOVED HERE FROM LedSupport.h ==========>
 *
 */

#if		_MSC_VER == 1200
	//A bit of a hack for MSVC60, cuz this needs to be done before including <vector> - otherwise we get
	// lots of needless warnigns - regardless of what is done later -- LGP 980925
	#pragma	warning (4 : 4786)
#endif

#include	<cmath>
#include	<cstdlib>
#include	<map>
#include	<string>
#include	<vector>

#include	"LedSupport.h"





// WHY DONT WE NEED TO INCLUDE <Windows.h> here??? - LGP 991213

#if		qXWindows
	#include	<X11/Xlib.h>
	#include	<X11/Xutil.h>
#endif


#if		qLedUsesNamespaces
namespace	Led {
#endif





/*
@CONFIGVAR:		qWorkAroundWin95UNICODECharImagingBugs
@DESCRIPTION:	<p>Contrary to the MSDEV 50 online docs, many of the Win32 UNICODE versions of GDI functions don't work,
	or worse - only half-way work.</p>
		<p>For example, GetTextExtentExPointW DOESN'T WORK AT ALL, TextOutW works pretty well, but draws
	underlines the wrong width...(for proportional fonts).</p>
		<p>Turning this feature on makes the code work under Win95/Win98 and NT, but makes the NT code a little slower.</p>
		<p>NB: This has NO EFFECT unless you have built for UNICODE (@'qWideCharacters').</p>
		<p>Turn ON by default.</p>
 */
#ifndef	qWorkAroundWin95UNICODECharImagingBugs
	#define	qWorkAroundWin95UNICODECharImagingBugs	1
#endif






/*
@CONFIGVAR:		qWorkAroundWin98UNICODECharImagingBugs
@DESCRIPTION:	<p>See @'qWorkAroundWin95UNICODECharImagingBugs.</p>
		<p>This is very similar, but a strict subset of the problems in @'qWorkAroundWin95UNICODECharImagingBugs'.
	In other words, some were fixed, and some weren't.</p>
		<p>Turn ON by default.</p>
 */
#ifndef	qWorkAroundWin98UNICODECharImagingBugs
	#define	qWorkAroundWin98UNICODECharImagingBugs	1
#endif













/*
@CLASS:			Led_Coordinate
@DESCRIPTION:	<p><code>Led_Coordinate</code> is the <code>signed</code> analog of @'Led_Distance'.
	@'Led_Point' is a tuple of <code>Led_Coordinates</code>s
	(vertical and horizontal).</p>
*/
typedef	long			Led_Coordinate;

/*
@CLASS:			Led_Distance
@DESCRIPTION:	<p><code>Led_Distance</code> is an unsigned type, specifying the distance (always non-negative)
	between two graphics locations. @'Led_Size' is a tuple of <code>Led_Distance</code>s
	(vertical and horizontal).</p>
*/
typedef	unsigned long	Led_Distance;








/*
@CLASS:			Led_TWIPS
@DESCRIPTION:	<p>Many distances are specified in Led in TWIPS - 1/20 of a printers point.
	This means - 1/1440 of an inch.</p>
		<p>This size refers to the size when printed on a page. So it can be scaled - depnding on screen resolution.
	Led will often save this internally - and scale it at the last minute to the resolution of the @'Led_Tablet' being printed on.</p>
		<p>NB: This marks a change from Led 2.3 and earlier - where most distances were stored in pixels (still many are).</p>
		<p>NB: declard as a class instead of a typedef so we get better type checking. Shouldn't affect sizes or code
	generation - I would hope!</p>
*/
class	Led_TWIPS {
	public:
		explicit Led_TWIPS (long v);
		operator long () const;
	private:
		long	fValue;

	public:
		nonvirtual	Led_TWIPS& operator+= (const Led_TWIPS& rhs);
		nonvirtual	Led_TWIPS& operator-= (const Led_TWIPS& rhs);
		nonvirtual	Led_TWIPS& operator*= (double rhs);

	public:
		static	const	Led_TWIPS	kPoint;
		static	const	Led_TWIPS	kInch;
		static	const	Led_TWIPS	kOneInch;
};

Led_TWIPS	operator+ (const Led_TWIPS& lhs, const Led_TWIPS& rhs);
Led_TWIPS	operator- (const Led_TWIPS& lhs, const Led_TWIPS& rhs);




class	Led_Tablet_;


/*
@CLASS:			Led_TabStopList
@DESCRIPTION:	<p>Even though WinSDK supports GetTabbedTextExtent/TabbedTextOut () - they do a bad job.
	In particular, they offer no (obvious) way to specify the tab origin for
	GetTabbedTextExtent(). This makes proper text display nearly impossible.</p>
		<p>Also, the Mac provides NO tab support at all. Because of the Mac - we NEED
	to implement our own tab support. Given we are doing that anyhow, may as
	well use our portable support on both platforms.</p>
		<p>For both calculating widths, and doing imaging, we follow essentially
	the same algorithm.
	<ul>
		<li>	Walk the text looking for a tab or EOL.
		<li>	Take the tab-free initial segment and image(calculate) it.
		<li>	if now on a tab, image (calculate) that.
		<li>	Advance indexes over what we've imaged (calculated) and repeat.
	</ul>
	</p>
*/
class	Led_TabStopList {
	protected:
		Led_TabStopList ();
	public:
		virtual ~Led_TabStopList ();
	public:
		/*
		@METHOD:		TextImager::TabStopList::ComputeIthTab
		@DESCRIPTION:	<p>Compute where the 'ith' tabstop lies (i >= 0, and can be arbitrarily high).
			Note that though tabstop <code>i=0</code> is the first tabstop (which by convention is usually NOT
			at the left margin - but is really the logically 'first' one).</p>
				<p>Note - this used to return Led_Distance - but in Led 3.0 - it changed to returning TWIPS (see SPR#0767);</p>
		*/
		virtual		Led_TWIPS		ComputeIthTab (size_t i) const								=	0;
		/*
		@METHOD:		TextImager::TabStopList::ComputeTabStopAfterPosition
		@DESCRIPTION:	<p>Given a distance from the left margin, find the immediately following tabstops position.
			This is what is actaully used in the imagers to compute where to draw text. It can logically
			be implemented by walking the tabstoplist and calling @'TextImager::TabStopList::ComputeIthTab', and
			returning the next one after the one past(or equal to) <code>afterPos</code>.</p>
				<p>Note - this used to take/return Led_Distance - but in Led 3.0 - it changed to take/return TWIPS (see SPR#0767);</p>
				<p>Now there is an overloaded version using Led_Distance that takes a Led_Tablet as argument.</p>
		*/
		virtual		Led_TWIPS		ComputeTabStopAfterPosition (Led_TWIPS afterPos) const		=	0;
		nonvirtual	Led_Distance	ComputeTabStopAfterPosition (Led_Tablet_* tablet, Led_Distance afterPos) const;
};











#if		qMacOS

	inline	GrafPtr	Led_GetCurrentGDIPort ()
		{
			#if		qPeekAtQuickDrawGlobals
				GrafPtr	t	=	qd.thePort;
			#else
				// This used to (and perhaps could in ifdefs) return qd.thePort - 
				GrafPtr	t	=	NULL;
				::GetPort (&t);
			#endif
			Led_EnsureNotNil (t);
			return t;
		}

#elif	qWindows




class	Led_FontObject {
	public:
		Led_FontObject ():
			m_hObject (NULL)
			{
			}
		~Led_FontObject()
			{
				(void)DeleteObject ();
			}
		nonvirtual	operator HFONT() const
			{
				return m_hObject;
			}
		nonvirtual	int	GetObject (int nCount, LPVOID lpObject) const
			{
				Led_Assert (m_hObject != NULL);
				return ::GetObject(m_hObject, nCount, lpObject);
			}
		nonvirtual	BOOL DeleteObject ()
			{
				if (m_hObject == NULL)
					return FALSE;
				HFONT	h	=	m_hObject;
				m_hObject = NULL;
				return ::DeleteObject (h);
			}
		nonvirtual	BOOL	CreateFontIndirect (const LOGFONT* lpLogFont)
			{
				return Attach (::CreateFontIndirect(lpLogFont));
			}
		nonvirtual	BOOL Attach (HFONT hObject)
			{
				Led_Assert (m_hObject == NULL);      // only attach once, detach on destroy
				if (hObject == NULL)
					return FALSE;
				m_hObject = hObject;
				return TRUE;
			}
	public:
		HFONT	m_hObject;
};




class	Led_Brush {
	public:
		Led_Brush (COLORREF crColor):
			m_hObject (NULL)
			{
				if (!Attach(::CreateSolidBrush(crColor)))
					Led_ThrowOutOfMemoryException ();
			}
		~Led_Brush ()
			{
				(void)DeleteObject ();
			}
		nonvirtual	operator HBRUSH() const
			{
				return m_hObject;
			}
		nonvirtual	BOOL Attach (HBRUSH hObject)
			{
				Led_Assert (m_hObject == NULL);      // only attach once, detach on destroy
				if (hObject == NULL)
					return FALSE;
				m_hObject = hObject;
				return TRUE;
			}
		nonvirtual	BOOL DeleteObject ()
			{
				if (m_hObject == NULL)
					return FALSE;
				HBRUSH	h	=	m_hObject;
				m_hObject = NULL;
				return ::DeleteObject (h);
			}
	private:
		HBRUSH	m_hObject;
};

#endif





/*
@CLASS:			Point_Base<COORD_TYPE>
@DESCRIPTION:	
*/
template	<typename	COORD_TYPE>
	struct	Point_Base {
		public:
			typedef	COORD_TYPE	CoordinateType;

		public:
			Point_Base ();
			Point_Base (COORD_TYPE newV, COORD_TYPE newH);
			#if		!qNestedTemplateCTORInTemplateBug
				template	<typename	OTHER_POINT_BASE_TYPE>
					explicit Point_Base (OTHER_POINT_BASE_TYPE o):
						v (COORD_TYPE (o.v)),
						h (COORD_TYPE (o.h))
						{
						}
			#endif

		public:
			COORD_TYPE	v;
			COORD_TYPE	h;
	};
template	<typename	COORD_TYPE>
	bool	operator== (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	bool	operator!= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	bool	operator<  (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	bool	operator<= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	bool	operator>  (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	bool	operator>= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
template	<typename	COORD_TYPE>
	Point_Base<COORD_TYPE>	operator+  (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);







/*
@CLASS:			Rect_Base
@DESCRIPTION:	
*/
template	<typename POINT_TYPE, typename SIZE_TYPE>
	struct	Rect_Base  {
		public:
			typedef	typename	POINT_TYPE::CoordinateType		CoordinateType;
			typedef typename	SIZE_TYPE::CoordinateType		DistanceType;
			typedef				Rect_Base<POINT_TYPE,SIZE_TYPE>	THIS_TYPE;

		public:
			Rect_Base ();
			Rect_Base (CoordinateType newTop, CoordinateType newLeft, DistanceType newHeight, DistanceType newWidth);
			Rect_Base (POINT_TYPE origin, SIZE_TYPE size);

		public:
			nonvirtual	CoordinateType	GetTop () const;
			nonvirtual	CoordinateType	GetLeft () const;
			nonvirtual	CoordinateType	GetBottom () const;
			nonvirtual	CoordinateType	GetRight () const;
			nonvirtual	bool			IsEmpty () const;		/* contains no bits - ie one side <= zero length */
			nonvirtual	DistanceType	GetHeight () const;
			nonvirtual	DistanceType	GetWidth () const;
			nonvirtual	POINT_TYPE		GetTopLeft () const;
			nonvirtual	POINT_TYPE		GetTopRight () const;
			nonvirtual	POINT_TYPE		GetBotRight () const;
			nonvirtual	POINT_TYPE		GetOrigin () const;
			nonvirtual	SIZE_TYPE		GetSize () const;
			nonvirtual	bool			Contains (POINT_TYPE p) const;
			nonvirtual	bool			Contains (THIS_TYPE r) const;

			nonvirtual	void			SetOrigin (POINT_TYPE p);
			nonvirtual	void			SetTop (CoordinateType top);
			nonvirtual	void			SetLeft (CoordinateType left);
			nonvirtual	void			SetBottom (CoordinateType bottom);
			nonvirtual	void			SetRight (CoordinateType right);

		public:
			CoordinateType	top;
			CoordinateType	left;
			CoordinateType	bottom;
			CoordinateType	right;

		public:
			nonvirtual	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	operator+= (const POINT_TYPE& delta);
			nonvirtual	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	operator-= (const POINT_TYPE& delta);
			nonvirtual	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	operator*= (const THIS_TYPE& intersectWith);
	};

template	<typename POINT_TYPE, typename SIZE_TYPE>
	bool	operator== (const Rect_Base<POINT_TYPE,SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE,SIZE_TYPE>& rhs);
template	<typename POINT_TYPE, typename SIZE_TYPE>
	bool	operator!= (const Rect_Base<POINT_TYPE,SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE,SIZE_TYPE>& rhs);






#if		qNestedTemplateCTORInTemplateBug
	struct	Led_Point;
	struct Led_Size : public Point_Base<Led_Distance> {
		typedef Point_Base<Led_Distance> inherited;
		inline Led_Size  (): inherited () {}
		inline Led_Size  (inherited i): inherited (i) {}
		inline Led_Size  (Led_Distance newV, Led_Distance newH): inherited (newV, newH) {}
		Led_Size (Led_Point p);
	};
#else
	/*
	@CLASS:			Led_Size
	@DESCRIPTION:	<p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_Distance'.</p>
	*/
	typedef	Point_Base<Led_Distance>	Led_Size;
#endif


/*
@CLASS:			Led_Point
@DESCRIPTION:	<p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_Coordinate'.</p>
*/
#if		qNestedTemplateCTORInTemplateBug
	struct Led_Point : public Point_Base<Led_Coordinate> {
		typedef Point_Base<Led_Coordinate> inherited;
		inline Led_Point  (): inherited () {}
		inline Led_Point  (inherited i): inherited (i) {}
		inline Led_Point  (Led_Coordinate newV, Led_Coordinate newH): inherited (newV, newH) {}
		Led_Point (Led_Size p);
	};
	inline Led_Size::Led_Size (Led_Point p): inherited (p.v, p.h) {}
	inline Led_Point::Led_Point (Led_Size p): inherited (p.v, p.h) {}
#else
	typedef	Point_Base<Led_Coordinate>	Led_Point;
#endif
Led_Point	operator-  (const Led_Point& lhs, const Led_Point& rhs);


/*
@CLASS:			Led_TWIPS_Point
@DESCRIPTION:	<p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_TWIPS'.</p>
*/
typedef	Point_Base<Led_TWIPS>	Led_TWIPS_Point;





/*
@CLASS:			Led_Rect
@DESCRIPTION:	<p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'Led_Point' and @'Led_Size'.</p>
*/
typedef	Rect_Base<Led_Point,Led_Size>	Led_Rect;


/*
@CLASS:			Led_TWIPS_Rect
@DESCRIPTION:	<p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'Led_TWIPS_Point'.</p>
*/
typedef	Rect_Base<Led_TWIPS_Point,Led_TWIPS_Point>	Led_TWIPS_Rect;




#if		qWindows
/*
@CLASS:			auto_gdi_ptr
@DESCRIPTION:		<p>Like auto_ptr<> - except to just assure a Win32 GDI object is deleted. NB: We cannot
				use a template specialization of auto_ptr<> because HGDIOBJ is really just a simple typedef
				(void*) - and so we would be specializing the deletion of any void* typedef (more than
				really desired and less than safe).</p>
*/
class	auto_gdi_ptr {
	public:
		auto_gdi_ptr (HGDIOBJ gdiObj);
		~auto_gdi_ptr ();
	private:
		HGDIOBJ	fGDIObj;
};
#endif



/*
@CLASS:			Led_Region
@DESCRIPTION:	<p>Portable GDI abstraction for 'Region' object.</p>
*/
class	Led_Region  {
	public:
		Led_Region ();
		Led_Region (const Led_Rect& r);
		Led_Region (const Led_Region& from);
		virtual ~Led_Region ();
		nonvirtual	const Led_Region& operator= (const Led_Region& rhs);


	public:
		nonvirtual	bool		IsEmpty () const;
		nonvirtual	Led_Rect	GetBoundingRect () const;

	public:
		#if		qMacOS
			Led_Region (RgnHandle rgn):
				fRgn (rgn),
				fOwned (false)
				{
				}
			RgnHandle	GetOSRep () const
				{
					return fRgn;
				}
			RgnHandle	GetOSRep ()
				{
					return fRgn;
				}

		private:
			bool	fOwned;
		#elif	qWindows
			operator HRGN() const
				{
					return fRgn;
				}
			int CombineRgn(Led_Region* pRgn1, Led_Region* pRgn2, int nCombineMode)
				{
					Led_Require (pRgn1 != NULL); 
					Led_Require (pRgn2 != NULL); 
					Led_Require (fRgn != NULL); 
					return ::CombineRgn (fRgn, pRgn1->fRgn, pRgn2->fRgn, nCombineMode); 
				}
			BOOL	PtInRegion (int x, int y) const
				{
					Led_Require (fRgn != NULL);
					return ::PtInRegion (fRgn, x, y);
				}
			BOOL PtInRegion(POINT point) const
				{
					Led_Require (fRgn != NULL);
					return ::PtInRegion (fRgn, point.x, point.y);
				}
		private:
			BOOL DeleteObject ()
				{
					if (fRgn == NULL)
						return FALSE;
					HRGN	r	=	fRgn;
					fRgn = NULL;
					return ::DeleteObject (r);
				}
		#endif
	private:
		#if		qMacOS
			RgnHandle	fRgn;
		#elif	qWindows
			HRGN	fRgn;
		#endif
};

Led_Region	operator* (const Led_Region& lhs, const Led_Region& rhs);
Led_Region	operator+ (const Led_Region& lhs, const Led_Region& rhs);







/*
@CLASS:			Led_Color
@DESCRIPTION:	<p>This class is a portable representation of a color. It can be constructed either
			from its basic RGB componets, or from the native color representations on a particular platform.</p>
*/
class	Led_Color {
	public:
		// regardless of Mac or Windows, we use the same size ColorValue so we can write
		// portable code more easily...
		typedef	unsigned short	ColorValue;
		enum { kColorValueMin = 0, kColorValueMax = USHRT_MAX };
	public:
		explicit	Led_Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue);
		#if		qMacOS
			explicit	Led_Color (const RGBColor& rgbColor);
		#elif	qWindows
			explicit	Led_Color (COLORREF colorRef);
		#endif

	public:
		nonvirtual	ColorValue	GetRed () const;
		nonvirtual	ColorValue	GetGreen () const;
		nonvirtual	ColorValue	GetBlue () const;


	/*
	 *	Some useful predefined values.
	 */
	public:
		static	const	Led_Color	kBlack;
		static	const	Led_Color	kWhite;
		static	const	Led_Color	kRed;
		static	const	Led_Color	kGreen;
		static	const	Led_Color	kBlue;
		static	const	Led_Color	kCyan;
		static	const	Led_Color	kMagenta;
		static	const	Led_Color	kYellow;
		static	const	Led_Color	kMaroon;
		static	const	Led_Color	kOlive;
		static	const	Led_Color	kNavyBlue;
		static	const	Led_Color	kPurple;
		static	const	Led_Color	kTeal;
		static	const	Led_Color	kGray;
		static	const	Led_Color	kSilver;		// aka 'light gray'
		static	const	Led_Color	kDarkGreen;
		static	const	Led_Color	kLimeGreen;
		static	const	Led_Color	kFuchsia;
		static	const	Led_Color	kAqua;

	public:
		#if		qMacOS
			nonvirtual	RGBColor	GetOSRep () const;
		#elif	qWindows
			nonvirtual	COLORREF	GetOSRep () const;
		#endif
	private:
		ColorValue	fRed;
		ColorValue	fGreen;
		ColorValue	fBlue;

	STLDefCTORDeclare_BWA(Led_Color)
};
bool	operator== (Led_Color lhs, Led_Color rhs);
bool	operator!= (Led_Color lhs, Led_Color rhs);

Led_Color	operator* (Led_Color lhs, float factor);
Led_Color	operator/ (Led_Color lhs, float divBy);
Led_Color	operator+ (Led_Color lhs, Led_Color rhs);
Led_Color	operator- (Led_Color lhs, Led_Color rhs);

unsigned int	Distance (Led_Color lhs, Led_Color rhs);
unsigned int	Distance_Squared (Led_Color lhs, Led_Color rhs);
#if		qWindows
unsigned int	Distance_Squared (COLORREF lhs, COLORREF rhs);
#endif





/*
@CLASS:			Led_Pen
@DESCRIPTION:	<p>Helper class to keep track of GDI information used for drawing.
			Very different implementations befween Mac and Windows.</p>
				<p>Note - this class is used in conjunction with @'Led_GDI_Obj_Selector'.</p>
*/
class	Led_Pen {
#if		qWindows
	public:
		Led_Pen (int nPenStyle, int nWidth, COLORREF crColor):
			m_hObject (NULL)
			{
				if (!Attach (::CreatePen (nPenStyle, nWidth, crColor)))
					Led_ThrowOutOfMemoryException ();
			}
		~Led_Pen ()
			{
				(void)DeleteObject ();
			}
		nonvirtual	operator HPEN () const
			{
				return m_hObject;
			}
		nonvirtual	BOOL Attach (HPEN hObject)
			{
				Led_Assert (m_hObject == NULL);      // only attach once, detach on destroy
				if (hObject == NULL)
					return FALSE;
				m_hObject = hObject;
				return TRUE;
			}
		nonvirtual	BOOL DeleteObject ()
			{
				if (m_hObject == NULL)
					return FALSE;
				HPEN	h	=	m_hObject;
				m_hObject = NULL;
				return ::DeleteObject (h);
			}
	private:
		HPEN	m_hObject;
#endif
#if		qMacOS
	public:
		static	const	Pattern	kWhitePattern;
		static	const	Pattern	kLightGrayPattern;
		static	const	Pattern	kGrayPattern;
		static	const	Pattern	kDarkGrayPattern;
		static	const	Pattern	kBlackPattern;
	public:
		Led_Pen (short penStyle = srcCopy, const Pattern* penPat= &kBlackPattern, const Led_Color& color = Led_Color::kBlack):
			fPenStyle (penStyle),
			fPenPat (*penPat),
			fPenColor (color)
			{
			}
	public:
		short		fPenStyle;
		Pattern		fPenPat;
		Led_Color	fPenColor;
#endif
#if		qXWindows
	public:
		Led_Pen () {}
#endif
};






/*
@CLASS:			Led_Justification
@DESCRIPTION:	<p>Led defines several kinds of justification, but doesn't implement all of them.
	<ul>
		<li><em>eLeftJustify</em></li>
		<li><em>eRightJustify</em></li>
		<li><em>eCenterJustify</em></li>
		<li><em>eFullyJustify</em>-	not implemented in Led 3.0 - treated as eLeftJustify</li>
		<li><em>eDefaultForScriptJustify</em>-not implemented in Led 3.0 - treated as eLeftJustify</li>
	</ul>
	</p>
		<p>These extra unimplemented enums are provided so you can more easily write code and read/write files
	etc which keep track of this information, and even show the styles in the UI. They just aren't reflected
	in how the text is drawn yet. That should come in the next major Led release.</p>
*/
enum	Led_Justification { eLeftJustify, eRightJustify, eCenterJustify, eFullyJustify, eDefaultForScriptJustify };






/*
@CLASS:			TextDirection
@DESCRIPTION:	<p>There are two defined text directions:
	<ul>
		<li><em>eLeftToRight</em></li>
		<li><em>eRightToLeft</em></li>
	</ul>
	</p>
*/
enum	TextDirection { eLeftToRight, eRightToLeft };



/*
@CLASS:			ListStyle
@DESCRIPTION:	<p>Different styles of bullet / list markers for list items in the WordProcessor.</p>

		<p>Numeric values come from RTF 1.5 Spec \levelnfc (except for eListStyle_None which is special)</p>
*/
enum	ListStyle {
	eListStyle_None = 9999,
	eListStyle_Bullet = 23
};





/*
@CLASS:			Led_LineSpacing
@DESCRIPTION:	<p>Support at least all the crazy formats/options in the Win32 PARAFORMAT2 structure, and
	the ill-documented RTF 1.5 SPEC \sl options.
		<ul>
				<li>eSingleSpace-	fArg IGNORED
				<li>eOnePointFiveSpace-	fArg IGNORED
				<li>eDoubleSpace-	fArg IGNORED
				<li>eAtLeastTWIPSSpacing-<br>
					The fArg member specifies the spacing from one line to the next, in twips.
					However, if fArg specifies a value that is less than single line spacing, Led displays single-spaced text. 
				<li>eExactTWIPSSpacing-<br>
					The fArg member specifies the spacing from one row to the next, in twips.
					Led uses the exact spacing specified, even if fArg specifies a value that is less than single spacing. 
				<li>eExactLinesSpacing-<br>
					The value of fArg / 20 is the spacing, in lines, from one line to the next.
					Thus, setting fArg to 20 produces single-spaced text, 40 is double spaced, 60 is triple spaced, and so on. 
		</ul>
		</p>
*/
class	Led_LineSpacing {
	public:
		enum Rule {
				eSingleSpace,
				eOnePointFiveSpace,
				eDoubleSpace,
				eAtLeastTWIPSSpacing,
				eExactTWIPSSpacing,
				eExactLinesSpacing
		};
		Rule		fRule;
		unsigned	fArg;
	public:
		Led_LineSpacing ():
			fRule (eSingleSpace),
			fArg (0)
			{
			}
		Led_LineSpacing (Rule rule):
			fRule (rule),
			fArg (0)
			{
				Led_Require (rule == eSingleSpace or rule == eOnePointFiveSpace or rule == eDoubleSpace);
			}
		Led_LineSpacing (Rule rule, Led_TWIPS twips):
			fRule (rule),
			fArg (twips)
			{
				Led_Require (rule == eAtLeastTWIPSSpacing or rule == eExactTWIPSSpacing);
			}
		Led_LineSpacing (Rule rule, unsigned lineCount):
			fRule (rule),
			fArg (lineCount)
			{
				Led_Require (rule == eExactLinesSpacing);
				switch (lineCount) {
					case	20:		fRule = eSingleSpace;	break;
					case	30:		fRule = eOnePointFiveSpace;	break;
					case	40:		fRule = eDoubleSpace;	break;
				}
			}
};
inline	bool operator== (Led_LineSpacing lhs, Led_LineSpacing rhs)
	{
		if (lhs.fRule != rhs.fRule) {
			return false;
		}
		if (lhs.fRule == Led_LineSpacing::eAtLeastTWIPSSpacing or lhs.fRule == Led_LineSpacing::eExactTWIPSSpacing or lhs.fRule == Led_LineSpacing::eExactLinesSpacing) {
			if (lhs.fArg != rhs.fArg) {
				return false;
			}
		}
		return true;
	}
inline	bool operator!= (Led_LineSpacing lhs, Led_LineSpacing rhs)
	{
		return not (lhs == rhs);
	}







class	Led_IncrementalFontSpecification;
/*
@CLASS:			Led_FontSpecification
@DESCRIPTION:	<p><code>Led_FontSpecification</code> is a utility class which portably represents
	a user font choice. This largely corresponds to the MS-Windows <code>LOGFONT</code> structure
	or the Macintosh <code>txFace, txSize, txStyle</code>.</p>
		<p>In addition to being a portable represenation of this information, it
	also contains handy wrapper accessors, and extra information like subscript,
	superscript, and font color.</p>
		<p>See also, @'Led_IncrementalFontSpecification'</p>
*/
class	Led_FontSpecification {
	public:
		Led_FontSpecification ();
		#if		qWindows
		explicit Led_FontSpecification (const LOGFONT& logFont);
		#endif

	// Force users to be EXPLICIT about this object-slicing, since many of the fields
	// maybe invalid... Will the compiler REALLY do this check???? We'll see - LGP 970314
	public:
		explicit	Led_FontSpecification (const Led_IncrementalFontSpecification& from);

	public:
		#if		qMacOS
			typedef	short	FontNameSpecifier;
		#elif	qWindows
			struct	FontNameSpecifier {		// So struct copies etc will work and so we can define op==
				FontNameSpecifier ();
				FontNameSpecifier (const Led_SDK_Char* from);
				Led_SDK_Char	fName [LF_FACESIZE];
			};
		#elif	qXWindows
			typedef	Led_SDK_String	FontNameSpecifier;
		#endif

	public:
		// string/name wrapper. Trivial for PC, and for Mac - converts between name under NUMBER ID
		nonvirtual	Led_SDK_String	GetFontName () const;
		nonvirtual	void			SetFontName (const Led_SDK_String& fontName);

		// FontName info
		nonvirtual	FontNameSpecifier	GetFontNameSpecifier () const;
		nonvirtual	void				SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier);

		// Style info
		nonvirtual	bool	GetStyle_Plain () const;							// special composite of all other styles...
		nonvirtual	void	SetStyle_Plain ();

		nonvirtual	bool	GetStyle_Bold () const;
		nonvirtual	void	SetStyle_Bold (bool isBold);

		nonvirtual	bool	GetStyle_Italic () const;
		nonvirtual	void	SetStyle_Italic (bool isItalic);

		nonvirtual	bool	GetStyle_Underline () const;
		nonvirtual	void	SetStyle_Underline (bool isUnderline);

		enum SubOrSuperScript { eSubscript, eSuperscript, eNoSubOrSuperscript };
		nonvirtual	SubOrSuperScript	GetStyle_SubOrSuperScript () const;
		nonvirtual	void				SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript);

		#if		qMacOS
			nonvirtual	bool	GetStyle_Outline () const;
			nonvirtual	void	SetStyle_Outline (bool isOutline);

			nonvirtual	bool	GetStyle_Shadow () const;
			nonvirtual	void	SetStyle_Shadow (bool isShadow);

			nonvirtual	bool	GetStyle_Condensed () const;
			nonvirtual	void	SetStyle_Condensed (bool isCondensed);

			nonvirtual	bool	GetStyle_Extended () const;
			nonvirtual	void	SetStyle_Extended (bool isExtended);
		#elif	qWindows
			nonvirtual	bool	GetStyle_Strikeout () const;
			nonvirtual	void	SetStyle_Strikeout (bool isStrikeout);
		#endif

		nonvirtual	unsigned short	GetPointSize () const;
		nonvirtual	void			SetPointSize (unsigned short pointSize);
		#if		qWindows
		nonvirtual	 long			PeekAtTMHeight () const;		// Speed tweek
		nonvirtual	 void			PokeAtTMHeight (long tmHeight);	// ditto
		#endif

		nonvirtual	Led_Color	GetTextColor () const;
		nonvirtual	void		SetTextColor (const Led_Color& textColor);

	public:
		#if		qMacOS
			nonvirtual	void	GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const;
			nonvirtual	void	SetOSRep (short fontID, short fontSize, Style fontStyle);
		#elif	qWindows
			nonvirtual	LOGFONT	GetOSRep () const;
			nonvirtual	void	GetOSRep (LOGFONT* logFont) const;
			nonvirtual	void	SetOSRep (LOGFONT logFont);
			nonvirtual	void	LightSetOSRep (LOGFONT logFont);
		#elif	qXWindows
			static		string	mkOSRep (const string& foundry, const string& family, const string& weight, const string& slant, const string& pointSize);
			nonvirtual	string	GetOSRep () const;
			nonvirtual	void	SetFromOSRep (const string& osRep);
		#endif

	public:
		nonvirtual	void	MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes);

	// OSRep.
	private:
		#if		qMacOS
			short				fFontSpecifier;
			short				fFontSize;
			Style				fFontStyle;
		#elif	qWindows
			LOGFONT				fFontInfo;			// Could make this MUCH smaller on windows - do for future release!
		#elif	qXWindows
			FontNameSpecifier	fFontFamily;
			bool				fBold:1;
			bool				fItalics:1;
			bool				fUnderline:1;
			short				fFontSize;
		#endif
		SubOrSuperScript	fSubOrSuperScript;
		Led_Color			fTextColor;
};
#if		qWindows
bool	operator== (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs);
bool	operator!= (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs);
#endif

bool	operator== (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs);
bool	operator!= (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs);





/*
@CLASS:			Led_IncrementalFontSpecification
@DESCRIPTION:	<p><code>Led_IncrementalFontSpecification</code> is a simple subclass of
	@'Led_FontSpecification' which adds a bool flag for each
	font attribute indicating whether or not it is really specified.
	With this, and the @'Led_FontSpecification::MergeIn' method,
	you can specify just one or two changes to a font record, pass them around,
	and apply them to an existing font choice.</p>
		<p>See also, @'Led_FontSpecification'</p>
*/
class	Led_IncrementalFontSpecification : public Led_FontSpecification {
	private:
		typedef	Led_FontSpecification	inherited;
	public:
		Led_IncrementalFontSpecification ();
		
		// I may end up regretting this, for all the confusion it
		// can cause, but it sure makes a number of things simpler
		// and clearer-- LGP 960520
		Led_IncrementalFontSpecification (const Led_FontSpecification& fontSpec);


	/*
	 *	Basic form of this API. For a feature of a font, say its size, there is a
	 *	GetFEATURE () -
	 *		REQUIRES FEATURE VALID
	 *	GetFEATURE (bool* valid, FEATUURE* feature)-
	 *		Either valid or feature can be NULL, but if NON-NULL filled in with
	 *		feature/valid (except feature NOT filled in if not valid.
	 *	InvalidateFEATURE ()-
	 *		Just sets valid-flag false.
	 *	SetFEATURE(FEATURE)-
	 *		Specifies this as the feature value, and sets the valid flag TRUE.
	 *
	 *	(NOTE: MAYBE simplify this with member templates if/when that supported by compilers? - LGP 970314)
	 */
	public:
		nonvirtual	FontNameSpecifier	GetFontNameSpecifier () const;
		nonvirtual	bool				GetFontNameSpecifier_Valid () const;
		nonvirtual	void				InvalidateFontNameSpecifier ();
		nonvirtual	void				SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier);
		nonvirtual	void				SetFontName (const Led_SDK_String& fontName);

		// Style info
		nonvirtual	bool	GetStyle_Plain () const;							// special composite of all other styles...
		nonvirtual	bool	GetStyle_Plain_Valid () const;
		nonvirtual	void	InvalidateStyles ();
		nonvirtual	void	SetStyle_Plain ();

		nonvirtual	bool	GetStyle_Bold () const;
		nonvirtual	bool	GetStyle_Bold_Valid () const;
		nonvirtual	void	InvalidateStyle_Bold ();
		nonvirtual	void	SetStyle_Bold (bool isBold);

		nonvirtual	bool	GetStyle_Italic () const;
		nonvirtual	bool	GetStyle_Italic_Valid () const;
		nonvirtual	void	InvalidateStyle_Italic ();
		nonvirtual	void	SetStyle_Italic (bool isItalic);

		nonvirtual	bool	GetStyle_Underline () const;
		nonvirtual	bool	GetStyle_Underline_Valid () const;
		nonvirtual	void	InvalidateStyle_Underline ();
		nonvirtual	void	SetStyle_Underline (bool isUnderline);

		nonvirtual	SubOrSuperScript	GetStyle_SubOrSuperScript () const;
		nonvirtual	bool				GetStyle_SubOrSuperScript_Valid () const;
		nonvirtual	void				InvalidateStyle_SubOrSuperScript ();
		nonvirtual	void				SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript);

		#if		qMacOS
			nonvirtual	bool	GetStyle_Outline () const;
			nonvirtual	bool	GetStyle_Outline_Valid () const;
			nonvirtual	void	InvalidateStyle_Outline ();
			nonvirtual	void	SetStyle_Outline (bool isOutline);

			nonvirtual	bool	GetStyle_Shadow () const;
			nonvirtual	bool	GetStyle_Shadow_Valid () const;
			nonvirtual	void	InvalidateStyle_Shadow ();
			nonvirtual	void	SetStyle_Shadow (bool isShadow);

			nonvirtual	bool	GetStyle_Condensed () const;
			nonvirtual	bool	GetStyle_Condensed_Valid () const;
			nonvirtual	void	InvalidateStyle_Condensed ();
			nonvirtual	void	SetStyle_Condensed (bool isCondensed);

			nonvirtual	bool	GetStyle_Extended () const;
			nonvirtual	bool	GetStyle_Extended_Valid () const;
			nonvirtual	void	InvalidateStyle_Extended ();
			nonvirtual	void	SetStyle_Extended (bool isExtended);
		#elif	qWindows
			nonvirtual	bool	GetStyle_Strikeout () const;
			nonvirtual	bool	GetStyle_Strikeout_Valid () const;
			nonvirtual	void	InvalidateStyle_Strikeout ();
			nonvirtual	void	SetStyle_Strikeout (bool isStrikeout);
		#endif

		/*
		 *	Note the closely related nature of PointSize and PointSizeIncement.
		 *	If one is set, that automatically unsets the other. You cannot set both
		 *	at the same time. The former is used to specify the point size exactly.
		 *	The latter is used to specify an ajdustment to the pointsize.
		 */
		nonvirtual	unsigned short	GetPointSize () const;
		nonvirtual	bool			GetPointSize_Valid () const;
		nonvirtual	void			InvalidatePointSize ();
		nonvirtual	void			SetPointSize (unsigned short pointSize);
		#if		qWindows
		nonvirtual	 void			PokeAtTMHeight (long tmHeight);	// ditto
		#endif

		nonvirtual	short 	GetPointSizeIncrement () const;
		nonvirtual	bool	GetPointSizeIncrement_Valid () const;
		nonvirtual	void	InvalidatePointSizeIncrement ();
		nonvirtual	void	SetPointSizeIncrement (short pointSizeIncrement);

		nonvirtual	Led_Color	GetTextColor () const;
		nonvirtual	bool		GetTextColor_Valid () const;
		nonvirtual	void		InvalidateTextColor ();
		nonvirtual	void		SetTextColor (const Led_Color& textColor);

	public:
		#if		qMacOS
			nonvirtual	void	GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const;
			nonvirtual	void	SetOSRep (short fontID, short fontSize, Style fontStyle);	// marks all attribs as valid
		#elif	qWindows
			nonvirtual	LOGFONT	GetOSRep () const;
			nonvirtual	void	GetOSRep (LOGFONT* logFont) const;
			nonvirtual	void	SetOSRep (LOGFONT logFont);									// marks all attribs as valid
			nonvirtual	void	LightSetOSRep (LOGFONT logFont);
			nonvirtual	bool	GetDidSetOSRepCallFlag () const;							// special flag indicating user did SetOSRep() cuz that
																							// sets things valid that we don't have special 'valid' flags
																							// for. Makes MergeIn() just copy flat out!
		#endif


	public:
		nonvirtual	void	MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes);

	private:
		bool	fFontSpecifierValid:1;
		bool	fStyleValid_Bold:1;
		bool	fStyleValid_Italic:1;
		bool	fStyleValid_Underline:1;
		bool	fStyleValid_SubOrSuperScript:1;
		#if		qMacOS
			bool	fStyleValid_Outline:1;
			bool	fStyleValid_Shadow:1;
			bool	fStyleValid_Condensed:1;
			bool	fStyleValid_Extended:1;
		#elif	qWindows
			bool	fStyleValid_Strikeout:1;
			bool	fDidSetOSRepCallFlag:1;
		#endif
		bool	fFontSizeValid:1;
		bool	fFontSizeIncrementValid:1;
		bool	fTextColorValid:1;
};

bool	operator== (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);
bool	operator!= (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);

Led_IncrementalFontSpecification	Intersection (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);


// Must support for other Platforms - but not done yet... Also - should have OPTIONS specified to this class (CTOR) picking various filter
// options...
class	Led_InstalledFonts {
	public:
		enum	FilterOptions {
			#if		qWindows
				eSkipAtSignFonts	=	0x1,
				eSkipRasterFonts	=	0x2,
			#endif


			#if		qWindows
			eDefaultFilterOptions	=	eSkipAtSignFonts | eSkipRasterFonts
			#else
			eDefaultFilterOptions	=	0
			#endif
		};
		Led_InstalledFonts (
								#if		qXWindows
								Display* display,
								#endif
								FilterOptions filterOptions = eDefaultFilterOptions
							);

	public:
		nonvirtual	const vector<Led_SDK_String>&	GetUsableFontNames () const;

	private:
		FilterOptions			fFilterOptions;
		vector<Led_SDK_String>	fFontNames;

	#if		qWindows
	private:
		static	BOOL	FAR	PASCAL	FontFamilyAdderProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int fontType, LPVOID pThis);
	#endif
};





/*
@CLASS:			Led_GDIGlobals
@DESCRIPTION:	<p>Something of a hack version of GDI global variables. We want to keep certain GDI global variables
	computed ONCE, for speed reasons. And yet - if we get a certain windows message, we must refresh our cached global
	variables. The compromise is that all these globals are associated with this class, so that there is one place to
	call to refresh those globals.</p>
*/
class	Led_GDIGlobals {
	public:
		Led_GDIGlobals ();

	public:
		static	Led_GDIGlobals&	Get ();
	private:
		static	Led_GDIGlobals*	sThe;

	public:
		nonvirtual	void	InvalidateGlobals ();

	public:
		nonvirtual	Led_Distance	GetMainScreenLogPixelsH () const;
		nonvirtual	Led_Distance	GetMainScreenLogPixelsV () const;

	public:
		Led_Distance	fLogPixelsH;
		Led_Distance	fLogPixelsV;

	private:
		class	_Global_DESTRUCTOR_;
		friend	class	_Global_DESTRUCTOR_;
};






	
	
#if		qSupportLed30CompatAPI
Led_Point		GetRectOrigin (const Led_Rect& r);
Led_Size		GetRectSize (const Led_Rect& r);
Led_Distance	GetRectWidth (const Led_Rect& r);
Led_Distance	GetRectHeight (const Led_Rect& r);
Led_Point		GetRectTopLeft (const Led_Rect& r);
Led_Point		GetRectTopRight (const Led_Rect& r);
Led_Point		GetRectBottomRight (const Led_Rect& r);
Led_Point		GetRectBottomLeft (const Led_Rect& r);
#endif
	

Led_Rect	operator- (const Led_Rect& lhs, const Led_Point& rhs);
Led_Rect	operator+ (Led_Point p, Led_Rect r);
Led_Rect	operator+ (Led_Rect r, Led_Point p);
bool	Intersect (const Led_Rect& lhs, const Led_Rect& rhs);
bool	Intersect (const Led_Rect& lhs, const Led_Region& rhs);
bool	Intersect (const Led_Region& lhs, const Led_Rect& rhs);
bool	Intersect (const Led_Region& lhs, const Led_Region& rhs);

Led_Rect	Intersection (const Led_Rect& lhs, const Led_Rect& rhs);
Led_Rect	operator* (const Led_Rect& lhs, const Led_Rect& rhs);


Led_Size	operator+ (Led_Size lhs, Led_Size rhs);
Led_Size	operator* (int lhs, Led_Size rhs);

Led_Rect	InsetRect (const Led_Rect& r, int vBy, int hBy);
Led_Rect	EnsureRectInRect (const Led_Rect& r, Led_Rect enlosingR);
Led_Rect	EnsureRectOnScreen (const Led_Rect& r);



#if		qMacOS
	Led_Point	AsLedPoint (Point p);
	Point		AsQDPoint (Led_Point p);
	Led_Rect	AsLedRect (Rect r);
	Rect		AsQDRect (Led_Rect r);
	Led_Size	AsLedSize (Point s);
	Point		GetRectOrigin (const Rect& r);
	Point		GetRectSize (const Rect& r);
	short		GetRectHeight (const Rect& r);
	short		GetRectWidth (const Rect& r);
#elif	qWindows
	Led_Point	AsLedPoint (POINT p);
	POINT		AsPOINT (Led_Point p);
	Led_Rect	AsLedRect (RECT r);
	RECT		AsRECT (Led_Rect p);
	SIZE		AsSIZE (Led_Size s);
	Led_Size	AsLedSize (SIZE s);
#elif	qXWindows
	Led_Rect	AsLedRect (const XRectangle& r);
	XRectangle	AsXRect (const Led_Rect& r);
#endif






Led_TWIPS		Led_CvtScreenPixelsToTWIPSV (Led_Coordinate from);
Led_TWIPS		Led_CvtScreenPixelsToTWIPSH (Led_Coordinate from);
Led_Coordinate	Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS from);
Led_Coordinate	Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS from);




/*
@CLASS:			Led_FontMetrics
@DESCRIPTION:	<p><code>Led_FontMetrics</code> is a portable wrapper class on the Macintosh
	<em>FontInfo</em> structure, or the Windows <em>TEXTMETRIC</em> structure. It provides
	portable access to things like GetLineHeight (), and GetAscent (), etc...</p>
*/
class	Led_FontMetrics {
	#if		qXWindows
	public:
		struct	PlatformSpecific	{
			Led_Distance	fAscent;
			Led_Distance	fDescent;
			Led_Distance	fLeading;
			Led_Distance	fMaxCharWidth;
		};
	#endif
	public:
		Led_FontMetrics ();
		#if		qMacOS
			Led_FontMetrics (const FontInfo& from);
		#elif	qWindows
			Led_FontMetrics (const TEXTMETRIC& from);
		#elif	qXWindows
			Led_FontMetrics (const PlatformSpecific& from);
		#endif
		Led_FontMetrics (const Led_FontMetrics& from);
		const Led_FontMetrics& operator= (const Led_FontMetrics& rhs);


	public:
		nonvirtual	Led_Distance	GetAscent () const;
		nonvirtual	Led_Distance	GetDescent () const;
		nonvirtual	Led_Distance	GetLeading () const;
		nonvirtual	Led_Distance	GetHeight () const;	// does NOT include leading
		nonvirtual	Led_Distance	GetLineHeight () const;	// includes leading

	public:
		nonvirtual	Led_Distance	GetMaxCharacterWidth () const;
		#if		qWindows
			nonvirtual	Led_Distance	GetAveCharacterWidth () const;
		#endif

	// Convertion operator to make it easier to make GDI calls with one of our guys on a
	// with something expected a system specific one - like to fill in its value!
	public:
	#if		qMacOS
		operator const FontInfo* () const;
		operator FontInfo* ();
	#elif	qWindows
		operator const TEXTMETRIC* () const;
		operator TEXTMETRIC* ();
	#endif

	private:
	#if		qMacOS
		FontInfo	fPlatformSpecific;
	#elif	qWindows
		TEXTMETRIC	fPlatformSpecific;
	#elif	qXWindows
		PlatformSpecific	fPlatformSpecific;
	#endif
};





Led_Color	Led_GetTextColor ();
Led_Color	Led_GetTextBackgroundColor ();
Led_Color	Led_GetSelectedTextColor ();
Led_Color	Led_GetSelectedTextBackgroundColor ();






class	OffscreenTablet;

/*
@CLASS:			Led_Tablet_
@DESCRIPTION:	<p>See also @'Led_Tablet' - since that is what Led tends to make use of directly.</p>
		<p>This class is used to wrap a low level graphics drawing device. On Windows - this is an HDC.
	On the Mac - a GrafPtr (also CGrafPtr and GWorldPtr). On X-Windows - a drawable and display, and GC.</p>
		<p>This class right now is a very thin wrapper on those drawing prodedures (mostly for backward compatability reasons.
	Eventually - it may do a better job of wrapping those concepts/APIs genericly.</p>
*/
class	Led_Tablet_  {
	public:
		#if		qMacOS
			Led_Tablet_ (GrafPtr gp);
		#elif	qWindows
			enum	OwnDCControl	{ eOwnsDC, eDoesntOwnDC };

			Led_Tablet_ (HDC hdc = NULL, OwnDCControl ownsDC = eOwnsDC);
		#elif	qXWindows
			Led_Tablet_ (Display* display, Drawable drawable);
		#endif

	public:
		virtual ~Led_Tablet_ ();


	public:
		#if		qMacOS
			nonvirtual	operator GrafPtr () const;
		#elif	qWindows
			nonvirtual	operator HDC () const;
		#endif


	public:
		nonvirtual	Led_Coordinate	CvtFromTWIPSV (Led_TWIPS from) const;
		nonvirtual	Led_Coordinate	CvtFromTWIPSH (Led_TWIPS from) const;
		nonvirtual	Led_TWIPS		CvtToTWIPSV (Led_Coordinate from) const;
		nonvirtual	Led_TWIPS		CvtToTWIPSH (Led_Coordinate from) const;

	public:
		nonvirtual	Led_Point		CvtFromTWIPS (Led_TWIPS_Point from) const;
		nonvirtual	Led_TWIPS_Point	CvtToTWIPS (Led_Point from) const;
		nonvirtual	Led_Rect		CvtFromTWIPS (Led_TWIPS_Rect from) const;
		nonvirtual	Led_TWIPS_Rect	CvtToTWIPS (Led_Rect from) const;

	public:
		nonvirtual	void	ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, Led_Coordinate scrollBy);
		nonvirtual	void	FrameRegion (const Led_Region& r, const Led_Color& c);


	public:
		nonvirtual	void	FrameRectangle (const Led_Rect& r, Led_Color c, Led_Distance borderWidth);

	public:
		#if		qMacOS
			nonvirtual	void	SetPort ();
		#elif	qWindows
			nonvirtual	BOOL BitBlt(int x, int y, int nWidth, int nHeight, Led_Tablet_* pSrcDC, int xSrc, int ySrc, DWORD dwRop);
			nonvirtual	BOOL CreateCompatibleDC (Led_Tablet_* pDC);
			nonvirtual	COLORREF SetTextColor (COLORREF crColor);
			nonvirtual	COLORREF SetBkColor(COLORREF crColor);
			nonvirtual	BOOL IsPrinting() const;
			nonvirtual	BOOL RoundRect(int x1, int y1, int x2, int y2, int x3, int y3);
			nonvirtual	BOOL TextOut(int x, int y, LPCTSTR lpszString, int nCount);
	//		nonvirtual	SIZE	GetTextExtent (LPCTSTR lpszString, int nCount) const;
			nonvirtual	int SetBkMode(int nBkMode);
			nonvirtual	SIZE GetWindowExt() const;
			nonvirtual	SIZE GetViewportExt() const;
			nonvirtual	BOOL Rectangle(int x1, int y1, int x2, int y2);
			nonvirtual	BOOL Rectangle(const RECT& r);
			nonvirtual	BOOL Rectangle(LPCRECT lpRect);
			nonvirtual	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const;
			nonvirtual	HBITMAP	SelectObject (HBITMAP hBitmap);
			#if		STRICT
				nonvirtual	HFONT	SelectObject (HFONT hFont);
			#endif
			nonvirtual	POINT SetWindowOrg(int x, int y);
			nonvirtual	int	GetDeviceCaps (int nIndex) const;
			nonvirtual	BOOL Attach (HDC hDC, OwnDCControl ownsDC = eOwnsDC);
			nonvirtual	HDC Detach ();
		#elif	qXWindows
	public:
			nonvirtual	void				SetFont (const Led_FontSpecification& fontSpec);
	private:
		map<string,XFontStruct*>	fFontCache;
		enum { kMaxFontCacheSize = 5 };
	public:
			nonvirtual	void				SetDrawableOrigin (const Led_Point& origin);
	private:
			Led_Point	fDrawableOrigin;
		#endif


	#if		qWindows
	private:
		nonvirtual	HWND GetWindow() const;
		nonvirtual	unsigned int SetTextAlign (unsigned int nTextAlign);
	#endif

#if		qXWindows
	public:
		static int	IngoreXErrorHandler (Display* display, XErrorEvent* error);
#endif

	#if		qXWindows
	private:
		nonvirtual	Led_SDK_String	BestMatchFont (const Led_FontSpecification& fsp, const vector<Led_SDK_String>& fontsList);
	public:
		static	void			ParseFontName (const Led_SDK_String& fontName, Led_SDK_String* familyName, Led_SDK_String* fontSize, Led_SDK_String* fontWeight, Led_SDK_String* fontSlant);
	#endif

	public:
		nonvirtual	void	MoveTo (const Led_Point& to);
		nonvirtual	void	LineTo (const Led_Point& to);

	#if		qXWindows
		private:
			Led_Point	fCurDrawLineLoc;
	#endif

	public:
		nonvirtual	void	MeasureText (const Led_FontMetrics& precomputedFontMetrics,
											const Led_tChar* text, size_t nTChars, Led_Distance* charLocations
										);
		nonvirtual	void	TabbedTextOut (const Led_FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
												TextDirection direction,
												Led_Point outputAt, Led_Coordinate hTabOrigin, const Led_TabStopList& tabStopList,
												Led_Distance* amountDrawn, Led_Coordinate hScrollOffset
											);


	public:
		nonvirtual	void	SetBackColor (const Led_Color& backColor);
		nonvirtual	void	SetForeColor (const Led_Color& foreColor);

	public:
		nonvirtual	void	EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Led_Color& eraseColor);
#if		qSupportLed30CompatAPI
		nonvirtual	void	HilightARectangle_SolidHelper (const Led_Rect& hilightRect, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);
#endif
		nonvirtual	void	HilightArea_SolidHelper (const Led_Rect& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);
		nonvirtual	void	HilightArea_SolidHelper (const Led_Region& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);

#if		qWindows
	private:
		class	RecolorHelper;
	private:
		RecolorHelper*	fRecolorHelper;
#endif

	public:
		class	ClipNarrowAndRestore;

	public:
		nonvirtual	Led_Region	GetClip () const;
		nonvirtual	bool		GetClip (Led_Region* r) const;
		nonvirtual	void		SetClip ();
		nonvirtual	void		SetClip (const Led_Rect& clipTo);
		nonvirtual	void		SetClip (const Led_Region& clipTo);

	public:
		nonvirtual	Led_FontMetrics	GetFontMetrics () const;


	#if		qMacOS
		private:
			GrafPtr	fGrafPort;
	#elif	qWindows
		public:
			HDC m_hDC;          // The output DC (must be first data member)
			HDC m_hAttribDC;    // The Attribute DC
			BOOL m_bPrinting;
			OwnDCControl	fOwnsDC;
		private:
			mutable	Led_Distance	fLogPixelsV;
			mutable	Led_Distance	fLogPixelsH;
	#elif	qXWindows
		private:
			Display*				fDisplay;
			Drawable				fDrawable;
			GC						fGC;
			Colormap				fColormap;
			mutable	XFontStruct*	fCachedFontInfo;
			map<string,string>		fFontMappingCache;
	#endif

	public:
		friend	class	OffscreenTablet;
};



/*
@CLASS:			Led_Tablet_::ClipNarrowAndRestore
@DESCRIPTION:	<p>Further narrow the existing clip region in the given tablet to the constructor. Then
			restore the clip region for the tablet to what it was when the contructor was called.</p>
*/
class	Led_Tablet_::ClipNarrowAndRestore {
	public:
		ClipNarrowAndRestore (Led_Tablet_* tablet);
		ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Rect& clipFurtherTo);
		ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Region& clipFurtherTo);
		~ClipNarrowAndRestore ();

	private:
		Led_Tablet_*	fTablet;
		bool			fHasOldClip;
		Led_Region		fOldClip;
};







/*
@CLASS:			Led_Tablet
@DESCRIPTION:	<p>A pointer to a @'Led_Tablet_' structure. These pointers are used throughout Led.</p>
	<p>In versions of Led prior to Led 3.0 - this typedef refered directly to a Mac GrafPort or MFC CDC.</p>
	<p>Now it refers to a structure which wraps those lower level concepts (and doesnt depend on MFC anymore).</p>
*/
typedef	Led_Tablet_*	Led_Tablet;








#if		qWindows
class	Led_WindowDC : public Led_Tablet_ {
	public:
		Led_WindowDC (HWND hWnd):
			m_hWnd (hWnd)
			{
				Led_Require (m_hWnd == NULL or ::IsWindow (m_hWnd));
				if (!Attach (::GetWindowDC (m_hWnd))) {
					Led_ThrowOutOfMemoryException ();
				}
			}

			~Led_WindowDC ()
				{
					Led_AssertNotNil (m_hDC);
					::ReleaseDC (m_hWnd, Detach ());
				}
	private:
		HWND	m_hWnd;
};
#endif





#if		qMacOS
class	Led_MacPortAndClipRegionEtcSaver {
	public:
		Led_MacPortAndClipRegionEtcSaver ();
		~Led_MacPortAndClipRegionEtcSaver ();

	private:
		GrafPtr		fSavedPort;
		short		fOldLeft;
		short		fOldTop;
		RGBColor	fRGBFgColor;
		RGBColor	fRGBBkColor;
		RgnHandle	fOldClip;
};
#endif





#if		qWindows
class	Led_Bitmap {
	public:
		Led_Bitmap ();
		~Led_Bitmap ();
	
	public:
		nonvirtual	void	DeleteObject ();
		nonvirtual	operator HBITMAP() const;

	public:
		nonvirtual	Led_Size	GetImageSize () const;
		nonvirtual	BOOL		CreateCompatibleBitmap (HDC hdc, Led_Distance nWidth, Led_Distance nHeight);
		nonvirtual	BOOL		CreateCompatibleDIBSection (HDC hdc, Led_Distance nWidth, Led_Distance nHeight);
		nonvirtual	void		LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName);

	private:
		HBITMAP		m_hObject;
		Led_Size	fImageSize;
};
#endif






/*
@CLASS:			OffscreenTablet
@DESCRIPTION:	<p>An offscreen tablet is a helper object used to do offscreen imaging. This is useful in
			avoidance of flicker. Also, by encapsulating this procedure into a class, it becomes easier
			to add the functionality to several places in Led, yet with very different underlying implementations
			on each platform.</p>
*/
class	OffscreenTablet {
	public:
		OffscreenTablet ();
		~OffscreenTablet ();

	public:
		nonvirtual	void		Setup (Led_Tablet origTablet);
		nonvirtual	Led_Tablet	PrepareRect (const Led_Rect& currentRowRect, Led_Distance extraToAddToBottomOfRect = 0);
		nonvirtual	void		BlastBitmapToOrigTablet ();

	private:
		class	OT : public Led_Tablet_ {
			private:
				typedef	Led_Tablet_	inherited;
			public:
				#if		qMacOS
					OT (GrafPtr gp);
				#elif	qWindows
					OT (HDC hdc = NULL, OwnDCControl ownsDC = eOwnsDC);
				#elif	qXWindows
					OT (Display* display, Drawable drawable);
				#endif
		};

	private:
		Led_Tablet	fOrigTablet;
		Led_Rect	fOffscreenRect;
		Led_Tablet	fOffscreenTablet;
	#if		qMacOS
		GDHandle	fOrigDevice;
		CGrafPtr	fOrigPort;
		GWorldPtr	fOffscreenGWorld;
	#elif	qWindows
		OT			fMemDC;
		Led_Bitmap	fMemoryBitmap;		// only can create / select inside loop cuz there is where we know the size.
										// but decare outside, so stays around for successive rows which are the same size.
		HBITMAP		fOldBitmapInDC;		// used for save/restore of bitmap associated with the DC.
	#elif	qXWindows
		Drawable	fPixmap;
	#endif
};





/*
@CLASS:			Led_GDI_Obj_Selector
@DESCRIPTION:	<p><code>Led_GDI_Obj_Selector</code> is a stack-based class designed to help
	out selecting objects into a Led_Tablet (windows DC, grafport, etc).
		<p>The constructor takes a tablet, and object to select into it (HGDIObject, etc),
	and selects it into the tablet. It saves gthe results of the SelectObject calls (old values).
	And on its destructor, it restores the old values.</p>
		<p>This is useful when you want to make sure that an object you've selected into a tablet (HDC)
	will be released, and restored to its original state through all paths through
	the code, including in the even of exceptions.</p>
*/
class	Led_GDI_Obj_Selector {
	public:
		#if		qWindows
			Led_GDI_Obj_Selector (Led_Tablet tablet, HGDIOBJ objToSelect);
		#elif	qMacOS || qXWindows
			Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen);
		#endif
	public:
		~Led_GDI_Obj_Selector ();
	private:
		Led_Tablet	fTablet;
		#if		qWindows
		HGDIOBJ 	fRestoreObject;
		HGDIOBJ 	fRestoreAttribObject;
		#elif	qMacOS
			Led_Pen	fRestorePen;
		#endif
};



#if		qWindows
/*
@CLASS:			Led_Win_Obj_Selector
@DESCRIPTION:	<p>Alias for newer @'Led_GDI_Obj_Selector'.</p>
*/
typedef	Led_GDI_Obj_Selector	Led_Win_Obj_Selector;
#endif




/*
 *	Trap Caching support
 *
 *		This is a groty hack - but can be quite a big speed improvment for
 *	the Mac (at least 68K - I've never tried on PowerPC). For now
 *	(and perhaps always) we only support this for the 68K code.
 */
#if		qMacOS
	void		GDI_RGBForeColor (const RGBColor& color);
	void		GDI_RGBBackColor (const RGBColor& color);
	RGBColor	GDI_GetForeColor ();
	RGBColor	GDI_GetBackColor ();
#endif












// Even for windows we have have this defined if we build including QuickTime support!
#ifndef	qHaveMacPictureDefined
	#define	qHaveMacPictureDefined	qMacOS
#endif

/*
@CLASS:			Led_Picture
@DESCRIPTION:	<p><code>Led_Picture</code> is a portable abstraction of a Macintosh Picture object.
	It can be displayed both on windows, and on the Mac (on Windows, it is only displayed if Apples
	QuickTime is installed). There are a bunch of routines (e.g Led_GetMacPictTop) which portable
	allow access to the size of the picture (even on windows if QT not available). And there are
	portable routines to draw the picture (again, with the windows QT caveat).</p>
*/
#if		qHaveMacPictureDefined
	typedef	Picture	Led_Picture;
#else
	struct	Led_Picture {
		short	picSize;
		short	picFrameTop;
		short	picFrameLeft;
		short	picFrameBottom;
		short	picFrameRight;
		// other data off end (specified by picSize) - note byte order for shorts is MAC
	};
#endif


short		Led_GetMacPictTop (const Led_Picture* picture);
short		Led_GetMacPictLeft (const Led_Picture* picture);
short		Led_GetMacPictBottom (const Led_Picture* picture);
short		Led_GetMacPictRight (const Led_Picture* picture);
short		Led_GetMacPictWidth (const Led_Picture* picture);
short		Led_GetMacPictHeight (const Led_Picture* picture);
Led_Size	Led_GetMacPictSize (const Led_Picture* picture);
#if		qMacOS
short		Led_GetMacPictTop (const Led_Picture*const* picture);
short		Led_GetMacPictLeft (const Led_Picture*const* picture);
short		Led_GetMacPictBottom (const Led_Picture*const* picture);
short		Led_GetMacPictRight (const Led_Picture*const* picture);
short		Led_GetMacPictWidth (const Led_Picture*const* picture);
short		Led_GetMacPictHeight (const Led_Picture*const* picture);
Led_Size	Led_GetMacPictSize (const Led_Picture*const* picture);
#endif




// Windows DIB support
#ifndef	qHaveWindowsDIBDefined
	#define	qHaveWindowsDIBDefined	qWindows
#endif

#if		!qHaveWindowsDIBDefined
	// structs copied (paraphrased) from MSVC 4.1 includes (WinGDI.h)
	struct	BITMAPINFOHEADER	{
		unsigned long	biSize;
		long			biWidth;
		long			biHeight;
		unsigned short	biPlanes;
		unsigned short	biBitCount;
		unsigned long	biCompression;
		unsigned long	biSizeImage;
		long			biXPelsPerMeter;
		long			biYPelsPerMeter;
		unsigned long	biClrUsed;
		unsigned long	biClrImportant;
	};
	struct BITMAPCOREHEADER {
		unsigned long	bcSize;
		unsigned short	bcWidth;
		unsigned short	bcHeight;
		unsigned short	bcPlanes;
		unsigned short	bcBitCount;
	};
	struct	RGBQUAD	{
		unsigned char	rgbBlue;
		unsigned char	rgbGreen;
		unsigned char	rgbRed;
		unsigned char	rgbReserved;
	};
	struct RGBTRIPLE {
		unsigned char	rgbtBlue;
		unsigned char	rgbtGreen;
		unsigned char	rgbtRed;
	};
	struct	BITMAPINFO {
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[1];
	};
#endif
/*
@CLASS:			Led_DIB
@DESCRIPTION:	<p><em>Led_DIB</em> is a MS-Windows DIB (device independent bitmap) object. Much
	like @'Led_Picture', it has a host of accessor routines to get at its size, etc. But slightly
	better than Led_Picture, Led_DIB can be rendered on the Mac without assuming any special
	software (ie no analog to QT) is available. The downside is that I only support a few types
	of DIBs, but I appear to have most that windows currently generates. And it wouldn't be
	too hard to extend the code to support a few more types, if I knew what they were.</p>
*/
typedef	BITMAPINFO	Led_DIB;

Led_Size	Led_GetDIBImageSize (const Led_DIB* dib);
size_t		Led_GetDIBPalletByteCount (const Led_DIB* dib);
size_t		Led_GetDIBImageRowByteCount (const Led_DIB* dib);
size_t		Led_GetDIBImageByteCount (const Led_DIB* dib);
Led_DIB*	Led_CloneDIB (const Led_DIB* dib);
const void*	Led_GetDIBBitsPointer (const Led_DIB* dib);


#if		qWindows
Led_DIB*	Led_DIBFromHBITMAP (HDC hDC, HBITMAP hbm);
#endif





void	AddRectangleToRegion (Led_Rect addRect, Led_Region* toRgn);









#if		qProvideIMESupport
class	Led_IME {
	public:
		Led_IME ();

	public:
		static	Led_IME&	Get ();
	private:
		static	Led_IME*	sThe;

	private:
		nonvirtual	bool	Available ()  const;	//tmphack - don't think this is ever used
	public:
		nonvirtual	void	ForgetPosition ();
		nonvirtual	void	NotifyPosition (HWND pWnd, const SHORT x, const SHORT y);
		nonvirtual	void	NotifyOfFontChange (HWND hWnd, const LOGFONT& lf);
		nonvirtual	void	IMEOn (HWND pWnd);
		nonvirtual	void	IMEOff (HWND pWnd);
		nonvirtual	void	Enable ();
		nonvirtual	void	Disable ();
		nonvirtual	wstring	GetCompositionResultStringW (HWND hWnd);

	private:
		nonvirtual	void	UpdatePosition (const HWND hWnd, const SHORT x, const SHORT y);
		nonvirtual	void	SendSimpleMessage (HWND pWnd, UINT fnc, WPARAM wParam);

		short			(FAR PASCAL *fSendIMEMessageProc) (HWND, DWORD);
		BOOL			(FAR PASCAL *fIMEEnableProc) (HWND, BOOL);
		DWORD			(FAR PASCAL *fImmGetContext) (HWND);
		BOOL			(FAR PASCAL *fImmSetCompositionFont) (DWORD, const LOGFONT*);
		BOOL			(FAR PASCAL *fImmReleaseContext) (HWND, DWORD);
		LONG			(FAR PASCAL *fImmGetCompositionStringW) (DWORD, DWORD, LPVOID, DWORD);
		BOOL			(FAR PASCAL *fImmSetCompositionWindow) (DWORD, const void*);
		BOOL			(FAR PASCAL *fImmSetOpenStatus) (DWORD, BOOL);

		

		bool			fWinNlsAvailable;
		SHORT			fLastX;
		SHORT			fLastY;

	private:
		class	_Global_DESTRUCTOR_;
		friend	class	_Global_DESTRUCTOR_;
};
#endif


Led_Rect	CenterRectInRect (const Led_Rect& r, const Led_Rect& centerIn);
#if		qWindows
void	Led_CenterWindowInParent (HWND w);
#endif






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//struct	Point_Base
	template	<typename	COORD_TYPE>
		inline	Point_Base<COORD_TYPE>::Point_Base ():
			v (COORD_TYPE (0)),
			h (COORD_TYPE (0))
			{
			}
	template	<typename	COORD_TYPE>
		inline	Point_Base<COORD_TYPE>::Point_Base (COORD_TYPE newV, COORD_TYPE newH):
			v (newV),
			h (newH)
			{
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator== (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return (lhs.v == rhs.v and lhs.h == rhs.h);
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator!= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return (lhs.v != rhs.v or lhs.h != rhs.h);
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator<  (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return  ((lhs.v <= rhs.v) and (lhs.h <= rhs.h) and (lhs != rhs));
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator<= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return  ((lhs.v <= rhs.v) and (lhs.h <= rhs.h));
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator> (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return  ((lhs.v >= rhs.v) and (lhs.h >= rhs.h) and lhs != rhs);
			}
	template	<typename	COORD_TYPE>
		inline	bool	operator>= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				return  ((lhs.v >= rhs.v) and (lhs.h >= rhs.h));
			}
	template	<typename	COORD_TYPE>
		inline	Point_Base<COORD_TYPE>	operator+  (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
			{
				Led_Point	result	=	lhs;
				result.v += rhs.v;
				result.h += rhs.h;
				return result;
			}




//struct	Led_Point
	inline	Led_Point	operator- (const Led_Point& lhs, const Led_Point& rhs)
		{
			return (Led_Point (lhs.v-rhs.v, lhs.h-rhs.h));
		}




//struct	Rect_Base<POINT_TYPE,SIZE_TYPE>
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base
		@DESCRIPTION:	<p>No-argument constructor leaves object uninitialized (garbage data).</p>
		*/
		inline	Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base ():
			top (CoordinateType (0)),
			left (CoordinateType (0)),
			bottom (CoordinateType (0)),
			right (CoordinateType (0))
			{
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base
		@DESCRIPTION:	<p>Initialize with argument top, left, height, width.</p>
		*/
		inline	Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base (CoordinateType newTop, CoordinateType newLeft, DistanceType newHeight, DistanceType newWidth):
				top (newTop),
				left (newLeft),
				bottom (newTop + newHeight),
				right (newLeft + newWidth)
				{
				}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base (POINT_TYPE origin, SIZE_TYPE size):
				top (origin.v),
				left (origin.h),
				bottom (origin.v + size.v),
				right (origin.h + size.h)
			{
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetTop
		@DESCRIPTION:	<p>Retrieve top of rectangle.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::CoordinateType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetTop () const
			{
				return top;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetLeft
		@DESCRIPTION:	<p>Retrieve left of rectangle.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::CoordinateType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetLeft () const
			{
				return left;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetBottom
		@DESCRIPTION:	<p>Retrieve bottom of rectangle.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::CoordinateType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetBottom () const
			{
				return bottom;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetRight
		@DESCRIPTION:	<p>Retrieve right of rectangle.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::CoordinateType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetRight () const
			{
				return right;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::IsEmpty
		@DESCRIPTION:	<p>Returns true if either horizontal or vertical dimentions are less or equal to zero.</p>
		*/
		inline	bool	Rect_Base<POINT_TYPE,SIZE_TYPE>::IsEmpty () const
				{
					return (right <= left or bottom <= top);
				}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetHeight
		@DESCRIPTION:	<p>Asserts height is non-negative, and then returns height.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::DistanceType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetHeight () const
			{
				Led_Ensure (bottom >= top);
				return (bottom-top);
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::GetWidth
		@DESCRIPTION:	<p>Asserts width is non-negative, and then returns width.</p>
		*/
		inline	typename	Rect_Base<POINT_TYPE,SIZE_TYPE>::DistanceType	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetWidth () const
			{
				Led_Ensure (right >= left);
				return (right-left);
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	POINT_TYPE	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetTopLeft () const
			{
				return (POINT_TYPE (top, left));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	POINT_TYPE	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetTopRight () const
			{
				return (POINT_TYPE (top, right));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	POINT_TYPE	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetBotRight () const
			{
				return (POINT_TYPE (bottom, right));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	POINT_TYPE	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetOrigin () const
			{
				return (POINT_TYPE (top, left));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	SIZE_TYPE	Rect_Base<POINT_TYPE,SIZE_TYPE>::GetSize () const
			{
				return (SIZE_TYPE (bottom-top, right-left));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	bool	Rect_Base<POINT_TYPE,SIZE_TYPE>::Contains (POINT_TYPE p) const
			{
				return  ((p >= GetTopLeft ()) and (p <= GetBotRight ()));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	bool	Rect_Base<POINT_TYPE,SIZE_TYPE>::Contains (Rect_Base<POINT_TYPE,SIZE_TYPE> r) const
			{
				// could be more terse, but this does it...
				return  (Contains (r.GetTopLeft ()) and Contains (r.GetBotRight ()));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	void	Rect_Base<POINT_TYPE,SIZE_TYPE>::SetOrigin (POINT_TYPE p)
			{
				top = p.v;
				left = p.h;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	void	Rect_Base<POINT_TYPE,SIZE_TYPE>::SetTop (CoordinateType top)
			{
				this->top = top;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	void	Rect_Base<POINT_TYPE,SIZE_TYPE>::SetLeft (CoordinateType left)
			{
				this->left = left;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	void	Rect_Base<POINT_TYPE,SIZE_TYPE>::SetBottom (CoordinateType bottom)
			{
				this->bottom = bottom;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	void	Rect_Base<POINT_TYPE,SIZE_TYPE>::SetRight (CoordinateType right)
			{
				this->right = right;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	Rect_Base<POINT_TYPE,SIZE_TYPE>::operator+= (const POINT_TYPE& delta)
			{
				*this = THIS_TYPE (GetTopLeft () + delta, GetSize ());	// simple definition - but could tweek!
				return *this;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	Rect_Base<POINT_TYPE,SIZE_TYPE>::operator-= (const POINT_TYPE& delta)
			{
				*this = THIS_TYPE (GetTopLeft () - delta, GetSize ());	// simple definition - but could tweek!
				return *this;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		/*
		@METHOD:		Rect_Base<POINT_TYPE,SIZE_TYPE>::operator*=
		@DESCRIPTION:	<p>Intersects this rect with the argument rectangle.</p>
		*/
		inline	const Rect_Base<POINT_TYPE,SIZE_TYPE>&	Rect_Base<POINT_TYPE,SIZE_TYPE>::operator*= (const Rect_Base<POINT_TYPE,SIZE_TYPE>& intersectWith)
			{
				POINT_TYPE	topLeft		=	Led_Max (GetTopLeft (), intersectWith.GetTopLeft ());
				POINT_TYPE	botRight	=	Led_Min (GetBotRight (), intersectWith.GetBotRight ());
				POINT_TYPE	newSize		=	botRight-topLeft;
				if (newSize >= Led_Point (0, 0)) {
					*this = THIS_TYPE (topLeft, SIZE_TYPE (newSize));
				}
				else {
					*this = THIS_TYPE (0, 0, 0, 0);
				}
				return *this;
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	bool	operator== (const Rect_Base<POINT_TYPE,SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE,SIZE_TYPE>& rhs)
			{
				return ((lhs.GetLeft () == rhs.GetLeft ()) and (lhs.GetRight () == rhs.GetRight ())
					and (lhs.GetTop ()  == rhs.GetTop ())  and (lhs.GetBottom () == rhs.GetBottom ()));
			}
	template	<typename POINT_TYPE, typename SIZE_TYPE>
		inline	bool	operator!= (const Rect_Base<POINT_TYPE,SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE,SIZE_TYPE>& rhs)
			{
				return ((lhs.GetLeft () != rhs.GetLeft ()) or (lhs.GetRight () != rhs.GetRight ())
					or  (lhs.GetTop ()  != rhs.GetTop ())  or (lhs.GetBottom () != rhs.GetBottom ()));
			}





	//class	Led_TWIPS
		inline Led_TWIPS::Led_TWIPS (long v):
			fValue (v)
				{
				}
		inline	Led_TWIPS::operator long () const
			{
				return fValue; 
			}
		inline	Led_TWIPS& Led_TWIPS::operator+= (const Led_TWIPS& rhs)
			{
				fValue += rhs.fValue;
				return *this;
			}
		inline	Led_TWIPS& Led_TWIPS::operator-= (const Led_TWIPS& rhs)
			{
				fValue -= rhs.fValue;
				return *this;
			}
		inline	Led_TWIPS& Led_TWIPS::operator*= (double rhs)
			{
				fValue = static_cast<long> (fValue * rhs);
				return *this;
			}
		inline	Led_TWIPS	operator+ (const Led_TWIPS& lhs, const Led_TWIPS& rhs)
			{
				return Led_TWIPS (static_cast<long> (lhs) + static_cast<long> (rhs));
			}
		inline	Led_TWIPS	operator- (const Led_TWIPS& lhs, const Led_TWIPS& rhs)
			{
				return Led_TWIPS (static_cast<long> (lhs) - static_cast<long> (rhs));
			}


	#if		qWindows
	//class	auto_gdi_ptr
		inline auto_gdi_ptr::auto_gdi_ptr (HGDIOBJ gdiObj):
			fGDIObj (gdiObj)
			{
			}
		inline	auto_gdi_ptr::~auto_gdi_ptr ()
			{
				Led_Verify (::DeleteObject (fGDIObj));
			}
	#endif



	//class	Led_Region
		inline	Led_Region::Led_Region ()
			#if		qMacOS
				:fRgn (::NewRgn ()),
				fOwned (true)
			#elif	qWindows
				:fRgn (::CreateRectRgn (0, 0, 0, 0))
			#endif
			{
				#if		qMacOS || qWindows
					Led_ThrowIfNull (fRgn);
				#endif
			}
		inline	Led_Region::Led_Region (const Led_Rect& r)
			#if		qMacOS
				:fRgn (::NewRgn ()),
				fOwned (true)
			#elif	qWindows
				:fRgn (::CreateRectRgn (r.GetLeft (), r.GetTop (), r.GetRight (), r.GetBottom ()))
			#endif
			{
				Led_Require (r.GetHeight () >= 0);
				Led_Require (r.GetWidth () >= 0);
				#if		qMacOS || qWindows
					Led_ThrowIfNull (fRgn);
				#endif
				#if		qMacOS
					::SetRectRgn (fRgn, (short)r.left, (short)r.top, (short)r.right, (short)r.bottom);
				#endif
				Led_Assert (GetBoundingRect () == r or (GetBoundingRect ().IsEmpty () and r.IsEmpty ()));
			}
		inline	Led_Region::Led_Region (const Led_Region& from)
			#if		qMacOS
				:fRgn (::NewRgn ()),
				fOwned (true)
			#elif	qWindows
				:fRgn (::CreateRectRgn (0, 0, 0, 0))
			#endif
			{
				#if		qMacOS || qWindows
					Led_ThrowIfNull (fRgn);
				#endif
				#if		qMacOS
					::CopyRgn (from.GetOSRep (), fRgn);
				#elif	qWindows
					Led_Verify (::CombineRgn (fRgn, from, from, RGN_COPY) != ERROR);
				#endif
			}
		inline	const Led_Region& Led_Region::operator= (const Led_Region& rhs)
			{
				#if		qMacOS
					if (fOwned and fRgn != NULL) {
						::DisposeRgn (fRgn);
					}
					fOwned = true;
					fRgn = ::NewRgn ();
					::CopyRgn (rhs.GetOSRep (), fRgn);
				#elif	qWindows
					Led_Verify (::CombineRgn (fRgn, rhs, rhs, RGN_COPY) != ERROR);
				#endif
				#if		qMacOS || qWindows
					Led_ThrowIfNull (fRgn);
				#endif
				return *this;
			}
		inline	Led_Region::~Led_Region ()
			{
				#if		qMacOS
					if (fOwned and fRgn != NULL) {
						::DisposeRgn (fRgn);
					}
				#elif	qWindows
					if (fRgn != NULL) {
						::DeleteObject (fRgn);
					}
				#endif
			}
		inline	bool	Led_Region::IsEmpty () const
			{
				#if		qMacOS || qWindows
					Led_AssertNotNil (fRgn);
				#endif
				Led_Assert (false);	//NYI - not used yet - so don't worry about this right now... LGP 2002-12-03
				return false;
			}
		inline	Led_Rect	Led_Region::GetBoundingRect () const
			{
				#if		qMacOS || qWindows
					Led_AssertNotNil (fRgn);
				#endif
				#if		qMacOS
					#if		TARGET_CARBON
						Rect	bnds;
						return AsLedRect (*::GetRegionBounds (fRgn, &bnds));
					#else
						return AsLedRect ((**fRgn).rgnBBox);
					#endif
				#elif	qWindows
					RECT	r;
					int		tmp	=	::GetRgnBox (fRgn, &r);
					Led_Verify (tmp != 0);
					#if		qDebug
						{
							if (tmp == 0) {
								Led_Assert (AsLedRect (r) == Led_Rect (0, 0, 0, 0));
							}
						}
					#endif
					return AsLedRect (r);
				#else
					Led_Assert (false);
					return Led_Rect (0, 0, 0, 0);
				#endif
			}
		inline	Led_Region	operator* (const Led_Region& lhs, const Led_Region& rhs)
			{
				Led_Region	result;
				#if		qMacOS
					::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
				#elif	qWindows
					Led_Verify (::CombineRgn (result, lhs, rhs, RGN_AND) != ERROR);
				#endif
				return result;
			}
		inline	Led_Region	operator+ (const Led_Region& lhs, const Led_Region& rhs)
			{
				Led_Region	result;
				#if		qMacOS
					::UnionRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
				#elif	qWindows
					Led_Verify (::CombineRgn (result, lhs, rhs, RGN_OR) != ERROR);
				#endif
				return result;
			}




// class Led_TabStopList::Led_TabStopList
	inline	Led_TabStopList::Led_TabStopList ()
		{
		}
	inline	Led_TabStopList::~Led_TabStopList ()
		{
		}
	inline	Led_Distance	Led_TabStopList::ComputeTabStopAfterPosition (Led_Tablet tablet, Led_Distance afterPos) const
		{
			Led_RequireNotNil (tablet);
			return tablet->CvtFromTWIPSH (ComputeTabStopAfterPosition (tablet->CvtToTWIPSH (afterPos)));
		}




#if		qWindows
// class Led_Bitmap
	inline	Led_Bitmap::Led_Bitmap ():
		m_hObject (NULL),
		fImageSize (Led_Size (0, 0))
		{
		}
	inline	Led_Bitmap::~Led_Bitmap ()
		{
			if (m_hObject != NULL) {
				::DeleteObject (m_hObject);
			}
		}
	inline	void	Led_Bitmap::DeleteObject ()
		{
			if (m_hObject != NULL) {
				::DeleteObject (m_hObject);
				m_hObject = NULL;
			}
		}
	inline	Led_Bitmap::operator HBITMAP() const
		{
			return m_hObject;
		}
	inline	Led_Size	Led_Bitmap::GetImageSize () const
		{
			// only valid if m_hObject != NULL
			Led_RequireNotNil (m_hObject);
			return fImageSize;
		}
#endif



	//class	Led_Tablet_
	#if		qMacOS
		inline	Led_Tablet_::operator GrafPtr () const
			{
				return fGrafPort;
			}
	#elif	qWindows
		inline	Led_Tablet_::operator HDC() const
			{
				return m_hDC;
			}
	#endif
	/*
	@METHOD:		Led_Tablet_::CvtFromTWIPSV
	@DESCRIPTION:	
		<p>See also @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
	*/
	inline	Led_Coordinate	Led_Tablet_::CvtFromTWIPSV (Led_TWIPS from) const
		{
			#if		qWindows
				if (fLogPixelsV == 0) {
					fLogPixelsV = GetDeviceCaps (LOGPIXELSY);
				}
				POINT	vpOrg;
				Led_Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
				POINT	wOrg;
				Led_Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
				POINT	x	=	vpOrg;
				x.y += ::MulDiv (from, fLogPixelsV, 1440);
				Led_Verify (::DPtoLP (m_hAttribDC, &x, 1));
				x.y -= wOrg.y;
				Led_Assert (x.x == wOrg.x);
				return x.y;
			#else
				//TMPHACK - look at tablet resolution?
				return from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsV () / 1440;
//				return from / 20;	// assume 72dpi on mac
			#endif
		}
	/*
	@METHOD:		Led_Tablet_::CvtFromTWIPSH
	@DESCRIPTION:	
		<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
	*/
	inline	Led_Coordinate	Led_Tablet_::CvtFromTWIPSH (Led_TWIPS from) const
		{
			#if		qWindows
				if (fLogPixelsH == 0) {
					fLogPixelsH = GetDeviceCaps (LOGPIXELSX);
				}
				POINT	vpOrg;
				Led_Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
				POINT	wOrg;
				Led_Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
				POINT	x	=	vpOrg;
				x.x += ::MulDiv (from, fLogPixelsH, 1440);
				Led_Verify (::DPtoLP (m_hAttribDC, &x, 1));
				x.x -= wOrg.x;
				Led_Assert (x.y == wOrg.y);
				return x.x;
			#else
				//TMPHACK - look at tablet resolution?
				return from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsH () / 1440;
//				return from / 20;	// assume 72dpi on mac
			#endif
		}
	/*
	@METHOD:		Led_Tablet_::CvtToTWIPSV
	@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
		<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSH'.</p>
	*/
	inline	Led_TWIPS	Led_Tablet_::CvtToTWIPSV (Led_Coordinate from) const
		{
			#if		qWindows
				if (fLogPixelsV == 0) {
					fLogPixelsV = GetDeviceCaps (LOGPIXELSY);
				}
				POINT	vpOrg;
				Led_Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
				POINT	wOrg;
				Led_Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
				POINT	x	=	wOrg;
				x.y += from;
				Led_Verify (::LPtoDP (m_hAttribDC, &x, 1));
				x.y -= vpOrg.y;
				Led_Assert (x.x == wOrg.x);
				return Led_TWIPS (::MulDiv (x.y, 1440, fLogPixelsV));
			#else
				return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
//				return Led_TWIPS (from * 20);	// assume 72dpi on mac
			#endif
		}
	/*
	@METHOD:		Led_Tablet_::CvtToTWIPSH
	@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
		<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV'.</p>
	*/
	inline	Led_TWIPS	Led_Tablet_::CvtToTWIPSH (Led_Coordinate from) const
		{
			#if		qWindows
				if (fLogPixelsH == 0) {
					fLogPixelsH = GetDeviceCaps (LOGPIXELSX);
				}
				POINT	vpOrg;
				Led_Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
				POINT	wOrg;
				Led_Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
				POINT	x	=	wOrg;
				x.x += from;
				Led_Verify (::LPtoDP (m_hAttribDC, &x, 1));
				x.x -= vpOrg.x;
				Led_Assert (x.y == vpOrg.y);
				return Led_TWIPS (::MulDiv (x.x, 1440, fLogPixelsH));
			#else
				return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
//				return Led_TWIPS (from * 20);	// assume 72dpi on mac
			#endif
		}
#if		qMacOS
		inline	void	Led_Tablet_::SetPort ()
			{
				::SetPort (fGrafPort);
			}
#elif	qWindows
		inline	BOOL Led_Tablet_::BitBlt(int x, int y, int nWidth, int nHeight, Led_Tablet_* pSrcDC, int xSrc, int ySrc, DWORD dwRop)
			{ 
				Led_AssertNotNil (m_hDC); 
				return ::BitBlt(m_hDC, x, y, nWidth, nHeight, pSrcDC->m_hDC, xSrc, ySrc, dwRop);
			}
		inline	BOOL Led_Tablet_::CreateCompatibleDC (Led_Tablet_* pDC)
			{
				Led_Assert (m_hDC == NULL);      // only attach once, detach on destroy
				Led_Assert (m_hAttribDC == NULL);    // only attach to an empty DC

				m_hDC = ::CreateCompatibleDC (pDC == NULL? NULL: pDC->m_hDC);
				if (m_hDC == NULL)
					return FALSE;

				m_hAttribDC = m_hDC;
				fOwnsDC = eOwnsDC;
				return TRUE;
			}
		inline	COLORREF Led_Tablet_::SetTextColor (COLORREF crColor)
			{
				Led_Assert(m_hDC != NULL);
				COLORREF crRetVal = CLR_INVALID;

				if (m_hDC != m_hAttribDC)
					crRetVal = ::SetTextColor(m_hDC, crColor);
				if (m_hAttribDC != NULL)
					crRetVal = ::SetTextColor(m_hAttribDC, crColor);
				return crRetVal;
			}
		inline	COLORREF Led_Tablet_::SetBkColor(COLORREF crColor)
			{
				Led_Assert(m_hDC != NULL);
				COLORREF crRetVal = CLR_INVALID;

				if (m_hDC != m_hAttribDC)
					crRetVal = ::SetBkColor(m_hDC, crColor);
				if (m_hAttribDC != NULL)
					crRetVal = ::SetBkColor(m_hAttribDC, crColor);
				return crRetVal;
			}
		inline	HWND Led_Tablet_::GetWindow() const
			{
				Led_Assert (m_hDC != NULL);
				return ::WindowFromDC (m_hDC);
			}
		inline	BOOL Led_Tablet_::IsPrinting() const
			{
				return m_bPrinting;
			}
		inline	BOOL Led_Tablet_::RoundRect(int x1, int y1, int x2, int y2, int x3, int y3)
			{
				Led_Assert(m_hDC != NULL); 
				return ::RoundRect(m_hDC, x1, y1, x2, y2, x3, y3);
			}
		inline	BOOL Led_Tablet_::TextOut(int x, int y, LPCTSTR lpszString, int nCount)
			{
				Led_Assert(m_hDC != NULL);
				return ::TextOut(m_hDC, x, y, lpszString, nCount); 
			}
#if 0
		inline	SIZE	Led_Tablet_::GetTextExtent (LPCTSTR lpszString, int nCount) const
			{
				Led_Assert(m_hAttribDC != NULL);
				SIZE size;
				Led_Verify (::GetTextExtentPoint32 (m_hAttribDC, lpszString, nCount, &size));
				return size;
			}
#endif
		inline	int Led_Tablet_::SetBkMode(int nBkMode)
			{
				Led_Assert (m_hDC != NULL);
				int nRetVal = 0;

				if (m_hDC != m_hAttribDC)
					nRetVal = ::SetBkMode(m_hDC, nBkMode);
				if (m_hAttribDC != NULL)
					nRetVal = ::SetBkMode(m_hAttribDC, nBkMode);
				return nRetVal;
			}
		inline	unsigned int Led_Tablet_::SetTextAlign (unsigned int nTextAlign)
			{
				Led_Assert (m_hDC != NULL);
				unsigned int nRetVal = 0;

				if (m_hDC != m_hAttribDC)
					nRetVal = ::SetTextAlign (m_hDC, nTextAlign);
				if (m_hAttribDC != NULL)
					nRetVal = ::SetTextAlign (m_hAttribDC, nTextAlign);
				return nRetVal;
			}

		inline	SIZE Led_Tablet_::GetWindowExt() const
			{
				Led_Assert(m_hAttribDC != NULL);
				SIZE size;
				Led_Verify(::GetWindowExtEx(m_hAttribDC, &size));
				return size;
			}
		inline	SIZE Led_Tablet_::GetViewportExt() const
			{
				Led_Assert(m_hAttribDC != NULL);
				SIZE size;
				Led_Verify (::GetViewportExtEx(m_hAttribDC, &size));
				return size;
			}
		inline	BOOL Led_Tablet_::Rectangle(int x1, int y1, int x2, int y2)
			{ 
				 Led_Assert(m_hDC != NULL); 
				return ::Rectangle(m_hDC, x1, y1, x2, y2);
			}
		inline	BOOL Led_Tablet_::Rectangle(const RECT& r)
			{ 
				Led_Assert(m_hDC != NULL); 
				return ::Rectangle (m_hDC, r.left, r.top, r.right, r.bottom);
			}
		inline	BOOL Led_Tablet_::Rectangle(LPCRECT lpRect)
			{ 
				Led_Assert(m_hDC != NULL); 
				return ::Rectangle (m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
			}
		inline	BOOL Led_Tablet_::GetTextMetrics(LPTEXTMETRIC lpMetrics) const
			{ 
				Led_Assert(m_hAttribDC != NULL);
				return ::GetTextMetrics(m_hAttribDC, lpMetrics);
			}
		inline	HBITMAP	Led_Tablet_::SelectObject (HBITMAP hBitmap)
			{
				Led_Assert (m_hDC != NULL);
				return (HBITMAP)::SelectObject (m_hDC, hBitmap);
			}
	#if		STRICT
		inline	HFONT	Led_Tablet_::SelectObject (HFONT hFont)
			{
				Led_Assert (m_hDC != NULL);
				return (HFONT)::SelectObject (m_hDC, hFont);
			}
	#endif
		inline	POINT Led_Tablet_::SetWindowOrg(int x, int y)
			{
				Led_Assert (m_hDC != NULL);
				POINT point;

				if (m_hDC != m_hAttribDC)
					Led_Verify (::SetWindowOrgEx(m_hDC, x, y, &point));
				if (m_hAttribDC != NULL)
					Led_Verify (::SetWindowOrgEx(m_hAttribDC, x, y, &point));
				return point;
			}
		inline	int	Led_Tablet_::GetDeviceCaps (int nIndex) const
			{
				Led_Assert (m_hAttribDC != NULL);
				return ::GetDeviceCaps(m_hAttribDC, nIndex);
			}
		inline	BOOL Led_Tablet_::Attach (HDC hDC, Led_Tablet_::OwnDCControl ownsDC)
			{
				Led_Assert(m_hDC == NULL);      // only attach once, detach on destroy
				Led_Assert(m_hAttribDC == NULL);    // only attach to an empty DC

				if (hDC == NULL)
					return FALSE;

				m_hDC = hDC;
				m_hAttribDC = m_hDC;     // Default to same as output
				fOwnsDC = ownsDC;
				return TRUE;
			}
		inline	HDC Led_Tablet_::Detach ()
			{
				HDC hDC = m_hDC;
				m_hAttribDC = NULL;
				m_hDC = NULL;
				return hDC;
			}
#endif
	inline	void	Led_Tablet_::MoveTo (const Led_Point& to)
		{
			#if		qMacOS
				::MoveTo (to.h, to.v);
			#elif	qWindows
				Led_Assert(m_hDC != NULL);
				Led_Verify (::MoveToEx (m_hDC, to.h, to.v, NULL));
			#elif	qXWindows
				fCurDrawLineLoc = to;
			#endif
		}
	inline	void	Led_Tablet_::LineTo (const Led_Point& to)
		{
			#if		qMacOS
				::LineTo (to.h, to.v);
			#elif	qWindows
				Led_Assert(m_hDC != NULL);
				Led_Verify (::LineTo (m_hDC, to.h, to.v));
			#elif	qXWindows
				::XDrawLine (fDisplay, fDrawable, fGC, fCurDrawLineLoc.h, fCurDrawLineLoc.v, to.h, to.v);
				fCurDrawLineLoc = to;
			#endif
		}
	inline	Led_Region	Led_Tablet_::GetClip () const
		{
			Led_Region	result;
			#if		qMacOS
				const_cast<Led_Tablet_*> (this)->SetPort ();
				::GetClip (result.GetOSRep ());
			#elif	qWindows
				int r =  ::GetClipRgn (*this, result);
				Led_Assert (r == 0 or r == 1 or r == -1);
				if (r == 0) {
					#if		!qInternalErrorWithStaticRegionDeclaredInFunction
						static
					#endif
					Led_Region	kWideOpened		=	Led_Region (Led_Rect (-10000, -10000, 20000, 20000));
					result = kWideOpened;
				}
			#else
				Led_Assert (false);		// NYI
			#endif
			return result;
		}
	inline	bool	Led_Tablet_::GetClip (Led_Region* r) const
		{
			Led_RequireNotNil (r);
			#if		qMacOS
				const_cast<Led_Tablet_*> (this)->SetPort ();
				::GetClip (r->GetOSRep ());
				return true;
			#elif	qWindows
				int res =  ::GetClipRgn (*this, *r);
				Led_Assert (res == 0 or res == 1 or res == -1);
				if (res == 0) {
					return false;
				}
				return true;
			#else
				Led_Assert (false);		// NYI
				return false;
			#endif
		}
	inline	void	Led_Tablet_::SetClip ()
		{
			#if		qMacOS
				SetPort ();
				static	Led_Region	kWideOpened	=	Led_Region (Led_Rect (-10000, -10000, 20000, 20000));
				::SetClip (kWideOpened.GetOSRep ());
			#elif	qWindows
				Led_Verify (::SelectClipRgn (*this, NULL) != ERROR);
			#elif	qXWindows
				static	Led_Rect	kWideOpened	=	Led_Rect (-10000, -10000, 20000, 20000);
				XRectangle xrectangle	=	AsXRect (kWideOpened);
				::XSetClipRectangles (fDisplay, fGC, 0, 0, &xrectangle, 1, Unsorted);
			#else
				Led_Assert (false);		// NYI
			#endif
		}
	inline	void	Led_Tablet_::SetClip (const Led_Rect& clipTo)
		{
			#if		qMacOS
				SetPort ();
				::SetClip (Led_Region (clipTo).GetOSRep ());
			#elif	qWindows
				Led_Verify (::SelectClipRgn (*this, Led_Region (clipTo)) != ERROR);
				Led_Ensure (GetClip ().GetBoundingRect () == clipTo);
			#elif	qXWindows
				XRectangle xrectangle	=	AsXRect (clipTo);
				::XSetClipRectangles (fDisplay, fGC, 0, 0, &xrectangle, 1, Unsorted);
			#else
				Led_Assert (false);		// NYI
			#endif
		}
	inline	void	Led_Tablet_::SetClip (const Led_Region& clipTo)
		{
			#if		qMacOS
				SetPort ();
				::SetClip (clipTo.GetOSRep ());
			#elif	qWindows
				Led_Verify (::SelectClipRgn (*this, clipTo) != ERROR);
			#else
				Led_Assert (false);		// NYI
			#endif
		}








	//class	Led_GDIGlobals
		inline	Led_GDIGlobals&	Led_GDIGlobals::Get ()
			{
				if (sThe == NULL) {
					sThe = new Led_GDIGlobals ();
				}
				return *sThe;
			}
		inline	Led_Distance	Led_GDIGlobals::GetMainScreenLogPixelsH () const
			{
				return fLogPixelsH;
			}
		inline	Led_Distance	Led_GDIGlobals::GetMainScreenLogPixelsV () const
			{
				return fLogPixelsV;
			}







#if		qSupportLed30CompatAPI
	inline	Led_Point	GetRectOrigin (const Led_Rect& r)
		{
			return r.GetOrigin ();
		}
	inline	Led_Size	GetRectSize (const Led_Rect& r)
		{
			return r.GetSize ();
		}
	inline	Led_Distance	GetRectWidth (const Led_Rect& r)
		{
			return r.GetWidth ();
		}
	inline	Led_Distance	GetRectHeight (const Led_Rect& r)
		{
			return r.GetHeight ();
		}
	inline	Led_Point		GetRectTopLeft (const Led_Rect& r)
		{
			return r.GetTopLeft ();
		}
	inline	Led_Point		GetRectTopRight (const Led_Rect& r)
		{
			return r.GetTopRight ();
		}
	inline	Led_Point		GetRectBottomRight (const Led_Rect& r)
		{
			return r.GetBotRight ();
		}
	inline	Led_Point		GetRectBottomLeft (const Led_Rect& r)
		{
			return (Led_Point (r.GetBottom (), r.GetLeft ()));
		}
#endif





	inline	Led_Rect	operator- (const Led_Rect& lhs, const Led_Point& rhs)
		{
			return (Led_Rect (lhs.GetTop () - rhs.v, lhs.GetLeft () - rhs.h, lhs.GetHeight (), lhs.GetWidth ()));
		}
	inline	Led_Rect	operator+ (Led_Point p, Led_Rect r)
		{
			return (Led_Rect (r.GetTop () + p.v, r.GetLeft () + p.h, r.GetHeight (), r.GetWidth ()));
		}
	inline	Led_Rect	operator+ (Led_Rect r, Led_Point p)
		{
			return (Led_Rect (r.GetTop () + p.v, r.GetLeft () + p.h, r.GetHeight (), r.GetWidth ()));
		}
	
	
	inline	bool	Intersect (const Led_Rect& lhs, const Led_Rect& rhs)
		{
			#if		qMacOS && qDebug
				bool	gdiResult;
				{
					Rect	mLHS;
					mLHS.top = lhs.top;
					mLHS.left = lhs.left;
					mLHS.bottom = lhs.bottom;
					mLHS.right = lhs.right;
					Rect	mRHS;
					mRHS.top = rhs.top;
					mRHS.left = rhs.left;
					mRHS.bottom = rhs.bottom;
					mRHS.right = rhs.right;
					Rect	ignored;
					gdiResult	= ::SectRect (&mLHS, &mRHS, &ignored);
				}
			#endif
			if (rhs.GetTop () >= lhs.GetBottom ()) {
				#if		qMacOS
					Led_Assert (not gdiResult);
				#endif
				return (false);
			}
			else if (rhs.GetBottom () <= lhs.GetTop ()) {
				#if		qMacOS
					Led_Assert (not gdiResult);
				#endif
				return (false);
			}
			else if (rhs.GetLeft () >= lhs.GetRight ()) {
				#if		qMacOS
					Led_Assert (not gdiResult);
				#endif
				return (false);
			}
			else if (rhs.GetRight () <= lhs.GetLeft ()) {
				#if		qMacOS
					Led_Assert (not gdiResult);
				#endif
				return (false);
			}

			if (rhs.GetHeight () == 0 or rhs.GetWidth () == 0 or lhs.GetHeight () == 0 or lhs.GetWidth () == 0) {
				#if		qMacOS
					Led_Assert (not gdiResult);
				#endif
				return false;
			}

			#if		qMacOS
				Led_Assert (gdiResult);
			#endif
			return (true);
		}
	inline	bool	Intersect (const Led_Rect& lhs, const Led_Region& rhs)
		{
			#if		qMacOS
				static	RgnHandle	result	=	::NewRgn ();
				static	RgnHandle	lhsRgn	=	::NewRgn ();
				::SetRectRgn (lhsRgn, (short)lhs.left, (short)lhs.top, (short)lhs.right, (short)lhs.bottom);
				::SectRgn (lhsRgn, rhs.GetOSRep (), result);
				return not ::EmptyRgn (result);
			#elif	qWindows
				Led_Region	lhsRgn =	lhs;
				Led_Region	result;
				return result.CombineRgn (&lhsRgn, const_cast<Led_Region*> (&rhs), RGN_AND) != NULLREGION;
			#endif
		}
	inline	bool	Intersect (const Led_Region& lhs, const Led_Rect& rhs)
		{
			#if		qMacOS
				static	RgnHandle	result	=	::NewRgn ();
				static	RgnHandle	rhsRgn	=	::NewRgn ();
				::SetRectRgn (rhsRgn, (short)rhs.left, (short)rhs.top, (short)rhs.right, (short)rhs.bottom);
				::SectRgn (lhs.GetOSRep (), rhsRgn, result);
				return not ::EmptyRgn (result);
			#elif	qWindows
				Led_Region	rhsRgn	=	rhs;
				Led_Region	result;
				return result.CombineRgn (const_cast<Led_Region*> (&lhs), &rhsRgn, RGN_AND) != NULLREGION;
			#endif
		}
	inline	bool	Intersect (const Led_Region& lhs, const Led_Region& rhs)
		{
			#if		qMacOS
				static	RgnHandle	result	=	::NewRgn ();
				::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result);
				return not ::EmptyRgn (result);
			#elif	qWindows
				Led_Region	result;
				return result.CombineRgn (const_cast<Led_Region*> (&lhs), const_cast<Led_Region*> (&rhs), RGN_AND) != NULLREGION;
			#endif
		}

	inline	Led_Rect	Intersection (const Led_Rect& lhs, const Led_Rect& rhs)
		{
			Led_Rect	tmp	=	lhs;
			return tmp *= rhs;
		}
	inline	Led_Rect	operator* (const Led_Rect& lhs, const Led_Rect& rhs)
		{
			Led_Rect	tmp	=	lhs;
			return tmp *= rhs;
		}
	inline	Led_Size	operator+ (Led_Size lhs, Led_Size rhs)
		{
			return (Led_Size (lhs.v + rhs.v, lhs.h + rhs.h));
		}
	inline	Led_Size	operator* (int lhs, Led_Size rhs)
		{
			return (Led_Size (lhs * rhs.v, lhs * rhs.h));
		}
	/*
	@METHOD:		InsetRect
	@DESCRIPTION:	<p>Utility routine to convert shrink (if vBy/hBy posative), or expand (if negative) the given @'Led_Rect'.
		NB: This routine pins the minimum output rect size (in each dimention) to be zero.</p>
	*/
	inline	Led_Rect	InsetRect (const Led_Rect& r, int vBy, int hBy)
		{
			return Led_Rect (r.GetTop () + vBy, r.GetLeft () + hBy,
								Led_Max (0, long (r.GetHeight ()) - 2*vBy), Led_Max (0, long (r.GetWidth ()) - 2*hBy)
							);
		}
	/*
	@METHOD:		EnsureRectInRect
	@DESCRIPTION:	<p>Utility routine to ensure the first rect is entirely enclosed in the second (enclosing) rectangle.
				Pin the edges so it fits.</p>
	*/
	inline	Led_Rect	EnsureRectInRect (const Led_Rect& r, Led_Rect enlosingR)
		{
			Led_Distance	winWidth  = min (r.GetWidth (), enlosingR.GetWidth ());
			Led_Distance	winHeight = min (r.GetHeight (), enlosingR.GetHeight ());
			Led_Coordinate	winLeft   = max (r.GetLeft (), enlosingR.GetLeft ());
			Led_Coordinate	winTop	  = max (r.GetTop (), enlosingR.GetTop ());

			#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
				#pragma	warning (push)
				#pragma	warning (disable : 4018)
			#endif
			if ((winLeft + winWidth) > enlosingR.GetRight ()) {
				winLeft = enlosingR.GetRight () - winWidth;
			}
			if ((winTop + winHeight) > enlosingR.GetBottom ()) {
				winTop = enlosingR.GetBottom () - winHeight;
			}
			#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
				#pragma	warning (pop)
			#endif
			return Led_Rect (winTop, winLeft, winHeight, winWidth);
		}
	/*
	@METHOD:		EnsureRectOnScreen
	@DESCRIPTION:	<p>Utility routine to ensure the first rect (typically used for a window) fits on the sceen.
				Pin the edges so it fits. See also @'EnsureRectInRect'.</p>
	*/
	inline	Led_Rect	EnsureRectOnScreen (Led_Rect& r)
		{
			#if		qWindows
			   // Get the limits of the 'workarea'
				RECT rWorkArea;
				BOOL  bResult = SystemParametersInfo (SPI_GETWORKAREA, sizeof(RECT), &rWorkArea,  0); 
				if (!bResult) { 
					rWorkArea.left = rWorkArea.top = 0; 
					rWorkArea.right = GetSystemMetrics(SM_CXSCREEN); 
					rWorkArea.bottom = GetSystemMetrics(SM_CYSCREEN); 
				} 
				return EnsureRectInRect (r, AsLedRect (rWorkArea));
			#else
				Led_Arg_Unused (r);
				Led_Assert (false); // NYI -
				return Led_Rect (0,0,0,0);
			#endif
		}




	#if		qMacOS
		inline	Led_Point	AsLedPoint (Point p)
			{
				return Led_Point (p.v, p.h);
			}
		inline	Point		AsQDPoint (Led_Point p)
			{
				Point p2;
				p2.v = p.v;
				p2.h = p.h;
				return p2;
			}
		inline	Led_Rect	AsLedRect (Rect r)
			{
			  Led_Rect newR;
			  newR.top = r.top;
			  newR.left = r.left;
			  newR.bottom = r.bottom;
			  newR.right = r.right;
			  return newR;
			}
		inline	Rect	AsQDRect (Led_Rect r)
			{
				Rect	newR;			// SHOULD ASSERT NO OVERFLOWS!!!!
				newR.top = r.top;
				newR.left = r.left;
				newR.bottom = r.bottom;
				newR.right = r.right;
				return (newR);
			}
		inline	Led_Size	AsLedSize (Point s)
			{
				return Led_Size (s.v, s.h);
			}
		inline	Point		GetRectOrigin (const Rect& r)
			{
				Point p;
				p.v = r.top;
				p.h = r.left;
				return p;
			}
		inline	Point		GetRectSize (const Rect& r)
			{
				Point p;
				p.v = r.bottom - r.top;
				p.h = r.right-r.left;
				return p;
			}
		inline	short	GetRectWidth (const Rect& r)
			{
				return (r.right-r.left);
			}
		inline	short	GetRectHeight (const Rect& r)
			{
				return (r.bottom-r.top);
			}
	#elif	qWindows
		inline	Led_Point	AsLedPoint (POINT p)
			{
				return Led_Point (p.y, p.x);
			}
		inline	POINT		AsPOINT (Led_Point p)
			{
				POINT	newP;
				newP.x = p.h;
				newP.y = p.v;
				return newP;
			}
		inline	Led_Rect	AsLedRect (RECT r)
			{
				return Led_Rect (r.top, r.left, r.bottom-r.top, r.right-r.left);
			}
		inline	RECT		AsRECT (Led_Rect r)
			{
				RECT	newR;
				newR.top = r.GetTop ();
				newR.left = r.GetLeft ();
				newR.bottom = r.GetBottom ();
				newR.right = r.GetRight ();
				return newR;
			}
		inline	SIZE		AsSIZE (Led_Size s)
			{
				SIZE	result;
				result.cx = s.h;
				result.cy = s.v;
				return result;
			}
		inline	Led_Size	AsLedSize (SIZE s)
			{
				Led_Size	result;
				result.h = s.cx;
				result.v = s.cy;
				return result;
			}
	#elif	qXWindows
		inline	Led_Rect	AsLedRect (const XRectangle& r)
			{
				return Led_Rect (r.y, r.x, r.height, r.width);
			}
		inline	XRectangle	AsXRect (const Led_Rect& r)
			{
				XRectangle	newR;
				newR.x = r.GetLeft ();
				newR.y = r.GetTop ();
				newR.height = r.GetHeight ();
				newR.width = r.GetWidth ();
				return newR;
			}
	#endif






	/*
	@METHOD:		Led_CvtScreenPixelsToTWIPSV
	@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.
		<p>See also @'Led_CvtScreenPixelsToTWIPSH'.</p>
	*/
	inline	Led_TWIPS	Led_CvtScreenPixelsToTWIPSV (Led_Coordinate from)
		{
			#if		qWindows
				return Led_TWIPS (::MulDiv (from, 1440, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ()));
			#else
				return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
			#endif
		}
	/*
	@METHOD:		Led_CvtScreenPixelsToTWIPSH
	@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.
		<p>See also @'Led_CvtScreenPixelsToTWIPSV'.</p>
	*/
	inline	Led_TWIPS	Led_CvtScreenPixelsToTWIPSH (Led_Coordinate from)
		{
			#if		qWindows
				return Led_TWIPS (::MulDiv (from, 1440, Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ()));
			#else
				return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
			#endif
		}
	/*
	@METHOD:		Led_CvtScreenPixelsFromTWIPSV
	@DESCRIPTION:	<p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
		<p>See also @'Led_CvtScreenPixelsFromTWIPSH'.</p>
	*/
	inline	Led_Coordinate	Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS from)
		{
			#if		qWindows
				return ::MulDiv (from, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV (), 1440);
			#else
				return Led_TWIPS (from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsV () / 1440);
			#endif
		}
	/*
	@METHOD:		Led_CvtScreenPixelsFromTWIPSH
	@DESCRIPTION:	<p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
		<p>See also @'Led_CvtScreenPixelsFromTWIPSV'.</p>
	*/
	inline	Led_Coordinate	Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS from)
		{
			#if		qWindows
				return ::MulDiv (from, Led_GDIGlobals::Get ().GetMainScreenLogPixelsH (), 1440);
			#else
				return Led_TWIPS (from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsH () / 1440);
			#endif
		}





//	class	Led_FontMetrics
	inline	Led_FontMetrics::Led_FontMetrics ():
		fPlatformSpecific ()
		{
			(void)::memset (&fPlatformSpecific, 0, sizeof (fPlatformSpecific));
		}
	#if		qMacOS
		inline	Led_FontMetrics::Led_FontMetrics (const FontInfo& from):
			fPlatformSpecific (from)
			{
			}
	#elif	qWindows
		inline	Led_FontMetrics::Led_FontMetrics (const TEXTMETRIC& from):
			fPlatformSpecific (from)
			{
			}
	#elif	qXWindows
		inline	Led_FontMetrics::Led_FontMetrics (const Led_FontMetrics::PlatformSpecific& from):
			fPlatformSpecific (from)
			{
			}
	#endif
	inline	Led_FontMetrics::Led_FontMetrics (const Led_FontMetrics& from):
		fPlatformSpecific (from.fPlatformSpecific)
		{
		}
	inline	const Led_FontMetrics& Led_FontMetrics::operator= (const Led_FontMetrics& rhs)
		{
			fPlatformSpecific = rhs.fPlatformSpecific;
			return *this;
		}
	inline	Led_Distance	Led_FontMetrics::GetAscent () const
		{
			#if		qMacOS
				return (fPlatformSpecific.ascent);
			#elif	qWindows
				return (fPlatformSpecific.tmAscent);
			#elif	qXWindows
				return fPlatformSpecific.fAscent;
			#endif
		}
	inline	Led_Distance	Led_FontMetrics::GetDescent () const
		{
			#if		qMacOS
				return (fPlatformSpecific.descent);
			#elif	qWindows
				return (fPlatformSpecific.tmDescent);
			#elif	qXWindows
				return fPlatformSpecific.fDescent;
			#endif
		}
	inline	Led_Distance	Led_FontMetrics::GetLeading () const
		{
			#if		qMacOS
				return (fPlatformSpecific.leading);
			#elif	qWindows
				return (fPlatformSpecific.tmExternalLeading);
			#elif	qXWindows
				return (fPlatformSpecific.fLeading);
			#endif
		}
	inline	Led_Distance	Led_FontMetrics::GetHeight () const
		{
			#if		qWindows
				Led_Assert (fPlatformSpecific.tmHeight >= 0);
				Led_Assert (GetAscent () + GetDescent () == Led_Distance (fPlatformSpecific.tmHeight));
			#endif
			return (GetAscent () + GetDescent ());
		}
	inline	Led_Distance	Led_FontMetrics::GetLineHeight () const
		{
			return (GetAscent () + GetDescent () + GetLeading ());
		}
	inline	nonvirtual	Led_Distance	Led_FontMetrics::GetMaxCharacterWidth () const
		{
			#if		qMacOS
				return (fPlatformSpecific.widMax);
			#elif	qWindows
				return (fPlatformSpecific.tmMaxCharWidth);
			#elif	qXWindows
				return (fPlatformSpecific.fMaxCharWidth);
			#endif
		}
	#if		qWindows
	inline	nonvirtual	Led_Distance	Led_FontMetrics::GetAveCharacterWidth () const
		{
			return (fPlatformSpecific.tmAveCharWidth);
		}
	#endif
	#if		qMacOS
	inline	Led_FontMetrics::operator const FontInfo* () const
		{
			return (&fPlatformSpecific);
		}
	inline	Led_FontMetrics::operator FontInfo* ()
		{
			return (&fPlatformSpecific);
		}
	#elif	qWindows
	inline	Led_FontMetrics::operator const TEXTMETRIC* () const
		{
			return (&fPlatformSpecific);
		}
	inline	Led_FontMetrics::operator TEXTMETRIC* ()
		{
			return (&fPlatformSpecific);
		}
	#endif



//	class	Led_Color
	inline	Led_Color::Led_Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue):
		fRed (redValue),
		fGreen (greenValue),
		fBlue (blueValue)
		{
		}
	#if		qMacOS
	inline	Led_Color::Led_Color (const RGBColor& rgbColor):
		fRed (rgbColor.red),
		fGreen (rgbColor.green),
		fBlue (rgbColor.blue)
		{
		}
	#elif	qWindows
	inline	Led_Color::Led_Color (COLORREF colorRef):
		fRed (static_cast<ColorValue> (GetRValue (colorRef)) << 8),
		fGreen (static_cast<ColorValue> (GetGValue (colorRef)) << 8),
		fBlue (static_cast<ColorValue> (GetBValue (colorRef)) << 8)
		{
		}
	#endif
	inline	Led_Color::ColorValue	Led_Color::GetRed () const
		{
			return fRed;
		}
	inline	Led_Color::ColorValue	Led_Color::GetGreen () const
		{
			return fGreen;
		}
	inline	Led_Color::ColorValue	Led_Color::GetBlue () const
		{
			return fBlue;
		}
	#if		qMacOS
	inline	RGBColor	Led_Color::GetOSRep () const
		{
			RGBColor	r;
			r.red = fRed;
			r.green = fGreen;
			r.blue = fBlue;
			return r;
		}
	#elif	qWindows
	inline	COLORREF	Led_Color::GetOSRep () const
		{
			return RGB (fRed >> 8, fGreen >> 8, fBlue >> 8);
		}
	#endif
	inline	bool	operator== (Led_Color lhs, Led_Color rhs)
		{
			return (lhs.GetRed () == rhs.GetRed () and lhs.GetGreen () == rhs.GetGreen () and lhs.GetBlue () == rhs.GetBlue ());
		}
	inline	bool	operator!= (Led_Color lhs, Led_Color rhs)
		{
			return not (lhs == rhs);
		}
	inline	Led_Color	operator* (Led_Color lhs, float factor)
		{
			typedef	Led_Color::ColorValue	CV;
			return Led_Color (
					static_cast<CV> (lhs.GetRed () * factor),
					static_cast<CV> (lhs.GetGreen () * factor),
					static_cast<CV> (lhs.GetBlue () * factor)
				);
		}
	inline	Led_Color	operator/ (Led_Color lhs, float divBy)
		{
			return Led_Color (static_cast<Led_Color::ColorValue> (lhs.GetRed () / divBy), static_cast<Led_Color::ColorValue> (lhs.GetGreen () / divBy), static_cast<Led_Color::ColorValue> (lhs.GetBlue () / divBy));
		}
	inline	Led_Color	operator+ (Led_Color lhs, Led_Color rhs)
		{
			return Led_Color (lhs.GetRed () + rhs.GetRed (), lhs.GetGreen () + rhs.GetGreen (), lhs.GetBlue () + rhs.GetBlue ());
		}
	inline	Led_Color	operator- (Led_Color lhs, Led_Color rhs)
		{
			return Led_Color (lhs.GetRed () - rhs.GetRed (), lhs.GetGreen () - rhs.GetGreen (), lhs.GetBlue () - rhs.GetBlue ());
		}
	inline	unsigned int	Distance_Squared (Led_Color lhs, Led_Color rhs)
		{
			int	rDiff	=	static_cast<int> (lhs.GetRed ()) - static_cast<int> (rhs.GetRed ());
			int	gDiff	=	static_cast<int> (lhs.GetGreen ()) - static_cast<int> (rhs.GetGreen ());
			int	bDiff	=	static_cast<int> (lhs.GetBlue ()) - static_cast<int> (rhs.GetBlue ());
			unsigned	int	sum	=	rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
			return sum;
		}
	inline	unsigned int	Distance (Led_Color lhs, Led_Color rhs)
		{
			return static_cast<unsigned int> (::sqrt (static_cast<float> (Distance_Squared (lhs, rhs))));
		}
#if		qWindows
	inline	unsigned int	Distance_Squared (COLORREF lhs, COLORREF rhs)
		{
			int	rDiff	=	static_cast<int> (GetRValue (lhs)) - static_cast<int> (GetRValue (rhs));
			int	gDiff	=	static_cast<int> (GetGValue (lhs)) - static_cast<int> (GetGValue (rhs));
			int	bDiff	=	static_cast<int> (GetBValue (lhs)) - static_cast<int> (GetBValue (rhs));
			unsigned	int	sum	=	rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
			return sum;
		}
#endif


	#if		qLedUsesNamespaces
	}
	namespace std {
	#endif
	template<>
		#if		qLedUsesNamespaces
		struct less<Led::Led_Color> : public binary_function<Led::Led_Color, Led::Led_Color, bool> {
			bool operator()(const Led::Led_Color& _Left, const Led::Led_Color& _Right) const
		#else
		struct less<Led_Color> : public binary_function<Led_Color, Led_Color, bool> {
			bool operator()(const Led_Color& _Left, const Led_Color& _Right) const
		#endif
				{
					if (_Left.GetRed () < _Right.GetRed ()) {
						return true;
					}
					else if (_Left.GetRed () == _Right.GetRed ()) {
						if (_Left.GetGreen () < _Right.GetGreen ()) {
							return true;
						}
						else if (_Left.GetGreen () == _Right.GetGreen ()) {
							return (_Left.GetBlue () < _Right.GetBlue ());
						}
					}
					return false;
				}
			};
	#if		qLedUsesNamespaces
	}
	namespace	Led {
	#endif







//	class	Led_FontSpecification
	#if		qWindows
		inline	Led_FontSpecification::FontNameSpecifier::FontNameSpecifier ()
			{
				fName[0] = '\0';
			}
		inline	Led_FontSpecification::FontNameSpecifier::FontNameSpecifier (const Led_SDK_Char* from)
			{
				(void)::_tcsncpy (fName, from, LF_FACESIZE);
				fName[LF_FACESIZE-1] = '\0';
			}
		inline	bool	operator== (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs)
			{
				return (::_tcscmp (lhs.fName, rhs.fName) == 0);
			}
		inline	bool	operator!= (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs)
			{
				return (::_tcscmp (lhs.fName, rhs.fName) != 0);
			}
	#endif
	inline	Led_FontSpecification::Led_FontSpecification ():
		#if		qMacOS
			fFontSpecifier (0),
			fFontSize (0),
			fFontStyle (0),
		#elif	qWindows
			fFontInfo (),
		#elif	qXWindows
			fFontFamily (),
			fBold (false),
			fItalics (false),
			fUnderline (false),
			fFontSize (0),
		#endif
			fSubOrSuperScript (eNoSubOrSuperscript),
			fTextColor (Led_Color::kBlack)
		{
			#if		qWindows
				(void)::memset (&fFontInfo, 0, sizeof (fFontInfo));
			#endif
		}
	inline	Led_FontSpecification::Led_FontSpecification (const Led_IncrementalFontSpecification& from):
		#if		qMacOS
			fFontSpecifier (((const Led_FontSpecification&)from).fFontSpecifier),
			fFontSize (((const Led_FontSpecification&)from).fFontSize),
			fFontStyle (((const Led_FontSpecification&)from).fFontStyle),
		#elif	qWindows
			fFontInfo (((const Led_FontSpecification&)from).fFontInfo),
		#elif	qXWindows
			fFontFamily (from.fFontFamily),
			fBold (from.fBold),
			fItalics (from.fItalics),
			fUnderline (from.fUnderline),
			fFontSize (from.fFontSize),
		#endif
			fSubOrSuperScript (((const Led_FontSpecification&)from).fSubOrSuperScript),
			fTextColor (((const Led_FontSpecification&)from).fTextColor)
		{
		}
	/*
	@METHOD:		Led_FontSpecification::GetFontName
	@DESCRIPTION:	<p>Retrieve the 'FontName' attribute of the given font specification. This name is a string, and corresponds to the font family name.</p>
					<p>See also @'Led_FontSpecification::SetFontName'.</p>
	*/
	inline	Led_SDK_String	Led_FontSpecification::GetFontName () const
		{
			#if		qMacOS
				Str255	fontName;
				::GetFontName (fFontSpecifier, fontName);
				size_t	len	=	fontName[0];
				return string ((const char*)&fontName[1], len);
			#elif	qWindows
				return fFontInfo.lfFaceName;
			#elif	qXWindows
				return fFontFamily;
			#endif
		}
	/*
	@METHOD:		Led_FontSpecification::SetFontName
	@DESCRIPTION:	<p>See also @'Led_FontSpecification::GetFontName'.</p>
	*/
	inline	void	Led_FontSpecification::SetFontName (const Led_SDK_String& fontName)
		{
			#if		qMacOS
				Str255	pFontName;
				pFontName[0] = fontName.length ();
				memcpy (&pFontName[1], fontName.c_str (), pFontName[0]);
				short	fontNum	=	0;
				::GetFNum (pFontName, &fontNum);
				// Alas, the Mac font Manager returns ZERO as the font number if it really
				// has no idea about the font. This is NOT what we want. But unsure what we can do better at this point!
				fFontSpecifier = fontNum;
			#elif	qWindows
				const	size_t	kBufLen	=	sizeof (fFontInfo.lfFaceName)/sizeof (fFontInfo.lfFaceName[0]);
				(void)_tcsncpy (fFontInfo.lfFaceName, fontName.c_str (), kBufLen);
				fFontInfo.lfFaceName[kBufLen-1] = '\0';
				fFontInfo.lfCharSet = DEFAULT_CHARSET;
			#elif	qXWindows
				fFontFamily = fontName;
			#endif
		}
	// FontName info
	inline	Led_FontSpecification::FontNameSpecifier	Led_FontSpecification::GetFontNameSpecifier () const
		{
			#if		qMacOS
				return fFontSpecifier;
			#elif	qWindows
				return fFontInfo.lfFaceName;
			#elif	qXWindows
				return fFontFamily;
			#endif
		}
	inline	void	Led_FontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
		{
			#if		qMacOS
				fFontSpecifier = fontNameSpecifier;
			#elif	qWindows
				const	size_t	kBufLen	=	sizeof (fFontInfo.lfFaceName)/sizeof (fFontInfo.lfFaceName[0]);
				(void)::_tcsncpy (fFontInfo.lfFaceName, fontNameSpecifier.fName, kBufLen);
				fFontInfo.lfFaceName[kBufLen-1] = '\0';
				fFontInfo.lfCharSet = DEFAULT_CHARSET;
			#elif	qXWindows
				fFontFamily = fontNameSpecifier;
			#endif
		}
	// Style info
	/*
	@METHOD:		Led_FontSpecification::GetStyle_Plain
	@DESCRIPTION:	<p>Checks all the various font 'style' attributes, such as 'bold', or 'italic'. Returns true iff all
				of these styles are not set.</p>
				<p>See also @'Led_FontSpecification::SetStyle_Plain'.</p>
	*/
	inline	bool	Led_FontSpecification::GetStyle_Plain () const
		{
			if (fSubOrSuperScript != eNoSubOrSuperscript) {
				return false;
			}
			#if		qMacOS
				return (fFontStyle == 0);
			#elif	qWindows
				return (
						fFontInfo.lfItalic == false and
						fFontInfo.lfWeight <= FW_NORMAL and
						fFontInfo.lfUnderline == false and
						fFontInfo.lfStrikeOut == false
					);
			#elif	qXWindows
				return not fBold and not fItalics and not fUnderline;
			#endif
		}
	/*
	@METHOD:		Led_FontSpecification::SetStyle_Plain
	@DESCRIPTION:	<p>Clears all 'style' attributes, such as 'bold', or 'italic'.</p>
				<p>See also @'Led_FontSpecification::GetStyle_Plain'.</p>
	*/
	inline	void	Led_FontSpecification::SetStyle_Plain ()
		{
			fSubOrSuperScript = eNoSubOrSuperscript;
			#if		qMacOS
				fFontStyle = 0;
			#elif	qWindows
				fFontInfo.lfItalic = false;
				fFontInfo.lfWeight = FW_NORMAL;
				fFontInfo.lfUnderline = false;
				fFontInfo.lfStrikeOut = false;
			#elif	qXWindows
				fBold = false;
				fItalics = false;
				fUnderline = false;
			#endif
		}
	inline	bool	Led_FontSpecification::GetStyle_Bold () const
		{
			#if		qMacOS
				return (fFontStyle & bold);
			#elif	qWindows
				return (fFontInfo.lfWeight > FW_NORMAL);
			#elif	qXWindows
				return fBold;
			#endif
		}
	inline	void	Led_FontSpecification::SetStyle_Bold (bool isBold)
		{
			#if		qMacOS
				if (isBold) {
					fFontStyle |= bold;
				}
				else {
					fFontStyle &= ~bold;
				}
			#elif	qWindows
				fFontInfo.lfWeight = isBold? FW_BOLD: FW_NORMAL;
			#elif	qXWindows
				fBold = isBold;
			#endif
		}
	inline	bool	Led_FontSpecification::GetStyle_Italic () const
		{
			#if		qMacOS
				return (fFontStyle & italic);
			#elif	qWindows
				return fFontInfo.lfItalic;
			#elif	qXWindows
				return fItalics;
			#endif
		}
	inline	void	Led_FontSpecification::SetStyle_Italic (bool isItalic)
		{
			#if		qMacOS
				if (isItalic) {
					fFontStyle |= italic;
				}
				else {
					fFontStyle &= ~italic;
				}
			#elif	qWindows
				fFontInfo.lfItalic = isItalic;
			#elif	qXWindows
				fItalics = isItalic;
			#endif
		}
	inline	bool	Led_FontSpecification::GetStyle_Underline () const
		{
			#if		qMacOS
				return (fFontStyle & underline);
			#elif	qWindows
				return fFontInfo.lfUnderline;
			#elif	qXWindows
				return fUnderline;
			#endif
		}
	inline	void	Led_FontSpecification::SetStyle_Underline (bool isUnderline)
		{
			#if		qMacOS
				if (isUnderline) {
					fFontStyle |= underline;
				}
				else {
					fFontStyle &= ~underline;
				}
			#elif	qWindows
				fFontInfo.lfUnderline = isUnderline;
			#elif	qXWindows
				fUnderline = isUnderline;
			#endif
		}
	inline	Led_FontSpecification::SubOrSuperScript	Led_FontSpecification::GetStyle_SubOrSuperScript () const
		{
			return fSubOrSuperScript;
		}
	inline	void	Led_FontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
		{
			fSubOrSuperScript= subOrSuperScript;
		}
	#if		qMacOS
		inline	bool	Led_FontSpecification::GetStyle_Outline () const
			{
				return (fFontStyle & outline);
			}
		inline	void	Led_FontSpecification::SetStyle_Outline (bool isOutline)
			{
				if (isOutline) {
					fFontStyle |= outline;
				}
				else {
					fFontStyle &= ~outline;
				}
			}
		inline	bool	Led_FontSpecification::GetStyle_Shadow () const
			{
				return (fFontStyle & shadow);
			}
		inline	void	Led_FontSpecification::SetStyle_Shadow (bool isShadow)
			{
				if (isShadow) {
					fFontStyle |= shadow;
				}
				else {
					fFontStyle &= ~shadow;
				}
			}
		inline	bool	Led_FontSpecification::GetStyle_Condensed () const
			{
				return (fFontStyle & condense);
			}
		inline	void	Led_FontSpecification::SetStyle_Condensed (bool isCondensed)
			{
				if (isCondensed) {
					fFontStyle |= condense;
				}
				else {
					fFontStyle &= ~condense;
				}
			}
		inline	bool	Led_FontSpecification::GetStyle_Extended () const
			{
				return (fFontStyle & extend);
			}
		inline	void	Led_FontSpecification::SetStyle_Extended (bool isExtended)
			{
				if (isExtended) {
					fFontStyle |= extend;
				}
				else {
					fFontStyle &= ~extend;
				}
			}
	#elif	qWindows
		inline	bool	Led_FontSpecification::GetStyle_Strikeout () const
			{
				return fFontInfo.lfStrikeOut;
			}
		inline	void	Led_FontSpecification::SetStyle_Strikeout (bool isStrikeout)
			{
				fFontInfo.lfStrikeOut = isStrikeout;
			}
	#endif
	// FontSize info
	inline	unsigned short	Led_FontSpecification::GetPointSize () const
		{
			#if		qMacOS
				return fFontSize;
			#elif	qWindows
				if (fFontInfo.lfHeight >= 0) {
						// I probably should be doing some magic here with subtracing internal leading, or something like that from this value - 
						// See TextImager::GetStaticDefaultFont () and Win32 SDK docs for LOGFONT
						// LGP 960222
						Led_WindowDC	screenDC (NULL);
						Led_FontObject	font;
						Led_Verify (font.CreateFontIndirect (&fFontInfo));
						HFONT	oldFont	=	screenDC.SelectObject (font);
						TEXTMETRIC	tms;
						screenDC.GetTextMetrics (&tms);
						screenDC.SelectObject (oldFont);
						return (unsigned short)::MulDiv (tms.tmHeight, 72, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
				}
				else {
					return ::MulDiv (-fFontInfo.lfHeight, 72, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
				}
			#elif	qXWindows
				return fFontSize;
			#endif
		}
	inline	void	Led_FontSpecification::SetPointSize (unsigned short pointSize)
		{
			#if		qMacOS
				fFontSize = pointSize;
			#elif	qWindows
				fFontInfo.lfHeight = ::MulDiv (-long (pointSize), Led_GDIGlobals::Get ().GetMainScreenLogPixelsV (), 72);
			#elif	qXWindows
				fFontSize = pointSize;
			#endif
		}
	#if		qWindows
	inline		 long	Led_FontSpecification::PeekAtTMHeight () const
		{
			return fFontInfo.lfHeight;
		}
	inline	 void		Led_FontSpecification::PokeAtTMHeight (long tmHeight)
		{
			fFontInfo.lfHeight = tmHeight;
		}
	#endif
	inline	Led_Color	Led_FontSpecification::GetTextColor () const
		{
			return fTextColor;
		}
	inline	void	Led_FontSpecification::SetTextColor (const Led_Color& textColor)
		{
			fTextColor = textColor;
		}
	#if		qMacOS
		inline	void	Led_FontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
			{
				if (fontID != NULL) {
					*fontID = fFontSpecifier;
				}
				if (fontSize != NULL) {
					*fontSize = fFontSize;
				}
				if (fontStyle != NULL) {
					*fontStyle = fFontStyle;
				}
			}
		inline	void	Led_FontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
			{
				fFontSpecifier = fontID;
				fFontSize = fontSize;
				fFontStyle = fontStyle;
			}
	#elif	qWindows
		inline	LOGFONT	Led_FontSpecification::GetOSRep () const
			{
				return fFontInfo;
			}
		inline	void	Led_FontSpecification::GetOSRep (LOGFONT* logFont) const
			{
				Led_RequireNotNil (logFont);
				*logFont = fFontInfo;
			}
		inline	void	Led_FontSpecification::SetOSRep (LOGFONT logFont)
			{
				fFontInfo = logFont;
			}
		inline	void	Led_FontSpecification::LightSetOSRep (LOGFONT logFont)
			{
				fFontInfo = logFont;
				fFontInfo.lfWidth = 0;
				fFontInfo.lfEscapement = 0;
				fFontInfo.lfOrientation = 0;
				fFontInfo.lfCharSet = DEFAULT_CHARSET;
				fFontInfo.lfOutPrecision = 0;
				fFontInfo.lfClipPrecision = 0;
				fFontInfo.lfQuality = 0;
				fFontInfo.lfPitchAndFamily = 0;
			}
	#endif
	#if		qWindows
	inline	Led_FontSpecification::Led_FontSpecification (const LOGFONT& logFont):
			fFontInfo (),
			fSubOrSuperScript (eNoSubOrSuperscript),
			fTextColor (Led_Color::kBlack)
		{
			LightSetOSRep (logFont);
		}
	#endif
	inline	bool	operator== (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
		{
			// FontName Info
			if (lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ()) {
				return false;
			}

			// Style Info
			#if		qMacOS
			Style	lhsStyle;
			Style	rhsStyle;
			lhs.GetOSRep (NULL, NULL, &lhsStyle);
			rhs.GetOSRep (NULL, NULL, &rhsStyle);
			if (lhsStyle != rhsStyle) {
				return false;
			}
			#elif	qWindows
			if (lhs.GetStyle_Bold () != rhs.GetStyle_Bold ()) {
				return false;
			}
			if (lhs.GetStyle_Italic () != rhs.GetStyle_Italic ()) {
				return false;
			}
			if (lhs.GetStyle_Underline () != rhs.GetStyle_Underline ()) {
				return false;
			}
			if (lhs.GetStyle_Strikeout () != rhs.GetStyle_Strikeout ()) {
				return false;
			}
			#elif	qXWindows
			if (lhs.GetStyle_Bold () != rhs.GetStyle_Bold ()) {
				return false;
			}
			if (lhs.GetStyle_Italic () != rhs.GetStyle_Italic ()) {
				return false;
			}
			if (lhs.GetStyle_Underline () != rhs.GetStyle_Underline ()) {
				return false;
			}
			#endif
			if (lhs.GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ()) {
				return false;
			}

			// Color Info
			if (lhs.GetTextColor () != rhs.GetTextColor ()) {
				return false;
			}

			// Size Info
			#if		qWindows
				// Speed tweek to avoid divide and getdevicecaps crap...
				if (lhs.PeekAtTMHeight () == rhs.PeekAtTMHeight ()) {
					return true;
				}
				else if ((lhs.PeekAtTMHeight () > 0) == (rhs.PeekAtTMHeight () > 0)) {
					return false;	// if same sign, we can just compare for equality, and since they
									// ABOVE didn't compare equal, they must be different point sizes
									// (or at least very close depending a little on resoution...)
									// If their signs DIFFER, we must fall through into the scaling crap (GetPointSize).
				}
			#endif
			if (lhs.GetPointSize () != rhs.GetPointSize ()) {
				return false;
			}

			return true;
		}
	inline	bool	operator!= (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
		{
			return not (lhs == rhs);
		}








	//	class	Led_IncrementalFontSpecification
	inline	Led_IncrementalFontSpecification::Led_IncrementalFontSpecification ():
		Led_FontSpecification (),
		fFontSpecifierValid (false),
		fStyleValid_Bold (false),
		fStyleValid_Italic (false),
		fStyleValid_Underline (false),
		fStyleValid_SubOrSuperScript (false),
		#if		qMacOS
			fStyleValid_Outline (false),
			fStyleValid_Shadow (false),
			fStyleValid_Condensed (false),
			fStyleValid_Extended (false),
		#elif	qWindows
			fStyleValid_Strikeout (false),
			fDidSetOSRepCallFlag (false),
		#endif
		fFontSizeValid (false),
		fFontSizeIncrementValid (false),
		fTextColorValid (false)
		{
		}
	inline	Led_IncrementalFontSpecification::Led_IncrementalFontSpecification (const Led_FontSpecification& fontSpec):
		Led_FontSpecification (fontSpec),
		fFontSpecifierValid (true),
		fStyleValid_Bold (true),
		fStyleValid_Italic (true),
		fStyleValid_Underline (true),
		fStyleValid_SubOrSuperScript (true),
		#if		qMacOS
			fStyleValid_Outline (true),
			fStyleValid_Shadow (true),
			fStyleValid_Condensed (true),
			fStyleValid_Extended (true),
		#elif	qWindows
			fStyleValid_Strikeout (true),
			fDidSetOSRepCallFlag (true),
		#endif
		fFontSizeValid (true),
		fFontSizeIncrementValid (false),
		fTextColorValid (true)
		{
		}
	// FontName info
	inline	Led_FontSpecification::FontNameSpecifier	Led_IncrementalFontSpecification::GetFontNameSpecifier () const
		{
			Led_Require (fFontSpecifierValid);
			return inherited::GetFontNameSpecifier ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetFontNameSpecifier_Valid () const
		{
			return (fFontSpecifierValid);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateFontNameSpecifier ()
		{
			fFontSpecifierValid = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
		{
			fFontSpecifierValid = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetFontNameSpecifier (fontNameSpecifier);
		}
	inline	void	Led_IncrementalFontSpecification::SetFontName (const Led_SDK_String& fontName)
		{
			fFontSpecifierValid = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetFontName (fontName);
		}
	// Style info
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Plain () const
		{
			Led_Require (fStyleValid_Bold);
			Led_Require (fStyleValid_Italic);
			Led_Require (fStyleValid_Underline);
			Led_Require (fStyleValid_SubOrSuperScript);
			#if		qMacOS
				Led_Require (fStyleValid_Outline);
				Led_Require (fStyleValid_Shadow);
				Led_Require (fStyleValid_Condensed);
				Led_Require (fStyleValid_Extended);
			#elif	qWindows
				Led_Require (fStyleValid_Strikeout);
			#endif
			return inherited::GetStyle_Plain ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Plain_Valid () const
		{
			bool	isValid	=	fStyleValid_Bold and fStyleValid_Italic and fStyleValid_Underline and fStyleValid_SubOrSuperScript;
			#if		qMacOS
				isValid = isValid and fStyleValid_Outline and fStyleValid_Shadow and fStyleValid_Condensed and fStyleValid_Extended;
			#elif	qWindows
				isValid = isValid and fStyleValid_Strikeout;
			#endif
			return isValid;
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateStyles ()
		{
			fStyleValid_Bold = false;
			fStyleValid_Italic = false;
			fStyleValid_Underline = false;
			fStyleValid_SubOrSuperScript = false;
			#if		qMacOS
				fStyleValid_Outline = false;
				fStyleValid_Shadow = false;
				fStyleValid_Condensed = false;
				fStyleValid_Extended = false;
			#elif	qWindows
				fStyleValid_Strikeout = false;
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetStyle_Plain ()
		{
			fStyleValid_Bold = true;
			fStyleValid_Italic = true;
			fStyleValid_Underline = true;
			fStyleValid_SubOrSuperScript = true;
			#if		qMacOS
				fStyleValid_Outline = true;
				fStyleValid_Shadow = true;
				fStyleValid_Condensed = true;
				fStyleValid_Extended = true;
			#elif	qWindows
				fStyleValid_Strikeout = true;
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetStyle_Plain ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Bold () const
		{
			Led_Require (fStyleValid_Bold);
			return inherited::GetStyle_Bold ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Bold_Valid () const
		{
			return (fStyleValid_Bold);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Bold ()
		{
			fStyleValid_Bold = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetStyle_Bold (bool isBold)
		{
			fStyleValid_Bold = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetStyle_Bold (isBold);
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Italic () const
		{
			Led_Require (fStyleValid_Italic);
			return inherited::GetStyle_Italic ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Italic_Valid () const
		{
			return (fStyleValid_Italic);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Italic ()
		{
			fStyleValid_Italic = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetStyle_Italic (bool isItalic)
		{
			fStyleValid_Italic = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetStyle_Italic (isItalic);
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Underline () const
		{
			Led_Require (fStyleValid_Underline);
			return inherited::GetStyle_Underline ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_Underline_Valid () const
		{
			return (fStyleValid_Underline);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Underline ()
		{
			fStyleValid_Underline = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetStyle_Underline (bool isUnderline)
		{
			fStyleValid_Underline = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetStyle_Underline (isUnderline);
		}
	inline	Led_FontSpecification::SubOrSuperScript	Led_IncrementalFontSpecification::GetStyle_SubOrSuperScript () const
		{
			Led_Require (fStyleValid_SubOrSuperScript);
			return inherited::GetStyle_SubOrSuperScript ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetStyle_SubOrSuperScript_Valid () const
		{
			return (fStyleValid_SubOrSuperScript);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateStyle_SubOrSuperScript ()
		{
			fStyleValid_SubOrSuperScript = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
		{
			fStyleValid_SubOrSuperScript = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetStyle_SubOrSuperScript (subOrSuperScript);
		}
	#if		qMacOS
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Outline () const
			{
				Led_Require (fStyleValid_Outline);
				return (inherited::GetStyle_Outline ());
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Outline_Valid () const
			{
				return (fStyleValid_Outline);
			}
		inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Outline ()
			{
				fStyleValid_Outline = false;
			}
		inline	void	Led_IncrementalFontSpecification::SetStyle_Outline (bool isOutline)
			{
				fStyleValid_Outline = true;
				inherited::SetStyle_Outline (isOutline);
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Shadow () const
			{
				Led_Require (fStyleValid_Shadow);
				return (inherited::GetStyle_Shadow ());
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Shadow_Valid () const
			{
				return (fStyleValid_Shadow);
			}
		inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Shadow ()
			{
				fStyleValid_Shadow = false;
			}
		inline	void	Led_IncrementalFontSpecification::SetStyle_Shadow (bool isShadow)
			{
				fStyleValid_Shadow = true;
				inherited::SetStyle_Shadow (isShadow);
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Condensed () const
			{
				Led_Require (fStyleValid_Condensed);
				return (inherited::GetStyle_Condensed ());
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Condensed_Valid () const
			{
				return (fStyleValid_Condensed);
			}
		inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Condensed ()
			{
				fStyleValid_Condensed = false;
			}
		inline	void	Led_IncrementalFontSpecification::SetStyle_Condensed (bool isCondensed)
			{
				fStyleValid_Condensed = true;
				inherited::SetStyle_Condensed (isCondensed);
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Extended () const
			{
				Led_Require (fStyleValid_Extended);
				return (inherited::GetStyle_Extended ());
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Extended_Valid () const
			{
				return (fStyleValid_Extended);
			}
		inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Extended ()
			{
				fStyleValid_Extended = false;
			}
		inline	void	Led_IncrementalFontSpecification::SetStyle_Extended (bool isExtended)
			{
				fStyleValid_Extended = true;
				inherited::SetStyle_Extended (isExtended);
			}
	#elif	qWindows
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Strikeout () const
			{
				Led_Require (fStyleValid_Strikeout);
				return (inherited::GetStyle_Strikeout ());
			}
		inline	bool	Led_IncrementalFontSpecification::GetStyle_Strikeout_Valid () const
			{
				return (fStyleValid_Strikeout);
			}
		inline	void	Led_IncrementalFontSpecification::InvalidateStyle_Strikeout ()
			{
				fStyleValid_Strikeout = false;
				#if		qWindows
					fDidSetOSRepCallFlag = false;
				#endif
			}
		inline	void	Led_IncrementalFontSpecification::SetStyle_Strikeout (bool isStrikeout)
			{
				fStyleValid_Strikeout = true;
				#if		qWindows
					fDidSetOSRepCallFlag = false;
				#endif
				inherited::SetStyle_Strikeout (isStrikeout);
			}
	#endif
	// FontSize info
	inline	unsigned short	Led_IncrementalFontSpecification::GetPointSize () const
		{
			Led_Require (fFontSizeValid);
			Led_Require (not fFontSizeIncrementValid);
			return inherited::GetPointSize ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetPointSize_Valid () const
		{
			return (fFontSizeValid);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidatePointSize ()
		{
			fFontSizeValid = false;
			fFontSizeIncrementValid = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetPointSize (unsigned short pointSize)
		{
			fFontSizeValid = true;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetPointSize (pointSize);
			fFontSizeIncrementValid = false;
		}
	#if		qWindows
	inline	void	Led_IncrementalFontSpecification::PokeAtTMHeight (long tmHeight)
		{
			fFontSizeValid = true;
			fDidSetOSRepCallFlag = false;
			fFontSizeIncrementValid = false;
			inherited::PokeAtTMHeight (tmHeight);
		}
	#endif
	inline	short	Led_IncrementalFontSpecification::GetPointSizeIncrement () const
		{
			Led_Require (not fFontSizeValid);
			Led_Require (fFontSizeIncrementValid);
			return (short)inherited::GetPointSize ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetPointSizeIncrement_Valid () const
		{
			return (fFontSizeIncrementValid);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidatePointSizeIncrement ()
		{
			fFontSizeValid = false;
			fFontSizeIncrementValid = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
		}
	inline	void	Led_IncrementalFontSpecification::SetPointSizeIncrement (short pointSizeIncrement)
		{
			fFontSizeValid = false;
			#if		qWindows
				fDidSetOSRepCallFlag = false;
			#endif
			inherited::SetPointSize ((unsigned short)pointSizeIncrement);
			fFontSizeIncrementValid = true;
		}
	// FontFaceColor info
	inline	Led_Color	Led_IncrementalFontSpecification::GetTextColor () const
		{
			Led_Require (fTextColorValid);
			return inherited::GetTextColor ();
		}
	inline	bool	Led_IncrementalFontSpecification::GetTextColor_Valid () const
		{
			return (fTextColorValid);
		}
	inline	void	Led_IncrementalFontSpecification::InvalidateTextColor ()
		{
			fTextColorValid = false;
		}
	inline	void	Led_IncrementalFontSpecification::SetTextColor (const Led_Color& textColor)
		{
			fTextColorValid = true;
			inherited::SetTextColor (textColor);
		}
	#if		qMacOS
		inline	void	Led_IncrementalFontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
			{
				Led_Require (fFontSpecifierValid and
							 fStyleValid_Bold and
							 fStyleValid_Italic and
							 fStyleValid_Underline and
							 fFontSizeValid
							);
				Led_Require (fStyleValid_Outline and
							 fStyleValid_Shadow and
							 fStyleValid_Condensed and
							 fStyleValid_Extended
							);
				inherited::GetOSRep (fontID, fontSize, fontStyle);
			}
		inline	void	Led_IncrementalFontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
			{
				fFontSpecifierValid = true;
				fStyleValid_Bold = true;
				fStyleValid_Italic = true;
				fStyleValid_Underline = true;
				fStyleValid_Outline = true;
				fStyleValid_Shadow = true;
				fStyleValid_Condensed = true;
				fStyleValid_Extended = true;
				fFontSizeValid = true;
				fFontSizeIncrementValid = false;
				inherited::SetOSRep (fontID, fontSize, fontStyle);
			}
	#elif	qWindows
		inline	LOGFONT	Led_IncrementalFontSpecification::GetOSRep () const
			{
				Led_Require (fFontSpecifierValid and
							 fStyleValid_Bold and
							 fStyleValid_Italic and
							 fStyleValid_Underline and
							 fFontSizeValid
							);
				Led_Require (fStyleValid_Strikeout);
				return inherited::GetOSRep ();
			}
		inline	void	Led_IncrementalFontSpecification::GetOSRep (LOGFONT* logFont) const
			{
				Led_RequireNotNil (logFont);
				Led_Require (fFontSpecifierValid and
							 fStyleValid_Bold and
							 fStyleValid_Italic and
							 fStyleValid_Underline and
							 fFontSizeValid
							);
				Led_Require (fStyleValid_Strikeout);
				inherited::GetOSRep (logFont);
			}
		inline	void	Led_IncrementalFontSpecification::SetOSRep (LOGFONT logFont)
			{
				fFontSpecifierValid = true;
				fStyleValid_Bold = true;
				fStyleValid_Italic = true;
				fStyleValid_Underline = true;
				fStyleValid_Strikeout = true;
				fFontSizeValid = true;
				fFontSizeIncrementValid = false;
				fDidSetOSRepCallFlag = true;
				inherited::SetOSRep (logFont);
			}
		inline	void	Led_IncrementalFontSpecification::LightSetOSRep (LOGFONT logFont)
			{
				fFontSpecifierValid = true;
				fStyleValid_Bold = true;
				fStyleValid_Italic = true;
				fStyleValid_Underline = true;
				fStyleValid_Strikeout = true;
				fFontSizeValid = true;
				fFontSizeIncrementValid = false;
				fDidSetOSRepCallFlag = true;
				inherited::LightSetOSRep (logFont);
			}
		inline	bool	Led_IncrementalFontSpecification::GetDidSetOSRepCallFlag () const
			{
				return fDidSetOSRepCallFlag;
			}
	#endif



	inline	void	Led_FontSpecification::MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes)
		{
			// Font Name
			if (addInTheseAttributes.GetFontNameSpecifier_Valid ()) {
				SetFontNameSpecifier (addInTheseAttributes.GetFontNameSpecifier ());
			}
			
			// Font Styles
			if (addInTheseAttributes.GetStyle_Bold_Valid ()) {
				SetStyle_Bold (addInTheseAttributes.GetStyle_Bold ());
			}
			if (addInTheseAttributes.GetStyle_Italic_Valid ()) {
				SetStyle_Italic (addInTheseAttributes.GetStyle_Italic ());
			}
			if (addInTheseAttributes.GetStyle_Underline_Valid ()) {
				SetStyle_Underline (addInTheseAttributes.GetStyle_Underline ());
			}
			if (addInTheseAttributes.GetStyle_SubOrSuperScript_Valid ()) {
				SetStyle_SubOrSuperScript (addInTheseAttributes.GetStyle_SubOrSuperScript ());
			}
			#if		qMacOS
				if (addInTheseAttributes.GetStyle_Outline_Valid ()) {
					SetStyle_Outline (addInTheseAttributes.GetStyle_Outline ());
				}
				if (addInTheseAttributes.GetStyle_Shadow_Valid ()) {
					SetStyle_Shadow (addInTheseAttributes.GetStyle_Shadow ());
				}
				if (addInTheseAttributes.GetStyle_Condensed_Valid ()) {
					SetStyle_Condensed (addInTheseAttributes.GetStyle_Condensed ());
				}
				if (addInTheseAttributes.GetStyle_Extended_Valid ()) {
					SetStyle_Extended (addInTheseAttributes.GetStyle_Extended ());
				}
			#elif	qWindows
				if (addInTheseAttributes.GetStyle_Strikeout_Valid ()) {
					SetStyle_Strikeout (addInTheseAttributes.GetStyle_Strikeout ());
				}
			#endif

			// Font Size
			if (addInTheseAttributes.GetPointSize_Valid ()) {
				#if		qWindows
					// speed tweek - avoid costly conversion to 'points'. All we want todo is copy the tmHeight field!
					PokeAtTMHeight (addInTheseAttributes.PeekAtTMHeight ());
				#else
					SetPointSize (addInTheseAttributes.GetPointSize ());
				#endif
			}
			if (addInTheseAttributes.GetPointSizeIncrement_Valid ()) {
				short	pointSize	=	GetPointSize ();
				pointSize += addInTheseAttributes.GetPointSizeIncrement ();
				if (pointSize <= 0) {	// never let point size get any smaller...
					pointSize = 1;
				}
				SetPointSize (pointSize);
			}

			// Text Color
			if (addInTheseAttributes.GetTextColor_Valid ()) {
				SetTextColor (addInTheseAttributes.GetTextColor ());
			}

			#if		qWindows
				// could have done somewhat earlier, but if so, must be more careful about what else gets changed... (like textcolor not part of this guy)
				if (addInTheseAttributes.GetDidSetOSRepCallFlag ()) {
					LOGFONT	lf;
					addInTheseAttributes.GetOSRep (&lf);
					SetOSRep (lf);
				}
			#endif
		}



	inline	void	Led_IncrementalFontSpecification::MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes)
		{
			// Font Name
			if (addInTheseAttributes.GetFontNameSpecifier_Valid ()) {
				SetFontNameSpecifier (addInTheseAttributes.GetFontNameSpecifier ());
			}

			// Font Styles
			if (addInTheseAttributes.GetStyle_Bold_Valid ()) {
				SetStyle_Bold (addInTheseAttributes.GetStyle_Bold ());
			}
			if (addInTheseAttributes.GetStyle_Italic_Valid ()) {
				SetStyle_Italic (addInTheseAttributes.GetStyle_Italic ());
			}
			if (addInTheseAttributes.GetStyle_Underline_Valid ()) {
				SetStyle_Underline (addInTheseAttributes.GetStyle_Underline ());
			}
			#if		qMacOS
				if (addInTheseAttributes.GetStyle_Outline_Valid ()) {
					SetStyle_Outline (addInTheseAttributes.GetStyle_Outline ());
				}
				if (addInTheseAttributes.GetStyle_Shadow_Valid ()) {
					SetStyle_Shadow (addInTheseAttributes.GetStyle_Shadow ());
				}
				if (addInTheseAttributes.GetStyle_Condensed_Valid ()) {
					SetStyle_Condensed (addInTheseAttributes.GetStyle_Condensed ());
				}
				if (addInTheseAttributes.GetStyle_Extended_Valid ()) {
					SetStyle_Extended (addInTheseAttributes.GetStyle_Extended ());
				}
			#elif	qWindows
				if (addInTheseAttributes.GetStyle_Strikeout_Valid ()) {
					SetStyle_Strikeout (addInTheseAttributes.GetStyle_Strikeout ());
				}
			#endif

			// Font Size
			if (addInTheseAttributes.GetPointSize_Valid ()) {
				#if		qWindows
					// speed tweek - avoid costly conversion to 'points'. All we want todo is copy the tmHeight field!
					PokeAtTMHeight (addInTheseAttributes.PeekAtTMHeight ());
				#else
					SetPointSize (addInTheseAttributes.GetPointSize ());
				#endif
			}
			if (addInTheseAttributes.GetPointSizeIncrement_Valid ()) {
				short	pointSize	=	GetPointSize ();
				pointSize += addInTheseAttributes.GetPointSizeIncrement ();
				if (pointSize <= 0) {	// never let point size get any smaller...
					pointSize = 1;
				}
				SetPointSize (pointSize);
			}

			// Text Color
			if (addInTheseAttributes.GetTextColor_Valid ()) {
				SetTextColor (addInTheseAttributes.GetTextColor ());
			}

			#if		qWindows
				fDidSetOSRepCallFlag = addInTheseAttributes.GetDidSetOSRepCallFlag ();
			#endif
		}

	inline	bool	operator== (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
		{
// Either make this non-portable, or somehow do some hack to make this test FASTER than it looks like
// it may be currently - profile??? - LGP 960517
//
			// FontName Info
			{
				if (lhs.GetFontNameSpecifier_Valid () != rhs.GetFontNameSpecifier_Valid ()) {
					return false;
				}
				if (lhs.GetFontNameSpecifier_Valid () and (lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ())) {
					return false;
				}
			}

			// Style Info
			{
				if (lhs.GetStyle_Bold_Valid () != rhs.GetStyle_Bold_Valid ()) {
					return false;
				}
				if (lhs.GetStyle_Bold_Valid () and (lhs.GetStyle_Bold () != rhs.GetStyle_Bold ())) {
					return false;
				}
			}
			{
				if (lhs.GetStyle_Italic_Valid () != rhs.GetStyle_Italic_Valid ()) {
					return false;
				}
				if (lhs.GetStyle_Italic_Valid () and (lhs.GetStyle_Italic () != rhs.GetStyle_Italic ())) {
					return false;
				}
			}
			{
				if (lhs.GetStyle_Underline_Valid () != rhs.GetStyle_Underline_Valid ()) {
					return false;
				}
				if (lhs.GetStyle_Underline_Valid () and (lhs.GetStyle_Underline () != rhs.GetStyle_Underline ())) {
					return false;
				}
			}
			{
				if (lhs.GetStyle_SubOrSuperScript_Valid () != rhs.GetStyle_SubOrSuperScript_Valid ()) {
					return false;
				}
				if (lhs.GetStyle_SubOrSuperScript_Valid () and (lhs.GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ())) {
					return false;
				}
			}
			#if		qMacOS
				{
					if (lhs.GetStyle_Outline_Valid () != rhs.GetStyle_Outline_Valid ()) {
						return false;
					}
					if (lhs.GetStyle_Outline_Valid () and (lhs.GetStyle_Outline () != rhs.GetStyle_Outline ())) {
						return false;
					}
				}
				{
					if (lhs.GetStyle_Shadow_Valid () != rhs.GetStyle_Shadow_Valid ()) {
						return false;
					}
					if (lhs.GetStyle_Shadow_Valid () and (lhs.GetStyle_Shadow () != rhs.GetStyle_Shadow ())) {
						return false;
					}
				}
				{
					if (lhs.GetStyle_Condensed_Valid () != rhs.GetStyle_Condensed_Valid ()) {
						return false;
					}
					if (lhs.GetStyle_Condensed_Valid () and (lhs.GetStyle_Condensed () != rhs.GetStyle_Condensed ())) {
						return false;
					}
				}
				{
					if (lhs.GetStyle_Extended_Valid () != rhs.GetStyle_Extended_Valid ()) {
						return false;
					}
					if (lhs.GetStyle_Extended_Valid () and (lhs.GetStyle_Extended () != rhs.GetStyle_Extended ())) {
						return false;
					}
				}
			#elif	qWindows
				{
					if (lhs.GetStyle_Strikeout_Valid () != rhs.GetStyle_Strikeout_Valid ()) {
						return false;
					}
					if (lhs.GetStyle_Strikeout_Valid () and (lhs.GetStyle_Strikeout () != rhs.GetStyle_Strikeout ())) {
						return false;
					}
				}
			#endif



			// Font Color Info
			{
				if (lhs.GetTextColor_Valid () != rhs.GetTextColor_Valid ()) {
					return false;
				}
				if (lhs.GetTextColor_Valid () and (lhs.GetTextColor () != rhs.GetTextColor ())) {
					return false;
				}
			}


			// Size Info
			{
				if (lhs.GetPointSizeIncrement_Valid () != rhs.GetPointSizeIncrement_Valid ()) {
					return false;
				}
				if (lhs.GetPointSizeIncrement_Valid () and (lhs.GetPointSizeIncrement () != rhs.GetPointSizeIncrement ())) {
					return false;
				}
			}
			{
				if (lhs.GetPointSize_Valid () != rhs.GetPointSize_Valid ()) {
					return false;
				}
				if (lhs.GetPointSize_Valid ()) {
					#if		qWindows
						// Speed tweek to avoid divide and getdevicecaps crap...
						if (lhs.PeekAtTMHeight () == rhs.PeekAtTMHeight ()) {
							return true;
						}
						else if ((lhs.PeekAtTMHeight () > 0) == (rhs.PeekAtTMHeight () > 0)) {
							return false;	// if same sign, we can just compare for equality, and since they
											// ABOVE didn't compare equal, they must be different point sizes
											// (or at least very close depending a little on resoution...)
											// If their signs DIFFER, we must fall through into the scaling crap (GetPointSize).
						}
					#endif
					if (lhs.GetPointSize () != rhs.GetPointSize ()) {
						return false;
					}
				}
			}

			return true;
		}
	inline	bool	operator!= (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
		{
			return not (lhs == rhs);
		}




//	class	Led_InstalledFonts
	inline	const vector<Led_SDK_String>&	Led_InstalledFonts::GetUsableFontNames () const
		{
			return fFontNames;
		}





	inline	Led_Color	Led_GetTextColor ()
		{
			#if		qMacOS
				return (Led_Color::kBlack);
			#elif	qWindows
				return Led_Color (::GetSysColor (COLOR_WINDOWTEXT));
			#elif	qXWindows
				return (Led_Color::kBlack);
			#endif
		}
	inline	Led_Color	Led_GetTextBackgroundColor ()
		{
			#if		qMacOS
				return (Led_Color::kWhite);
			#elif	qWindows
				return Led_Color (::GetSysColor (COLOR_WINDOW));
			#elif	qXWindows
				return (Led_Color::kWhite);
			#endif
		}
	inline	Led_Color	Led_GetSelectedTextColor ()
		{
			#if		qMacOS
				RGBColor	hiliteRGBValue;
				LMGetHiliteRGB (&hiliteRGBValue);
				/*
				 *	This is based on empirical testing with the behavior of the TE in the Color desk accessory
				 *	that comes with system 7.5. I REALLY should probably do something with color intensity
				 *	matching -- LGP 950531
				 */
				if (Led_Color (hiliteRGBValue) == Led_Color::kBlack) {
					return (Led_Color::kWhite);
				}
				else {
					return (Led_Color::kBlack);
				}
			#elif	qWindows
	//			return Led_Color (::GetSysColor (COLOR_CAPTIONTEXT));
				return Led_Color (::GetSysColor (COLOR_HIGHLIGHTTEXT));
			#elif	qXWindows
				return (Led_Color::kWhite);
			#endif
		}
	inline	Led_Color	Led_GetSelectedTextBackgroundColor ()
		{
			#if		qMacOS
				RGBColor	hiliteRGBValue;
				LMGetHiliteRGB (&hiliteRGBValue);
				return Led_Color (hiliteRGBValue);
			#elif	qWindows
				//return Led_Color (::GetSysColor (COLOR_ACTIVECAPTION));
				return Led_Color (::GetSysColor (COLOR_HIGHLIGHT));
			#elif	qXWindows
				return (Led_Color::kBlack);
			#endif
		}

 







//	class	Led_Tablet_::ClipNarrowAndRestore
	inline	Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet):
		fTablet (tablet),
		fHasOldClip (false),
 		fOldClip ()
		{
			Led_RequireNotNil (tablet);
			#if		qWindows
				if (::GetDeviceCaps (fTablet->m_hDC, TECHNOLOGY) == DT_METAFILE) {
					return;
				}
			#endif
			fHasOldClip = tablet->GetClip (&fOldClip);
		}
	inline	Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Rect& clipFurtherTo):
		fTablet (tablet),
		fHasOldClip (false),
 		fOldClip ()
		{
			Led_RequireNotNil (tablet);
			fHasOldClip = tablet->GetClip (&fOldClip);
			#if		qMacOS
				Led_Assert (fHasOldClip);
				tablet->SetClip (fOldClip * clipFurtherTo);
			#elif	qWindows
				/*
				 *	NB: We must use IntersectClipRect instead of the above SetClip () call because the CLIP on Win32 is in
				 *	device rather than logical coordinates.
				 */
				if (::GetDeviceCaps (tablet->m_hDC, TECHNOLOGY) == DT_METAFILE) {
					return;
				}
				Led_Verify (::IntersectClipRect (*tablet, clipFurtherTo.GetLeft (), clipFurtherTo.GetTop (), clipFurtherTo.GetRight (), clipFurtherTo.GetBottom ()) != ERROR);
			#endif
		}
	inline	Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Region& clipFurtherTo):
		fTablet (tablet),
		fHasOldClip (false),
 		fOldClip ()
		{
			Led_RequireNotNil (tablet);
			fHasOldClip = tablet->GetClip (&fOldClip);
			#if		qMacOS
				Led_Assert (fHasOldClip);
				tablet->SetClip (fOldClip * clipFurtherTo);
			#elif	qWindows
				Led_Assert (false);	// NYI - see SPR#????
			#else
				Led_Assert (false);	// NYI
			#endif
		}
	inline	Led_Tablet_::ClipNarrowAndRestore::~ClipNarrowAndRestore ()
		{
			Led_AssertNotNil (fTablet);
			#if		qWindows
				if (::GetDeviceCaps (fTablet->m_hDC, TECHNOLOGY) == DT_METAFILE) {
					return;
				}
			#endif
			if (fHasOldClip) {
				fTablet->SetClip (fOldClip);
			}
			else {
				fTablet->SetClip ();
			}
		}



#if		qMacOS
	inline	Led_MacPortAndClipRegionEtcSaver::Led_MacPortAndClipRegionEtcSaver ():
		fSavedPort (Led_GetCurrentGDIPort ()),
		#if		!TARGET_CARBON
			fOldLeft (Led_GetCurrentGDIPort ()->portRect.left),
			fOldTop (Led_GetCurrentGDIPort ()->portRect.top),
		#endif
		//fRGBFgColor (Led_GetCurrentGDIPort ()->rgbFgColor),
		//fRGBBkColor (Led_GetCurrentGDIPort ()->rgbBkColor)
		fOldClip (NULL)
		{
			#if		TARGET_CARBON
			Rect	portRect;
			::GetPortBounds (Led_GetCurrentGDIPort (), &portRect);
			fOldLeft = portRect.left;
			fOldTop = portRect.top;
			#endif
			fOldClip = ::NewRgn ();
			if (fOldClip != NULL) {
				::GetClip (fOldClip);
			}
			::GetForeColor (&fRGBFgColor);
			::GetBackColor (&fRGBBkColor);
		}
	inline	Led_MacPortAndClipRegionEtcSaver::~Led_MacPortAndClipRegionEtcSaver ()
		{
			::SetPort (fSavedPort);
			::SetOrigin (fOldLeft, fOldTop);
			::RGBForeColor (&fRGBFgColor);
			::RGBBackColor (&fRGBBkColor);
			if (fOldClip != NULL) {
				::SetClip (fOldClip);
				::DisposeRgn (fOldClip);
			}
		}
#endif




//	class	Led_GDI_Obj_Selector
#if		qWindows
	inline	Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, HGDIOBJ objToSelect):
		fTablet (tablet),
		fRestoreObject (NULL),
		fRestoreAttribObject (NULL)
		{
			Led_AssertNotNil (tablet);
			Led_AssertNotNil (objToSelect);
			// See CDC::SelectObject for the logic..., but we do better than thiers and restore
			// right object to right DC!!!! - LGP 950525
			if (tablet->m_hDC != tablet->m_hAttribDC) {
				fRestoreObject = ::SelectObject (tablet->m_hDC, objToSelect);
			}
			if (tablet->m_hAttribDC != NULL) {
				fRestoreAttribObject = ::SelectObject (tablet->m_hAttribDC, objToSelect);
			}
		}
#elif	qMacOS
	inline	Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen):
		fTablet (tablet),
		#if		TARGET_CARBON
			fRestorePen (Led_Pen (::GetPortPenMode (Led_GetCurrentGDIPort ()), &Led_Pen::kBlackPattern, Led_Color (GDI_GetForeColor ())))
		#else
			fRestorePen (Led_Pen (Led_GetCurrentGDIPort ()->pnMode, &Led_GetCurrentGDIPort ()->pnPat, Led_Color (GDI_GetForeColor ())))
		#endif
		{
			#if		qDebug
				Led_Assert (Led_GetCurrentGDIPort () == *tablet);
			#endif
			GDI_RGBForeColor (pen.fPenColor.GetOSRep ());
			::PenMode (pen.fPenStyle);
			::PenPat (&pen.fPenPat);
		}
#elif	qXWindows
	inline	Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen)
		{
		}
#endif
	inline	Led_GDI_Obj_Selector::~Led_GDI_Obj_Selector ()
		{
			#if		qWindows
				//NB: These restore objects CAN be NULL, if no font (or whatever) selected into DC before we do... (aside from error cases)
				if (fRestoreObject != NULL) {
					Led_Verify (::SelectObject (fTablet->m_hDC, fRestoreObject));
				}
				if (fRestoreAttribObject != NULL) {
					Led_Verify (::SelectObject (fTablet->m_hAttribDC, fRestoreAttribObject));
				}
			#elif	qMacOS
				GDI_RGBForeColor (fRestorePen.fPenColor.GetOSRep ());
				::PenMode (fRestorePen.fPenStyle);
				::PenPat (&fRestorePen.fPenPat);
			#endif
		}







#if		qMacOS
	inline	void	GDI_RGBForeColor (const RGBColor& color)
		{
			::RGBForeColor (&color);
		}
	inline	void	GDI_RGBBackColor (const RGBColor& color)
		{
			::RGBBackColor (&color);
		}
	inline	RGBColor	GDI_GetForeColor ()
		{
			RGBColor	color;
			::GetForeColor (&color);
			return (color);
		}
	inline	RGBColor	GDI_GetBackColor ()
		{
			RGBColor	color;
			::GetBackColor (&color);
			return (color);
		}
#endif




	inline	short	Led_GetMacPictTop (const Led_Picture* picture)
		{
			Led_RequireNotNil (picture);
			#if		qMacOS
				return picture->picFrame.top;
			#elif	qWindows
				return Led_ByteSwapFromMac (picture->picFrameTop);
			#endif
		}
	inline	short	Led_GetMacPictLeft (const Led_Picture* picture)
		{
			Led_RequireNotNil (picture);
			#if		qMacOS
				return picture->picFrame.left;
			#elif	qWindows
				return Led_ByteSwapFromMac (picture->picFrameLeft);
			#endif
		}
	inline	short	Led_GetMacPictBottom (const Led_Picture* picture)
		{
			Led_RequireNotNil (picture);
			#if		qMacOS
				return picture->picFrame.bottom;
			#elif	qWindows
				return Led_ByteSwapFromMac (picture->picFrameBottom);
			#endif
		}
	inline	short	Led_GetMacPictRight (const Led_Picture* picture)
		{
			Led_RequireNotNil (picture);
			#if		qMacOS
				return picture->picFrame.right;
			#elif	qWindows
				return Led_ByteSwapFromMac (picture->picFrameRight);
			#endif
		}
	inline	short	Led_GetMacPictWidth (const Led_Picture* picture)
		{
			return (Led_GetMacPictRight (picture) - Led_GetMacPictLeft (picture));
		}
	inline	short	Led_GetMacPictHeight (const Led_Picture* picture)
		{
			return (Led_GetMacPictBottom (picture) - Led_GetMacPictTop (picture));
		}
	inline	Led_Size	Led_GetMacPictSize (const Led_Picture* picture)
		{
			return Led_Size (Led_GetMacPictHeight (picture), Led_GetMacPictWidth (picture));
		}
	#if		qMacOS
	inline	short	Led_GetMacPictTop (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictTop (*picture);
		}
	inline	short	Led_GetMacPictLeft (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictLeft (*picture);
		}
	inline	short	Led_GetMacPictBottom (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictBottom (*picture);
		}
	inline	short	Led_GetMacPictRight (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictRight (*picture);
		}
	inline	short	Led_GetMacPictWidth (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictWidth (*picture);
		}
	inline	short	Led_GetMacPictHeight (const Led_Picture*const* picture)
		{
			Led_RequireNotNil (picture);
			return Led_GetMacPictHeight (*picture);
		}
	inline	Led_Size	Led_GetMacPictSize (const Led_Picture*const* picture)
		{
			return Led_Size (Led_GetMacPictHeight (picture), Led_GetMacPictWidth (picture));
		}
	#endif





#if		qProvideIMESupport
	inline	Led_IME&	Led_IME::Get ()
		{
			if (sThe == NULL) {
				new Led_IME ();
			}
			Led_AssertNotNil (sThe);
			return (*sThe);
		}
	inline	void	Led_IME::Enable ()
		{
			if (fIMEEnableProc != NULL) {
				fIMEEnableProc (NULL, true);
			}
		}
	inline	void	Led_IME::Disable ()
		{
			if (fIMEEnableProc != NULL) {
				fIMEEnableProc (NULL, false);
			}
		}
	inline	bool	Led_IME::Available () const
		{
			return fWinNlsAvailable;
		}
	inline	void	Led_IME::ForgetPosition ()
		{
			fLastX = -1;
			fLastY = -1;
		}
#endif


#if		qLedUsesNamespaces
}
#endif



#endif	/*____Led_GDI_h___*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
