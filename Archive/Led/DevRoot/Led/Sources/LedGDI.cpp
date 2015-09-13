/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/LedGDI.cpp,v 2.124 2003/12/29 15:45:19 lewis Exp $
 *
 * Changes:
 *	$Log: LedGDI.cpp,v $
 *	Revision 2.124  2003/12/29 15:45:19  lewis
 *	Added Led_Bitmap::LoadBitmap and auto_gdi_ptr class
 *	
 *	Revision 2.123  2003/12/12 01:54:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.122  2003/12/02 22:14:28  lewis
 *	someone with alot of fonts installed complained they weren't all listed. See if bumping size of arg to XListFonts() helps
 *	
 *	Revision 2.121  2003/05/28 16:56:48  lewis
 *	added CenterRectInRect() function
 *	
 *	Revision 2.120  2003/05/22 21:02:13  lewis
 *	fixed sizeof (RGBTRIPLE) assertion
 *	
 *	Revision 2.119  2003/05/21 01:01:14  lewis
 *	minor tweek to avoid GCC warnings about abs used with unsigned # (and added one use I missed) - in Led_GetDIBImageSize
 *	
 *	Revision 2.118  2003/05/17 21:46:22  lewis
 *	minor tweeks to recent DIB changes to get compiling (safely) on Mac
 *	
 *	Revision 2.117  2003/05/17 21:44:21  lewis
 *	minor tweeks to recent DIB changes to get compiling (safely) on Mac
 *	
 *	Revision 2.116  2003/05/17 21:42:36  lewis
 *	minor tweeks to recent DIB changes to get compiling (safely) on Mac
 *	
 *	Revision 2.115  2003/05/17 21:21:08  lewis
 *	SPR#1489: DIB fixes. Only REAL bug fixes was Led_GetDIBPalletByteCount for case of biCompression==BI_BITFIELDS. That was what caused the images to look slightly messed up. But also - added code for Non-30-version DIBs from DibLook sample code
 *	
 *	Revision 2.114  2003/04/22 15:25:51  lewis
 *	SPR#1447: ScriptStringAnalyse () in Led_Tablet_::TabbedTextOut () fails when applied to a metafile HDC, so fallback on older drawing code.
 *	
 *	Revision 2.113  2003/04/18 21:03:00  lewis
 *	misc cleanups to get things compiling with gcc 2.96 on Linux RedHat 7.3
 *	
 *	Revision 2.112  2003/04/17 16:23:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.111  2003/04/10 16:28:48  lewis
 *	(related to SPR#1329) fix Intersection (INCRFONTSPEC,INCFONTSPEC) for tricky case of font-size
 *	
 *	Revision 2.110  2003/04/10 15:54:44  lewis
 *	(vaguely related to SPR#1329)Added Intersection (const Led_IncrementalFontSpecification&,const Led_IncrementalFontSpecification&) function
 *	
 *	Revision 2.109  2003/04/07 15:20:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.108  2003/03/21 14:50:19  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support, and use new char[] instead of calling ::operator new
 *	
 *	Revision 2.107  2003/03/13 13:58:06  lewis
 *	SPR#1336 - fixed Led_Tablet_::FrameRectangle bug (bottom vs. height)
 *	
 *	Revision 2.106  2003/03/03 16:46:58  lewis
 *	SPR#1318 - fix OffscreenBitmap code for WIN32 to properly cache the BM if its size doesn't change
 *	
 *	Revision 2.105  2003/02/28 16:18:38  lewis
 *	SPR#1313 - added back ScriptStringFree call I had inadvertently left out in fix for SPR#1295.
 *	Caused SUBTLE mem leak!
 *	
 *	Revision 2.104  2003/02/26 22:19:57  lewis
 *	added Led_Tablet_::FrameRectangle helper
 *	
 *	Revision 2.103  2003/02/24 23:06:21  lewis
 *	minor change to assert to work around Borland C++ bug - not enough to warrant a bug define
 *	
 *	Revision 2.102  2003/02/14 20:40:18  lewis
 *	SPR#1303 - qLed_CharacterSet is now obsolete. Instead - use qSingleByteCharacters,
 *	qMultiByteCharacters, or qWideCharacters
 *	
 *	Revision 2.101  2003/02/13 15:26:35  lewis
 *	SPR#1271 - more minor cleanups to the WIN32 RecolorHelper code - keep it assocaited with the
 *	Led_Tablet_ and not a static (construct a bit more - but get really big cache guys cleaned out
 *	if not needed). Also lots of other minor cleanups to the code.
 *	
 *	Revision 2.100  2003/02/12 15:06:13  lewis
 *	SPR#1271 - finished code cleanups for Led_Tablet_::HilightRectangle() code (recolorhelper etc).
 *	Fixed a small bug I had introduced into Led_GetDIBPalletByteCount
 *	
 *	Revision 2.99  2003/02/12 03:14:05  lewis
 *	SPR#1271 - more cleanups of recoloring Win32 Hilight code
 *	
 *	Revision 2.98  2003/02/12 01:57:39  lewis
 *	SPR#1271- Lots of code cleanups to the recoloring (Hilight) code. Deleted tons of approaches
 *	that didn't pan out (not fast enuf). Still more cleanup todo, but its close
 *	
 *	Revision 2.97  2003/02/11 21:29:19  lewis
 *	SPR#1271- more hacks/tests/futzing with recolor / hilight code
 *	
 *	Revision 2.96  2003/02/11 21:16:29  lewis
 *	SPR#1271- minor tweeks - still not doing hilight fast enuf
 *	
 *	Revision 2.95  2003/02/10 18:36:47  lewis
 *	SPR#1295 - added qTryToOptimizeLongUNISCRIBEScriptOutCalls to ameliorate Win32 problem
 *	drawing on long lines - but still needs more testing
 *	
 *	Revision 2.94  2003/02/10 17:47:29  lewis
 *	SPR#1295- added kMaxUNISCRIBECharacters and set to 30000, so we don't make too big a call to
 *	UNISCRIBE. Its STILL VERY SLOW when we get that many characters. See if I can optimize THAT.
 *	But at least no invalid HRESULT or buggy display
 *	
 *	Revision 2.93  2003/02/08 21:57:50  lewis
 *	more fidling with Recolor code - to see if I can find a way that is fast enough on laptop
 *	
 *	Revision 2.92  2003/02/07 18:46:21  lewis
 *	I tried DoRecolor_TryDirectDIBSection and DoRecolor_TryDIBits and neither worked acceptably
 *	
 *	Revision 2.91  2003/02/07 01:32:19  lewis
 *	fixed minor bugs (in recolor code)
 *	
 *	Revision 2.90  2003/02/07 01:16:39  lewis
 *	In Led_Tablet_::HilightArea_SolidHelper: test of using B&W for
 *	hilight color - and if so - use OLD DSTINVERT code - since that is much
 *	faster (at least on some systems) - for now - til I can further speed
 *	tweek. That way - people who want the old behavior can easily get it ().
 *	
 *	Revision 2.89  2003/02/07 01:09:29  lewis
 *	More cleanups to RecolorHelper code. Now we actually properly
 *	compute the mapping table used for qUseMyOwnMappingCodeCuzWin32CLUTChangeTooSlow,
 *	and have generally cleaned up the code. Also make sure the CLUT we generate for
 *	our DIBSECTION has full 256 colors (speeds things up by 33% for unknown reasons).
 *	And make sure that CLUT has the hilight color (etc) in it.
 *	
 *	Revision 2.88  2003/02/05 22:40:06  lewis
 *	SPR#1271- Fixed ::GetSysColor() args - SB COLOR_HILIGHTTEXT instead of COLOR_CAPTION_TEXT etc.
 *	Also - get rid of PER-Led_Tablet instance of RecolorHelper. Instead keep one global instance. Also,
 *	several other cleanups, and added preliminary version (still disabled) of
 *	qUseMyOwnMappingCodeCuzWin32CLUTChangeTooSlow code
 *	
 *	Revision 2.87  2003/02/05 17:58:29  lewis
 *	SPR#1271 - added RecolorHelper for Win32 HilightARectangle code. Still more work todo (its too slow)
 *	
 *	Revision 2.86  2003/02/05 00:52:21  lewis
 *	SPR#1271 - improve RecolorDIBSection code for adjusting the color table - now looks much better
 *	
 *	Revision 2.85  2003/02/05 00:25:46  lewis
 *	SPR#1271 - CreateStandardPalette improved so it generates max values for each color dimension
 *	
 *	Revision 2.84  2003/02/04 23:11:36  lewis
 *	SPR#1271- Added DibSection/RecolorDIBSection code to improve function of Led_Tablet_::HilightArea_SolidHelper
 *	for MSWindows. This is NOT done - but close
 *	
 *	Revision 2.83  2003/02/01 06:00:17  lewis
 *	SPR#1276- use ScriptStringAnalyze even for LTR text, and use SSA_FALLBACK so that fallback
 *	fonts used for glyphs not present in the selected font
 *	
 *	Revision 2.82  2003/01/28 22:47:17  lewis
 *	add Led_TWIPS::kPoint and Led_TWIPS::kInch
 *	
 *	Revision 2.81  2003/01/16 14:12:32  lewis
 *	use the spelling 'hilight' instead of 'hilite' a bit more consistently
 *	
 *	Revision 2.80  2003/01/06 14:45:50  lewis
 *	minor tweeks to RTL UNISCRIBE code
 *	
 *	Revision 2.79  2003/01/02 21:15:55  lewis
 *	SPR#1199- progress on display of arabic text (really measurement part). Dont set
 *	scriptState.fOverrideDirection  = true when measuring. Still need to study/test more.
 *	
 *	Revision 2.78  2003/01/02 18:41:58  lewis
 *	qTryScriptToCPX - doesn't appear to have worked - so far
 *	
 *	Revision 2.77  2002/12/19 13:34:07  lewis
 *	cleanups and add asserts to measuretext
 *	
 *	Revision 2.76  2002/12/19 03:14:08  lewis
 *	small cleanups to UNISCRIBE code - but still no FIXES
 *	
 *	Revision 2.75  2002/12/19 02:18:57  lewis
 *	SPR#1199- work on Arabic/Uniscribe/Windows RTL measuretext (ScriptStringGetLogicalWidths & ScriptString_pSize)
 *	
 *	Revision 2.74  2002/12/16 22:11:02  lewis
 *	qUniscribeAvailableWithSDK define set for MSVC7 but not earlier compilers. SPR#1209 hack to
 *	Led_RefCntPtr<> doesnt work if qXOfXRefCTORWithTemplateMemberXOfXRefFailsBug (MSVC6)
 *	
 *	Revision 2.73  2002/12/16 19:24:40  lewis
 *	small tweeks to compile on Windows NON-UNICODE
 *	
 *	Revision 2.72  2002/12/13 18:32:57  lewis
 *	major changes to windows code for imaging RTL
 *	text (SPR#1199). Implemented 3 approaches controlled by
 *	qUseUniscribeToImage/qUseFakeTTGetWPlacementToImage/qUseGetCharPlacementToImage
 *	flags.
 *	Mostly use UNISCRIBE to image RTL text (and LTR text same as before). All this code is Win32 only. Code for
 *	qUseFakeTTGetWPlacementToImage is LONG and almost certainly useless. Maybe get rid of it? This SPR isn't
 *	done and this code has seen little testing (just XP and
 *	WinME and then not much). But - it seems like the approach
 *	I will need to take.
 *	
 *	Revision 2.71  2002/12/03 15:49:06  lewis
 *	SPR#1191- use new TextDirection enum defined in LedGDI.h. Get rid of ScriptRunElement::fLength.
 *	Add direction arg to Led_Tablet_::TabbedTextOut () and use that to call (WIN32)Led_Tablet_::SetTextAlign().
 *	Hopefully this fixes display on Win9x of Arabic text
 *	
 *	Revision 2.70  2002/12/03 14:40:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2002/12/03 14:28:57  lewis
 *	SPR#1190 - changed name for (TextImager/Led_Tablet_)::HilightARectangle to HilightArea
 *	
 *	Revision 2.68  2002/11/01 15:29:15  lewis
 *	fix for MacOS
 *	
 *	Revision 2.67  2002/10/30 22:40:55  lewis
 *	minor cleaups to CvtToFromTwips stuff and Rect_Base<>
 *	
 *	Revision 2.66  2002/10/30 15:20:10  lewis
 *	Added new Led_Tablet_::CvtFromTWIPS overloads to handle Led_Point/Rect <-> Led_TWIPS_Point etc (SPR#1157)
 *	
 *	Revision 2.65  2002/10/30 13:29:15  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed) to
 *	make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them in tons of
 *	places - but still TONS more required. Leave that battle for another day. I'm not even SURE its a good idea.
 *	Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin with with all this
 *	nonsese).
 *	
 *	Revision 2.64  2002/05/06 21:33:43  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.63  2002/04/22 21:43:05  lewis
 *	added Led_GetDIBImageRowByteCount() helper as part of fix SPR#0811
 *	
 *	Revision 2.62  2001/11/27 00:29:50  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.61  2001/10/19 00:08:47  lewis
 *	DocComments & cleanups
 *	
 *	Revision 2.60  2001/10/17 20:42:57  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.59  2001/09/26 15:41:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2001/09/24 14:24:55  lewis
 *	SPR#1042- major cleanups to Led_Region code (not fully backward compatable. New CTORs
 *	and much more portable. New class OffscreenTablet::OT (turned out to not be needed but
 *	could be in the future). Led_Tablet_::ClipNarrowAndRestore added. New Led_Tablet_::Get/SetClip
 *	code. None of this is used much, but could be in a future release. Lots of new operator
 *	overloads (e.g. operator* for intersection, etc...)
 *	
 *	Revision 2.57  2001/09/16 18:31:49  lewis
 *	as a small part of SPR#1033 - noted that centerwindinwind routine needed to
 *	handle case where no parent wind(use desktop)
 *	
 *	Revision 2.56  2001/09/12 16:05:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.55  2001/09/12 14:53:31  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far).
 *	Redo Led_Region support for MacOS so now more like with PC (no operator conversion
 *	to macregion and auto constructs OSRegion except when called with region arg - in
 *	whcih case we dont OWN region and dont delete it on DTOR
 *	
 *	Revision 2.54  2001/09/11 22:29:15  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.53  2001/08/29 23:36:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.52  2001/08/28 18:43:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2001/07/31 15:15:22  lewis
 *	silence warnings (static_cast<>) for MSVC70B2
 *	
 *	Revision 2.50  2001/07/19 02:21:46  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.49  2001/06/05 14:18:10  lewis
 *	SPR#0905- use resolution of 100 instead of 72/75 for xwindows. And fix CVTToFROMTWIPS
 *	etc code to not hardwrie res - but grab from Led_GDIGlobals (on mac/XWin, on Win
 *	already did better and grabbed from tablet)
 *	
 *	Revision 2.48  2001/06/04 16:50:39  lewis
 *	move Led_Tablet_::CTOR/DTOR to .cpp file. SPR#0929- use new map<> to keep track (cache)
 *	several XFontStruct objects (XWindows) - instead of just one, and fixed bug where if we
 *	had to do name mapping, wasn't being cached at all (times new roman).
 *	
 *	Revision 2.47  2001/05/29 22:59:49  lewis
 *	added new color names (kLimeGreen/Fusia/Aqua) to rationalize with names we use elsewhere.
 *	Fix what WordPRocessor cmdnames/color coordination. Sync with 'Web Design in a Nutshell' -
 *	HTML reference name/values. Part of SPR#0943
 *	
 *	Revision 2.46  2001/05/27 02:21:40  lewis
 *	SPR#0936- call XSetClipRectangles/XSetClipMask to get clipping right.
 *	
 *	Revision 2.45  2001/05/26 18:24:06  lewis
 *	slight optimization to set fore/back color code for kBlack/White (avoid xalloccolor call)
 *	
 *	Revision 2.44  2001/05/24 15:48:02  lewis
 *	SPR#0929- a little work on X-Windows drawing speed (very simple caching - must redo much better)
 *	
 *	Revision 2.43  2001/05/22 21:33:43  lewis
 *	on last checkin - also fixed xwindows bug where empty fontfamily names weren't being
 *	filtered from the list of returned installed font names
 *	
 *	Revision 2.42  2001/05/22 21:33:05  lewis
 *	make Led_Tablet_::ParseFontName () public, so it can be used by new
 *	Led_FontSpecification::SetFromOSRep() - all X-Windows only routines
 *	
 *	Revision 2.41  2001/05/18 20:59:19  lewis
 *	SPR#0922 added Led_Tablet_::ScrollBitsAndInvalRevealed
 *	
 *	Revision 2.40  2001/05/16 15:58:38  lewis
 *	in Erasebackgroundhelper routine - for macOS - call setport - not assert port already set
 *	
 *	Revision 2.39  2001/05/14 17:31:35  lewis
 *	fix macos code- add back ifdefs safenewGWord code
 *	
 *	Revision 2.38  2001/05/12 22:30:48  lewis
 *	SPR#0915- got OffscreenTablet stuff mostly working (seems OK but slow) on XWindows
 *	
 *	Revision 2.37  2001/05/11 23:50:11  lewis
 *	Small cleanups of offscreen bitmap code
 *	
 *	Revision 2.36  2001/05/11 23:39:33  lewis
 *	SPR#0915- Added class OffscreenTablet to abstract away common code from
 *	MultiRowTextImager/SimpleTextImager to handle offscreen bitmap drawing. Much
 *	cleanedup (HOPEFULLY not broken).
 *	
 *	Revision 2.35  2001/05/11 19:38:26  lewis
 *	fix memleak from some calls to XListFonts (didnt always call XFreeFontNames). SPR#0791 progress -
 *	now handle italics for XWindows. SPR#0914- cache XWindows fontname mappings for speed.
 *	
 *	Revision 2.34  2001/05/09 22:40:36  lewis
 *	lose GDI_ wrappers for a few mac calls (now we use the Led_Tablet_ abstraction to do our
 *	mapping, and the trapcaching stuff is no more
 *	
 *	Revision 2.33  2001/05/08 21:40:16  lewis
 *	use Led_SDK_String instead of string for args to Led_Tablet_::ParseFontName etc. XWindows
 *	fBold/fItalic font attributes :1 size.
 *	
 *	Revision 2.32  2001/05/07 22:22:24  lewis
 *	SPR#0888- finishing touches on font support (well - at least on getting it usable for xwindows).
 *	Did FontMatcher(BestMatch). Other small cleanups.
 *	
 *	Revision 2.31  2001/05/07 16:49:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2001/05/07 16:45:07  lewis
 *	SPR#0888. CLeaned up Led_FontMetrics stuff (esp for X-windows - but also for Mac/Win).
 *	New portable Led_Tablet_::GetFontMetrics method. Now Led_FontMEtrics for x-windows
 *	produces good values.
 *	
 *	Revision 2.29  2001/05/05 12:28:12  lewis
 *	SPR#0888 - lots of work on X-Windows font details. FontSize/bold/italics mostly
 *	supported. Fixed serious bug(s) with Led_Tablet_::MeasureText () - wrong ptr arg
 *	to XTextWidth - was lucky it worked as well as it did
 *	
 *	Revision 2.28  2001/05/05 09:36:49  lewis
 *	added a few more predefined colors (by calling GetPaletteEntries(DEFAULT_PALETTE))- kTeal, etc
 *	
 *	Revision 2.27  2001/05/04 20:44:25  lewis
 *	SPR#0888- a lot of work on getting X fonts code working right.
 *	
 *	Revision 2.26  2001/05/04 17:37:24  lewis
 *	change API for HilightARectangle_SolidHelper - now takes fore/back hilgiht/normal colors
 *	(so in principle can exchange them all
 *	
 *	Revision 2.25  2001/05/03 22:08:44  lewis
 *	restore foreground to GC in erasebackground_helper call
 *	
 *	Revision 2.24  2001/05/02 14:53:54  lewis
 *	write HilightRectangle_helper () code with if (hilightrect.empty())) check to avoid
 *	possible assert error in getHeight()
 *	
 *	Revision 2.23  2001/04/30 22:36:37  lewis
 *	SPR#0886- Led_Tablet_::HilightARectangle_SolidHelper moved here from TextImager class.
 *	Update docs. Implement XWindows version.
 *	
 *	Revision 2.22  2001/04/30 20:59:34  lewis
 *	move Win32CharSetToCodePage() from private to public (in .h file). Losely related to SPR#0891
 *	
 *	Revision 2.21  2001/04/26 16:39:29  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.20  2001/04/25 22:44:47  lewis
 *	SPR#0879- moved EraseBackground_SolidHelper () to Led_Tablet_ from TextImager(wrapper now in TextImager).
 *	And added SetFore/BackColor methods to Led_Tablet_ - again - cleaning up code from TextImager.
 *	
 *	Revision 2.19  2001/04/23 17:20:42  lewis
 *	fix Led_Tablet_::TabbedTextOut () so we return right size of string drawn
 *	
 *	Revision 2.18  2001/04/23 16:38:46  lewis
 *	SPR#0876- Moved some utilities from TextImager to LedGDI
 *	
 *	Revision 2.17  2000/10/16 00:14:27  lewis
 *	Added Led_CenterWindowInParent() helper
 *	
 *	Revision 2.16  2000/09/05 14:31:47  lewis
 *	fix typo
 *	
 *	Revision 2.15  2000/09/02 21:53:06  lewis
 *	use ::operator new () instead of new char [] = cuz this gets deleted as a single object -
 *	not using array delete
 *	(was caught by MWEKRS free store code)
 *	
 *	Revision 2.14  2000/09/01 00:26:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  2000/08/31 20:45:51  lewis
 *	Added Led_Pen::kBlackPattern
 *	
 *	Revision 2.12  2000/07/26 05:46:14  lewis
 *	must manually include imm.h- incase compiled with NOIME or VC_EXTRALEAN
 *	
 *	Revision 2.11  2000/07/09 04:30:46  lewis
 *	Added Led_CloneDB function
 *	
 *	Revision 2.10  2000/07/08 17:21:13  lewis
 *	minor Win32 GDI helper fixups (asserts etc)
 *	
 *	Revision 2.9  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.8  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.7  2000/04/04 15:11:40  lewis
 *	Added Led_InstalledFonts () helper class to share some code among the various apps - and make
 *	filtering by various contraints options
 *	
 *	Revision 2.6  2000/03/29 05:15:16  lewis
 *	experimental new qUseNewIMECode IME fixes. Must test at LEC. Basicly - use newer
 *	(documented) versions of the
 *	APIs since the old ones seem to sometimes mysteriously fail on Win2k
 *	
 *	Revision 2.5  1999/12/21 20:52:21  lewis
 *	Added Led_DIBFromHBITMAP () - for SPR# 0672. And call new Led_ThrowIfNull() as needed.
 *	
 *	Revision 2.4  1999/12/14 21:24:30  lewis
 *	Add Led_IME::GetCompositionResultStringW()
 *	
 *	Revision 2.3  1999/12/14 20:03:09  lewis
 *	Moved Led_IME code from LedSupport to LedGDI
 *	
 *	Revision 2.2  1999/12/14 18:07:30  lewis
 *	Preliminary qXWindows support
 *	
 *	Revision 2.1  1999/11/15 21:20:32  lewis
 *	Moved some stuff here from LedSupport - in prep for X-Windows port, and to allow building for Win32 with
 *	no MFC dependencies. Re-implemented Led_Tablet (for Win32) ETC - - mimicing MFC versions - but not
 *	calling them - so that can build for Win32 not including MFC
 *	
 *
 *
 *
 *	<========== CODE MOVED HERE FROM LedSupport.cpp ==========>
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<algorithm>
#include	<cstdio>
#include	<set>

#include	"CodePage.h"
#include	"LedGDI.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qWindows
	// Often included by <Windows.h> automaticly, but sometimes people define NOIME or VC_EXTRALEAN, and then we
	// must include this manaully.
#include <windows.h>
	#include <imm.h>        
#endif

#if		qWindows
	// RTL Imaging flags
	#define	qUseUniscribeToImage				qUniscribeAvailableWithSDK && qWideCharacters
	#define	qUseFakeTTGetWPlacementToImage		1
	#define	qUseGetCharPlacementToImage			1
#endif


#if		qUseUniscribeToImage
#include	<Usp10.h>
#endif

/*
 *	Short term debugging crap to debug X-Windows font issues.
 */
#ifndef		qDebugFontDetails
	#define	qDebugFontDetails	qDebug && qXWindows
#endif


// Suggestion from Greg Binkerd [gregb@microsoft.com] about SRX021206603127 - LGP 2003-01-02
#if		qUniscribeAvailableWithSDK
	#define	qTryScriptToCPX	0
	//#define	qTryScriptToCPX	1


	#ifndef	qTryToOptimizeLongUNISCRIBEScriptOutCalls
	#define	qTryToOptimizeLongUNISCRIBEScriptOutCalls	1
	#endif

#endif



#if		qWindows
	/*
	 *	Used to use CreateCompatibleBitmap, but as of SPR#1271 try using a DIBSection (of a compatile depth) instead).
	 *	This has no noticable effect on normal drawing, but greatly speeds HilightRectangle () code for some computers.
	 */
	#ifndef	qUseDIBSectionForOffscreenBitmap
	#define	qUseDIBSectionForOffscreenBitmap			1
	#endif
#endif


#if		0
//Hack to occasionally have some reliable test to display in Arabic
static	const wchar_t	kHiInArabic[]	=	L"\xfe7d\xfe8d\xfe91\xfea3\xfead\xfedb";	// IN DISPLAY ORDER - NOT LOGICAL ORDER
#endif


#if		qWindows
		inline	bool operator== (PALETTEENTRY lhs, COLORREF rhs)
			{
				return RGB (lhs.peRed, lhs.peGreen, lhs.peBlue) == rhs;
			}
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif




#if		!qHaveWindowsDIBDefined
	#ifndef BI_BITFIELDS
	#define BI_BITFIELDS    3
	#endif
#endif



#if		qWindows
	#ifdef	_UNICODE
		const	bool	kRunning32BitGDI	=	true;									//	UNICODE only supported on 32GDI (NT or Win2k or Later)
	#else
		const	bool	kRunning32BitGDI	=	((::GetVersion () & 0x80000000) == 0);	// I BELIEVE this is how we can test we are under NT!!!
	#endif																				// Should be a better way to check for 32bit GDI!!!
																						// LGP 950504
	#if		qWorkAroundWin95UNICODECharImagingBugs
		inline	bool	_IsWin95Helper_ ()
			{
				OSVERSIONINFO	osvi;
				GetVersionEx (&osvi);
				return (
							(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) and
							(osvi.dwMajorVersion == 4) and
							(osvi.dwMinorVersion == 0)
						)
					;
			}
		const	bool	kRunningWin95GDI	=	not kRunning32BitGDI and _IsWin95Helper_ ();
	#endif
#endif




	#if		qMacOS
		inline	QDErr	SafeNewGWorld (GWorldPtr* offscreenGWorld, short pixelDepth, const Rect* boundsRect,
										CTabHandle cTable, GDHandle aGDevice, GWorldFlags flags
									)
			{
				// NewGWorld seems to crash with 7.5.3 when we are low on memory in our app heap.
				// So just treat this as a failure result from NewGWorld, and avoid the crash.
				// LGP 960524
				try {
					Led_CheckSomeLocalHeapRAMAvailable (8*1024);
				}
				catch (...) {
					return memFullErr;
				}
				return ::NewGWorld (offscreenGWorld, pixelDepth, boundsRect, cTable, aGDevice, flags);
			}
		inline	GWorldFlags	SafeUpdateGWorld (GWorldPtr* offscreenGWorld, short pixelDepth,
												const Rect* boundsRect, CTabHandle cTable,
												GDHandle aGDevice, GWorldFlags flags
											)
			{
				// UpdateGWorld seems to crash with 7.5.3 when we are low on memory in our app heap.
				// So just treat this as a failure result from UpdateGWorld, and avoid the crash.
				// LGP 960524
				try {
					Led_CheckSomeLocalHeapRAMAvailable (8*1024);
				}
				catch (...) {
					return -1;	// <0 implies error
				}
				return ::UpdateGWorld (offscreenGWorld, pixelDepth, boundsRect, cTable, aGDevice, flags);
			}
	#endif




#if		qWindows
	#if		qWideCharacters && (qWorkAroundWin95UNICODECharImagingBugs || qWorkAroundWin98UNICODECharImagingBugs)
		inline	bool	CodePageBetterOffUsingWideCharVersion (UINT codePage)
			{
				switch (codePage) {
					case	kCodePage_SJIS:
					case	kCodePage_Korean:
					case	kCodePage_GB2312:
					case	kCodePage_BIG5:		return true;
					default:					return false;
				}
			}
	#endif
	#if		qWideCharacters && (qWorkAroundWin95UNICODECharImagingBugs || qWorkAroundWin98UNICODECharImagingBugs)
		inline	void	Win32_GetTextExtentExPoint_Win95n98WorkAround (HDC hdc, const Led_tChar* str, size_t nChars, int maxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize)
			{
				Led_Assert (nChars >= 0);
				Led_SmallStackBuffer<char>	buf (2*nChars);


				/*
				 *	Under both Win95 and Win98 - the GetTextExtentExPointW function doesn't work. So I must use
				 *	GetTextExtentExPointA. But GetTextExtentExPointA () doesn't work for multibyte character sets.
				 *	So - the compromise is to use GetTextExtentExPointA () when I can - and fall back on GetTextExtentPoint32W
				 *	which DOES seem to work on Win95/98 - albeit much more slowly).
				 */
				bool	needToUseSlowLoopWithGetTextExtentPoint32W	=	false;
				{
					for (size_t i = 0; i < nChars; ++i) {
						if (str[i] >= static_cast<wchar_t> (256)) {
							needToUseSlowLoopWithGetTextExtentPoint32W = true;
							break;
						}
					}
				}

				if (needToUseSlowLoopWithGetTextExtentPoint32W) {
					lpSize->cx = 0;
					lpSize->cy = 0;
					for (size_t i = 0; i < nChars; ++i) {
						SIZE	size;
						Led_Verify (::GetTextExtentPoint32W (hdc, &str[i], 1, &size));
						lpSize->cx += size.cx;
						lpSize->cy = max (size.cy, lpSize->cy);
						alpDx[i] = lpSize->cx;
					}
					return;
				}
				UINT	codePage	=	Win32CharSetToCodePage (::GetTextCharset (hdc));	// Was CP_ACP...
				int	nChars2	=	::WideCharToMultiByte (codePage, 0, str, nChars, buf, nChars*2, NULL, NULL);
				Led_Assert (lpnFit == NULL);	// cuz we don't support handling/mapping this # back not needed right now - LGP 980422
				Led_SmallStackBuffer<int>	tmpAlpDxArray (nChars2);
				Led_Verify (::GetTextExtentExPointA (hdc, buf, nChars2, maxExtent, NULL, tmpAlpDxArray, lpSize));
				// Now walk through the tmpAlpDxArray, and as we find CHARACTER boundaries in the text, map that to CHARACTER (by one)
				// boundaries in the UNICODE based array
				const char* mbyteStrIter = buf;
				const int* mbyteAlpDxIter = tmpAlpDxArray;
				for (size_t i = 0; i < nChars; ++i, mbyteAlpDxIter++) {
					int	nBytesInThisChar	=	::WideCharToMultiByte (codePage, 0, &str[i], 1, NULL, 0, NULL, NULL);
					Led_Assert (nBytesInThisChar == 1 or nBytesInThisChar == 2);
					if (nBytesInThisChar == 2) {
						mbyteAlpDxIter++;
					}
					alpDx[i] = *mbyteAlpDxIter;
				}
				Led_Assert (mbyteAlpDxIter - static_cast<int*>(tmpAlpDxArray) == nChars2);	// be sure the sum we get is the sum of the individual chars... Else maybe we read off end of array!
			}
	#endif
	#if		qWideCharacters && qWorkAroundWin95UNICODECharImagingBugs
		inline	void	Win32_GetTextExtentPoint_Win95WorkAround (HDC hdc, const Led_tChar* str, int nChars, LPSIZE lpSize)
			{
				UINT	codePage	=	Win32CharSetToCodePage (::GetTextCharset (hdc));	// Was CP_ACP...
				if (CodePageBetterOffUsingWideCharVersion (codePage)) {
					Led_Verify (::GetTextExtentPointW (hdc, str, nChars, lpSize));
					return;
				}
				Led_SmallStackBuffer<char>	buf (2*nChars);
				int	nChars2	=	::WideCharToMultiByte (codePage, 0, str, nChars, buf, nChars*2, NULL, NULL);
				Led_Verify (::GetTextExtentPointA (hdc, buf, nChars2, lpSize));
			}
	#endif
	#if		qWideCharacters && qWorkAroundWin95UNICODECharImagingBugs
		inline	void	Win32_TextOut_Win95WorkAround (HDC hdc, int xStart, int yStart, const Led_tChar* str, int nChars)
			{
				UINT	codePage	=	Win32CharSetToCodePage (::GetTextCharset (hdc));	// Was CP_ACP...
				if (CodePageBetterOffUsingWideCharVersion (codePage)) {
					Led_Verify (::TextOutW (hdc, xStart, yStart, str, nChars));
					return;
				}
				Led_SmallStackBuffer<char>	buf (2*nChars);
				int	nChars2	=	::WideCharToMultiByte (codePage, 0, str, nChars, buf, nChars*2, NULL, NULL);
				Led_Verify (::TextOutA (hdc, xStart, yStart, buf, nChars2));
			}
	#endif
	inline	void	Win32_GetTextExtentExPoint (HDC hdc, const Led_tChar* str, size_t nChars, int maxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize)
		{
			#if		qWideCharacters
				#if		qWorkAroundWin95UNICODECharImagingBugs || qWorkAroundWin98UNICODECharImagingBug
					if (not kRunning32BitGDI) {
						Win32_GetTextExtentExPoint_Win95n98WorkAround (hdc, str, nChars, maxExtent, lpnFit, alpDx, lpSize);
						return;
					}
				#endif
				Led_Verify (::GetTextExtentExPointW (hdc, str, nChars, maxExtent, lpnFit, alpDx, lpSize));
			#else
				Led_Verify (::GetTextExtentExPointA (hdc, str, nChars, maxExtent, lpnFit, alpDx, lpSize));
			#endif
		}
	inline	void	Win32_GetTextExtentPoint (HDC hdc, const Led_tChar* str, int nChars, LPSIZE lpSize)
		{
			#if		qWideCharacters
				#if		qWorkAroundWin95UNICODECharImagingBugs
					if (kRunningWin95GDI) {
						Win32_GetTextExtentPoint_Win95WorkAround (hdc, str, nChars, lpSize);
						return;
					}
				#endif
				Led_Verify (::GetTextExtentPointW (hdc, str, nChars, lpSize));
			#else
				Led_Verify (::GetTextExtentPointA (hdc, str, nChars, lpSize));
			#endif
		}
	inline	void	Win32_TextOut (HDC hdc, int xStart, int yStart, const Led_tChar* str, int nChars)
		{
			#if		qWideCharacters
				#if		qWorkAroundWin95UNICODECharImagingBugs
					if (kRunningWin95GDI) {
						Win32_TextOut_Win95WorkAround (hdc, xStart, yStart, str, nChars);
						return;
					}
				#endif
				Led_Verify (::TextOutW (hdc, xStart, yStart, str, nChars));
			#else
				Led_Verify (::TextOutA (hdc, xStart, yStart, str, nChars));
			#endif
		}
#endif




#if		qWindows && qWideCharacters && qUseUniscribeToImage

	const	size_t	kMaxUNISCRIBECharacters	=	30000;

	/*
	 *	Use LoadLibrary/GetProcAddress instead of direct call to avoid having to link with
	 *	Usp10.lib. This avoidance allows us to run on systems that don't it installed.
	 */
	struct	UniscribeDLL {
		UniscribeDLL ():
			fDLL (::LoadLibrary (_T ("Usp10.dll"))),
			fScriptItemize (NULL),
			fScriptShape (NULL),
			fScriptPlace (NULL),
			fScriptStringAnalyse (NULL),
			fScriptStringOut (NULL),
			fScriptStringFree (NULL),
			fScriptStringGetLogicalWidths (NULL),
			fScriptString_pcOutChars (NULL),
			fScriptString_pSize (NULL),
			fScriptStringCPtoX (NULL)
			{
				if (fDLL != NULL) {
					fScriptItemize = (HRESULT (WINAPI *) (const WCHAR*, int, int, const SCRIPT_CONTROL*, const SCRIPT_STATE*, SCRIPT_ITEM*, int*))
							(::GetProcAddress (fDLL, "ScriptItemize"));
					fScriptShape = (HRESULT (WINAPI *) (HDC, SCRIPT_CACHE*, const WCHAR*, int, int, SCRIPT_ANALYSIS*, WORD*, WORD*, SCRIPT_VISATTR*, int*))
							(::GetProcAddress (fDLL, "ScriptShape"));
					fScriptPlace = (HRESULT (WINAPI *) (HDC, SCRIPT_CACHE*, const WORD*, int, const SCRIPT_VISATTR*, SCRIPT_ANALYSIS*, int*, GOFFSET*, ABC*))
							(::GetProcAddress (fDLL, "ScriptPlace"));
					fScriptStringAnalyse = (HRESULT (WINAPI *) (HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL *, SCRIPT_STATE *, const int *, SCRIPT_TABDEF *, const BYTE *, SCRIPT_STRING_ANALYSIS *))
							(::GetProcAddress (fDLL, "ScriptStringAnalyse"));
					fScriptStringOut = (HRESULT (WINAPI *) (SCRIPT_STRING_ANALYSIS, int, int, UINT, const RECT *, int, int, BOOL))
							(::GetProcAddress (fDLL, "ScriptStringOut"));
					fScriptStringFree = (HRESULT (WINAPI *) (SCRIPT_STRING_ANALYSIS*))
							(::GetProcAddress (fDLL, "ScriptStringFree"));
					fScriptStringGetLogicalWidths = (HRESULT (WINAPI *) (SCRIPT_STRING_ANALYSIS, int*))
							(::GetProcAddress (fDLL, "ScriptStringGetLogicalWidths"));
					fScriptString_pcOutChars = (const int* (WINAPI *) (SCRIPT_STRING_ANALYSIS))
							(::GetProcAddress (fDLL, "ScriptString_pcOutChars"));
					fScriptString_pSize = (const SIZE* (WINAPI *) (SCRIPT_STRING_ANALYSIS))
							(::GetProcAddress (fDLL, "ScriptString_pSize"));
					fScriptStringCPtoX = (HRESULT (WINAPI *) (SCRIPT_STRING_ANALYSIS, int, BOOL, int*))
							(::GetProcAddress (fDLL, "ScriptStringCPtoX"));
				}
			}
		~UniscribeDLL ()
			{
				if (fDLL != NULL) {
					Led_Verify (::FreeLibrary (fDLL));
				}
			}

		nonvirtual	bool	IsAvail () const
			{
				return fDLL != NULL;
			}

		HRESULT	WINAPI	ScriptItemize (const WCHAR *pwcInChars, int cInChars, int cMaxItems, const SCRIPT_CONTROL *psControl, const SCRIPT_STATE *psState, SCRIPT_ITEM *pItems, int *pcItems)
			{
				if (fScriptItemize == NULL) {
					return E_FAIL;
				}
				return (*fScriptItemize) (pwcInChars, cInChars, cMaxItems, psControl, psState, pItems, pcItems);
			}

		HRESULT	WINAPI	ScriptShape (HDC hdc, SCRIPT_CACHE *psc, const WCHAR *pwcChars, int cChars, int cMaxGlyphs, SCRIPT_ANALYSIS *psa, WORD *pwOutGlyphs, WORD *pwLogClust, SCRIPT_VISATTR *psva, int *pcGlyphs)
			{
				if (fScriptShape == NULL) {
					return E_FAIL;
				}
				return (*fScriptShape) (hdc, psc, pwcChars, cChars, cMaxGlyphs, psa, pwOutGlyphs, pwLogClust, psva, pcGlyphs);
			}

		HRESULT	WINAPI	ScriptPlace (HDC hdc, SCRIPT_CACHE *psc, const WORD *pwGlyphs, int cGlyphs, const SCRIPT_VISATTR *psva, SCRIPT_ANALYSIS *psa, int *piAdvance, GOFFSET* pGoffset, ABC* pABC)
			{
				if (fScriptPlace == NULL) {
					return E_FAIL;
				}
				return (*fScriptPlace) (hdc, psc, pwGlyphs, cGlyphs, psva, psa, piAdvance, pGoffset, pABC);
			}

		HRESULT	WINAPI	ScriptStringAnalyse (HDC hdc, const void *pString, int cString, int cGlyphs, int iCharset, DWORD dwFlags, int iReqWidth, SCRIPT_CONTROL *psControl, SCRIPT_STATE *psState, const int *piDx, SCRIPT_TABDEF *pTabdef, const BYTE *pbInClass, SCRIPT_STRING_ANALYSIS *pssa)
			{
				if (fScriptStringAnalyse == NULL) {
					return E_FAIL;
				}
				return (*fScriptStringAnalyse) (hdc, pString, cString, cGlyphs, iCharset, dwFlags, iReqWidth, psControl, psState, piDx, pTabdef, pbInClass, pssa);
			}
		
		HRESULT	WINAPI	ScriptStringOut (SCRIPT_STRING_ANALYSIS ssa, int iX, int iY, UINT uOptions, const RECT *prc, int iMinSel, int iMaxSel, BOOL fDisabled)
			{
				if (fScriptStringOut == NULL) {
					return E_FAIL;
				}
				return (*fScriptStringOut) (ssa, iX, iY, uOptions, prc, iMinSel, iMaxSel, fDisabled);
			}

		HRESULT	WINAPI	ScriptStringFree (SCRIPT_STRING_ANALYSIS* pssa)
			{
				if (fScriptStringFree == NULL) {
					return E_FAIL;
				}
				return (*fScriptStringFree) (pssa);
			}

		HRESULT	WINAPI	ScriptStringGetLogicalWidths (SCRIPT_STRING_ANALYSIS ssa, int *piDx)
			{
				if (fScriptStringGetLogicalWidths == NULL) {
					return E_FAIL;
				}
				return (*fScriptStringGetLogicalWidths) (ssa, piDx);
			}

		const int*	WINAPI	ScriptString_pcOutChars (SCRIPT_STRING_ANALYSIS ssa)
			{
				if (fScriptString_pcOutChars == NULL) {
					return NULL;
				}
				return (*fScriptString_pcOutChars) (ssa);
			}

		const SIZE*	WINAPI	ScriptString_pSize (SCRIPT_STRING_ANALYSIS ssa)
			{
				if (fScriptString_pSize == NULL) {
					return NULL;
				}
				return (*fScriptString_pSize) (ssa);
			}
		
		HRESULT	WINAPI	ScriptStringCPtoX (SCRIPT_STRING_ANALYSIS ssa, int icp, BOOL fTrailing, int* pX)
			{
				if (fScriptStringCPtoX == NULL) {
					return E_FAIL;
				}
				return (*fScriptStringCPtoX) (ssa, icp, fTrailing, pX);
			}

		HINSTANCE			fDLL;
		HRESULT	 (WINAPI*	fScriptItemize) (const WCHAR*, int, int, const SCRIPT_CONTROL*, const SCRIPT_STATE*, SCRIPT_ITEM*, int*);
		HRESULT	 (WINAPI*	fScriptShape) (HDC, SCRIPT_CACHE*, const WCHAR*, int, int, SCRIPT_ANALYSIS*, WORD*, WORD*, SCRIPT_VISATTR*, int*);
		HRESULT	 (WINAPI*	fScriptPlace) (HDC, SCRIPT_CACHE*, const WORD*, int, const SCRIPT_VISATTR*, SCRIPT_ANALYSIS*, int*, GOFFSET*, ABC*);
		HRESULT	 (WINAPI*	fScriptStringAnalyse) (HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL *, SCRIPT_STATE *, const int *, SCRIPT_TABDEF *, const BYTE *, SCRIPT_STRING_ANALYSIS *);
		HRESULT	 (WINAPI*	fScriptStringOut) (SCRIPT_STRING_ANALYSIS, int, int, UINT, const RECT *, int, int, BOOL);
		HRESULT	 (WINAPI*	fScriptStringFree) (SCRIPT_STRING_ANALYSIS*);
		HRESULT	 (WINAPI*	fScriptStringGetLogicalWidths) (SCRIPT_STRING_ANALYSIS, int*);
		const int*(WINAPI*	fScriptString_pcOutChars) (SCRIPT_STRING_ANALYSIS);
		const SIZE*(WINAPI*	fScriptString_pSize) (SCRIPT_STRING_ANALYSIS);
		HRESULT	 (WINAPI*	fScriptStringCPtoX) (SCRIPT_STRING_ANALYSIS, int, BOOL, int*);
	};
	static	UniscribeDLL	sUniscribeDLL;
#endif




	namespace	{
		inline	bool	IS_WIN30_DIB (const Led_DIB* dib)
			{
				// Logic from MSFT DibLook sample in MSVC.Net 2003
				Led_RequireNotNil (dib);
				const BITMAPINFOHEADER&	hdr	=	dib->bmiHeader;
				return Led_ByteSwapFromWindows (hdr.biSize) == sizeof(BITMAPINFOHEADER);
			}
		inline	size_t	DIBNumColors (const Led_DIB* dib)
			{
				// Logic from MSFT DibLook sample in MSVC.Net 2003
				Led_RequireNotNil (dib);
				const BITMAPINFOHEADER&	hdr	=	dib->bmiHeader;

				/*  If this is a Windows-style DIB, the number of colors in the
				 *  color table can be less than the number of bits per pixel
				 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
				 *  If this is the case, return the appropriate value.
				 */
				if (IS_WIN30_DIB (dib)) {
					unsigned long	clrUsed		=	Led_ByteSwapFromWindows (hdr.biClrUsed);
					if (clrUsed != 0) {
						return clrUsed;
					}
				}

				/*  Calculate the number of colors in the color table based on
				 *  the number of bits per pixel for the DIB.
				 */
				unsigned short	bitCount	=	IS_WIN30_DIB (dib)?
													Led_ByteSwapFromWindows (hdr.biBitCount):
													Led_ByteSwapFromWindows (((const BITMAPCOREHEADER*)dib)->bcBitCount)
												;
	
				/* return number of colors based on bits per pixel */
				switch (bitCount) {
					case 1:				return 2;
					case 4:				return 16;
					case 8:				return 256;
					default:			return 0;
				}
			}
	}


#if		qWindows
	namespace {

		inline	RGBQUAD	mkRGBQuad (COLORREF c)
			{
				RGBQUAD	r;
				r.rgbBlue = GetBValue (c);
				r.rgbGreen = GetGValue (c);
				r.rgbRed = GetRValue (c);
				r.rgbReserved = 0;
				return r;
			}
		inline	void	MaybeAddColorRefToTable_ (RGBQUAD colorTable[256], size_t* iP, COLORREF c)
			{
				Led_Assert (sizeof (RGBQUAD) == sizeof (COLORREF));
				COLORREF*	ct	=	reinterpret_cast<COLORREF*> (colorTable);	// use COLORREF instead of RGBQUAD cuz same size but COLOREF has op== defined
				if (find (ct, ct + *iP, c) == ct + *iP and c != RGB (255,255,255)) {
					colorTable[*iP] = mkRGBQuad (c);
					(*iP)++;
				}
			}

		void	CreateStandardColorTable (RGBQUAD colorTable[256], COLORREF c1 = RGB (0,0,0), COLORREF c2 = RGB (0,0,0), COLORREF c3 = RGB (0,0,0), COLORREF c4 = RGB (0,0,0)) 
			{
				/*
				 *	Cannot use ::GetStockObject (DEFAULT_PALETTE) - because - believe it or not - it returns a 20-entry pallete.
				 */
				(void)::memset (colorTable, 0, sizeof(RGBQUAD) * 256);

				const	BYTE	kColorSpecVals[]	=	{ 0, 32, 64, 128, 192, 255 };
				const	size_t	kColorSpecValCnt	=	sizeof (kColorSpecVals) / sizeof (kColorSpecVals[0]);

				size_t	i			=	0;
				{
					/*
					 *	Fill in the color table with all X x X x X entries where X varies over the
					 *	kColorSpecVals. This gives us a pretty good coverage of colors.
					 */
					size_t	redIdx		=	0;
					size_t	greenIdx	=	0;
					size_t	blueIdx		=	0;
					for (; i < 256; i++) {
						colorTable[i].rgbRed   = kColorSpecVals[redIdx];
						colorTable[i].rgbGreen = kColorSpecVals[greenIdx];
						colorTable[i].rgbBlue  = kColorSpecVals[blueIdx];
						blueIdx++;
						if (blueIdx >= kColorSpecValCnt) {
							blueIdx = 0;
							greenIdx++;
							if (greenIdx >= kColorSpecValCnt) {
								greenIdx = 0;
								redIdx++;
								if (redIdx >= kColorSpecValCnt) {
									Led_Assert (i == kColorSpecValCnt * kColorSpecValCnt * kColorSpecValCnt - 1);
									break;
								}
							}
						}
					}
				}
				/*
				 * Above algorithm wrote out WHITE at the end (RGB (255,255,255)). We will OVERWRITE that item with other stuff,
				 * (shades of gray) to save WHITE for the very end.
				 */

				i--;	// don't count the WHITE color just added. It will be forced to be last.
				size_t	nColorsLeft	=	255 - i;
				Led_Assert (nColorsLeft == 41);


				// Check each color and see if needs to be added. Do this BEFORE removing white so we don't need to check
				// that specially
				MaybeAddColorRefToTable_ (colorTable, &i, c1);
				MaybeAddColorRefToTable_ (colorTable, &i, c2);
				MaybeAddColorRefToTable_ (colorTable, &i, c3);
				MaybeAddColorRefToTable_ (colorTable, &i, c4);

				nColorsLeft	=	255 - i;
				Led_Assert (nColorsLeft > 0);

				size_t	aveSpace	=	255 / nColorsLeft;

				BYTE	startAt	=	aveSpace;
				while (i < 254 and startAt < 255) {
					COLORREF	c	=	RGB (startAt,startAt, startAt);
					Led_Assert (sizeof (RGBQUAD) == sizeof (COLORREF));
					COLORREF*	ct	=	reinterpret_cast<COLORREF*> (colorTable);	// use COLORREF instead of RGBQUAD cuz same size but COLOREF has op== defined
					if (find (ct, ct + i, c) == ct + i) {
						colorTable[i] = mkRGBQuad (c);
						i++;
						nColorsLeft--;
					}
					startAt += aveSpace;
				}

				Led_Assert (nColorsLeft	== 255 - i);

				Led_Assert (nColorsLeft < 5);	// I'm pretty sure this has to get us under 5 - or pretty close...
				Led_Assert (nColorsLeft >= 1);	// I'm pretty sure this has to get us under 5 - or pretty close...

				for (; i <= 255; ++i) {
					colorTable[i].rgbRed   = i;
					colorTable[i].rgbGreen = i;
					colorTable[i].rgbBlue  = i;
				}
			}

		HPALETTE	CreatePaletteForColorTable (const RGBQUAD colorTable[256])
			{
				/*
				 *	Cannot use ::GetStockObject (DEFAULT_PALETTE) - because - believe it or not - it returns a 20-entry pallete.
				 */
				Led_SmallStackBuffer<char>	palBuf (sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 256);
				LPLOGPALETTE				lplgPal = reinterpret_cast<LPLOGPALETTE> (static_cast<char*> (palBuf));

				lplgPal->palVersion = 0x300;
				lplgPal->palNumEntries = 256;

				for (size_t i = 0; i <= 255; ++i) {
					lplgPal->palPalEntry[i].peRed   = colorTable[i].rgbRed;
					lplgPal->palPalEntry[i].peGreen = colorTable[i].rgbGreen;
					lplgPal->palPalEntry[i].peBlue  = colorTable[i].rgbBlue;
					lplgPal->palPalEntry[i].peFlags = 0;
				}
				return ::CreatePalette (lplgPal);
			}

		HPALETTE	CreateStandardPalette (COLORREF c1, COLORREF c2, COLORREF c3, COLORREF c4) 
			{
				RGBQUAD colorTable[256];
				CreateStandardColorTable (colorTable, c1, c2, c3, c4);
				return CreatePaletteForColorTable (colorTable);
			}

		HBITMAP	Create8BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY, const RGBQUAD* colorTable = NULL, LPBYTE* ppBits = NULL)
			{
				LPBYTE	ignored	=	NULL;
				if (ppBits == NULL) {
					ppBits = &ignored;
				}

				const	WORD	wBits		= 8;

				RGBQUAD	colorTableBuf[256];
				if (colorTable == NULL) {
					CreateStandardColorTable (colorTableBuf);
					colorTable = colorTableBuf;
				}

				// Create the header big enough to contain color table and bitmasks if needed
				size_t						nInfoSize = sizeof (BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << wBits);
				Led_SmallStackBuffer<char>	bmiBuf (nInfoSize);
				LPBITMAPINFO				pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
				(void)::memset (pbmi, 0, nInfoSize);
				pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				pbmi->bmiHeader.biWidth = dwX;
				pbmi->bmiHeader.biHeight = dwY;
				pbmi->bmiHeader.biPlanes = 1; 
				pbmi->bmiHeader.biBitCount = wBits;
				pbmi->bmiHeader.biCompression = BI_RGB; // override below for 16 and 32bpp

				for (int i = 0; i < 256; i++) {
					pbmi->bmiColors[i].rgbRed = colorTable[i].rgbRed;
					pbmi->bmiColors[i].rgbGreen = colorTable[i].rgbGreen;
					pbmi->bmiColors[i].rgbBlue = colorTable[i].rgbBlue;
					pbmi->bmiColors[i].rgbReserved = 0;
				}
				pbmi->bmiHeader.biClrUsed = 256;

				return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)ppBits, NULL, 0);
			}

		HBITMAP Create16BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY)
			{
				// Create the header big enough to contain color table and bitmasks if needed
				size_t	nInfoSize = sizeof( BITMAPINFOHEADER ) +  3 * sizeof(DWORD);
				Led_SmallStackBuffer<char>	bmiBuf (nInfoSize);
				LPBITMAPINFO				pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
				(void)::memset (pbmi, 0, nInfoSize);
				pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				pbmi->bmiHeader.biWidth = dwX;
				pbmi->bmiHeader.biHeight = dwY;
				pbmi->bmiHeader.biPlanes = 1; 
				pbmi->bmiHeader.biBitCount = 16;
				{
					// if it's 16bpp, fill in the masks and override the compression
					// these are the default masks - you could change them if needed
					LPDWORD pMasks = (LPDWORD)(pbmi->bmiColors);
					pMasks[0] = 0x00007c00;
					pMasks[1] = 0x000003e0;
					pMasks[2] = 0x0000001f;
					pbmi->bmiHeader.biCompression = BI_BITFIELDS;
				}
				LPBYTE	pBits		=	0;
				return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
			}

		HBITMAP Create32BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY)
			{
				// Create the header big enough to contain color table and bitmasks if needed
				size_t	nInfoSize = sizeof( BITMAPINFOHEADER ) + 3 * sizeof(DWORD);
				Led_SmallStackBuffer<char>	bmiBuf (nInfoSize);
				LPBITMAPINFO				pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
				(void)::memset (pbmi, 0, nInfoSize);
				pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				pbmi->bmiHeader.biWidth = dwX;
				pbmi->bmiHeader.biHeight = dwY;
				pbmi->bmiHeader.biPlanes = 1; 
				pbmi->bmiHeader.biBitCount = 32;
				{
					// if it's 32bpp, fill in the masks and override the compression
					// these are the default masks - you could change them if needed
					LPDWORD pMasks = (LPDWORD)(pbmi->bmiColors);
					pMasks[0] = 0x00ff0000;
					pMasks[1] = 0x0000ff00;
					pMasks[2] = 0x000000ff;
					pbmi->bmiHeader.biCompression = BI_BITFIELDS; 
				}
				LPBYTE	pBits;
				return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
			}
	}
#endif



#if		qWindows && qWideCharacters && qUseFakeTTGetWPlacementToImage
static	bool	Win9x_Workaround_GetCharPlacementFunction (HDC hdc, const wchar_t* srcText, size_t len, wchar_t* glyphImagesOut);
#endif







#if		qWindows
/*
 ********************************************************************************
 ************************************ Led_Bitmap ********************************
 ********************************************************************************
 */
BOOL	Led_Bitmap::CreateCompatibleBitmap (HDC hdc, Led_Distance nWidth, Led_Distance nHeight)
{
	Led_Assert (m_hObject == NULL);
	m_hObject = ::CreateCompatibleBitmap (hdc, nWidth, nHeight);
	fImageSize = Led_Size (nHeight, nWidth);
	return (m_hObject != NULL);	// return value backward compat hack...
}

BOOL	Led_Bitmap::CreateCompatibleDIBSection (HDC hdc, Led_Distance nWidth, Led_Distance nHeight)
{
	Led_RequireNotNil (hdc);
	Led_Require (m_hObject == NULL);
	int	useDepth	=	16;	// default to DIBSection depth - seems to work pretty well in most cases

	fImageSize = Led_Size (nHeight, nWidth);

	int	deviceDepth	=	::GetDeviceCaps (hdc, BITSPIXEL) * ::GetDeviceCaps (hdc, PLANES);
	if (deviceDepth > 16) {
		useDepth = 32;
	}
	else if (deviceDepth <= 8) {
		useDepth = 8;
	}
	switch (useDepth) {
		case	8:	m_hObject = Create8BitDIBSection (hdc, nWidth, nHeight); break;
		case	16:	m_hObject = Create16BitDIBSection (hdc, nWidth, nHeight); break;
		case	32:	m_hObject = Create32BitDIBSection (hdc, nWidth, nHeight); break;
		default:	Led_Assert (false);	//NotReached
	}
	return (m_hObject != NULL);	// return value backward compat hack...
}

void	Led_Bitmap::LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName)
{
	Led_Require (m_hObject == NULL);
	m_hObject = ::LoadBitmap (hInstance, lpBitmapName);
	Led_ThrowIfNull (m_hObject);
	{
		BITMAP bm;
		(void)::memset (&bm, 0, sizeof (bm));
		Led_Verify (::GetObject (m_hObject, sizeof(BITMAP), (LPVOID)&bm));
		fImageSize = Led_Size (bm.bmHeight, bm.bmWidth);
	}
}
#endif









/*
 ********************************************************************************
 ************************************ Led_TWIPS *********************************
 ********************************************************************************
 */
const	Led_TWIPS	Led_TWIPS::kPoint			=	Led_TWIPS (20);		// a printers 'point' (1/72 of an inch)
const	Led_TWIPS	Led_TWIPS::kInch			=	Led_TWIPS (1440);
const	Led_TWIPS	Led_TWIPS::kOneInch			=	Led_TWIPS (1440);






/*
 ********************************************************************************
 ************************************ Led_Color *********************************
 ********************************************************************************
 */
/*
 *	Color name values from page 79 of Web Design in a Nutshell, O'Reilly, table 5-2.
 */
const	Led_Color	Led_Color::kBlack		=	Led_Color (0, 0, 0);
const	Led_Color	Led_Color::kWhite		=	Led_Color (Led_Color::kColorValueMax, Led_Color::kColorValueMax, Led_Color::kColorValueMax);
const	Led_Color	Led_Color::kRed			=	Led_Color (Led_Color::kColorValueMax, 0, 0);
const	Led_Color	Led_Color::kGreen		=	Led_Color (0, Led_Color::kColorValueMax/2, 0);
const	Led_Color	Led_Color::kBlue		=	Led_Color (0, 0, Led_Color::kColorValueMax);
const	Led_Color	Led_Color::kCyan		=	Led_Color (0, Led_Color::kColorValueMax, Led_Color::kColorValueMax);
const	Led_Color	Led_Color::kMagenta		=	Led_Color (Led_Color::kColorValueMax, 0, Led_Color::kColorValueMax);
const	Led_Color	Led_Color::kYellow		=	Led_Color (Led_Color::kColorValueMax, Led_Color::kColorValueMax, 0);
const	Led_Color	Led_Color::kMaroon		=	Led_Color (Led_Color::kColorValueMax/2, 0, 0);
const	Led_Color	Led_Color::kOlive		=	Led_Color (Led_Color::kColorValueMax/2, Led_Color::kColorValueMax/2, 0);
const	Led_Color	Led_Color::kNavyBlue	=	Led_Color (0, 0, Led_Color::kColorValueMax/2);
const	Led_Color	Led_Color::kPurple		=	Led_Color (Led_Color::kColorValueMax/2, 0, Led_Color::kColorValueMax/2);
const	Led_Color	Led_Color::kTeal		=	Led_Color (0, Led_Color::kColorValueMax/2, Led_Color::kColorValueMax/2);
const	Led_Color	Led_Color::kGray		=	Led_Color (Led_Color::kColorValueMax/2, Led_Color::kColorValueMax/2, Led_Color::kColorValueMax/2);
const	Led_Color	Led_Color::kSilver		=	Led_Color (Led_Color::kColorValueMax*3/4, Led_Color::kColorValueMax*3/4, Led_Color::kColorValueMax*3/4);
const	Led_Color	Led_Color::kDarkGreen	=	Led_Color (0, (Led_Color::kColorValueMax/256)*100, 0);
const	Led_Color	Led_Color::kLimeGreen	=	Led_Color (0, Led_Color::kColorValueMax, 0);
const	Led_Color	Led_Color::kFuchsia		=	Led_Color::kMagenta;	// same according to that table
const	Led_Color	Led_Color::kAqua		=	Led_Color::kCyan;		// same according to that table






/*
 ********************************************************************************
 ************************************** Led_Pen *********************************
 ********************************************************************************
 */
#if		qMacOS
const	Pattern	Led_Pen::kWhitePattern			= {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	};	
const	Pattern	Led_Pen::kLightGrayPattern		= {	0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22,	};	
const	Pattern	Led_Pen::kGrayPattern			= {	0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,	};	
const	Pattern	Led_Pen::kDarkGrayPattern		= {	0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd,	};	
const	Pattern	Led_Pen::kBlackPattern			= {	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	};	
#endif





/*
 ********************************************************************************
 ****************************** Led_FontSpecification ***************************
 ********************************************************************************
 */
#if		qXWindows
string	Led_FontSpecification::mkOSRep (const string& foundry, const string& family, const string& weight, const string& slant, const string& pointSize)
{
	char	hRes[1024];
	(void)::sprintf (hRes, "%d", Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
	char	vRes[1024];
	(void)::sprintf (vRes, "%d", Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
	string	result	=	 "-" + foundry + "-" + family + "-" + weight + "-" + slant + "-*-*-*-" + pointSize + "-" + hRes + "-" + vRes + "-*-*-*-*";
	return result;
}

string	Led_FontSpecification::GetOSRep () const
{
	string	foundry		=	"*";
	string	weight		=	fBold? "bold": "medium";
	string	slant		=	fItalics? "i": "r";
	char	pointSize[1024];
	(void)::sprintf (pointSize, "%d", GetPointSize () * 10);
	return mkOSRep (foundry, fFontFamily, weight, slant, pointSize);
}

void	Led_FontSpecification::SetFromOSRep (const string& osRep)
{
	Led_SDK_String	familyName;
	Led_SDK_String	fontSize;
	Led_SDK_String	fontWeight;
	Led_SDK_String	fontSlant;
	Led_Tablet_::ParseFontName (osRep, &familyName, &fontSize, &fontWeight, &fontSlant);
	SetFontName (familyName);
	if (fontSlant == "i") {
		SetStyle_Italic (true);
	}
	else if (fontSlant == "r") {
		SetStyle_Italic (false);
	}
	if (fontWeight == "bold") {
		SetStyle_Bold (true);
	}
	else if (fontWeight == "medium") {
		SetStyle_Bold (false);
	}
	int				fspPointSize	=	GetPointSize ();
	if (::sscanf (fontSize.c_str (), "%d", &fspPointSize) == 1) {
		fspPointSize /= 10;
		if (fspPointSize < 5) {
			fspPointSize = 5;
		}
		if (fspPointSize > 100) {
			fspPointSize = 100;
		}
		SetPointSize (fspPointSize);
	}
}
#endif







/*
 ********************************************************************************
 ************************ Led_IncrementalFontSpecification **********************
 ********************************************************************************
 */


/*
@METHOD:		Intersection
@DESCRIPTION:	<p>Compute the subset of the two @'Led_IncrementalFontSpecification' arguments where both parts
			are valid and identical.</p>
*/
Led_IncrementalFontSpecification	Intersection (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
{
	Led_IncrementalFontSpecification	result	=	lhs;

	// FontName Info
	{
		if (not lhs.GetFontNameSpecifier_Valid () or not rhs.GetFontNameSpecifier_Valid () or
			lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ()
			) {
			result.InvalidateFontNameSpecifier ();
		}
	}

	// Style Info
	{
		if (not lhs.GetStyle_Bold_Valid () or not rhs.GetStyle_Bold_Valid () or
			lhs.GetStyle_Bold () != rhs.GetStyle_Bold ()
			) {
			result.InvalidateStyle_Bold ();
		}
	}
	{
		if (not lhs.GetStyle_Italic_Valid () or not rhs.GetStyle_Italic_Valid () or
			lhs.GetStyle_Italic () != rhs.GetStyle_Italic ()
			) {
			result.InvalidateStyle_Italic ();
		}
	}
	{
		if (not lhs.GetStyle_Underline_Valid () or not rhs.GetStyle_Underline_Valid () or
			lhs.GetStyle_Underline () != rhs.GetStyle_Underline ()
			) {
			result.InvalidateStyle_Underline ();
		}
	}
	{
		if (not lhs.GetStyle_SubOrSuperScript_Valid () or not rhs.GetStyle_SubOrSuperScript_Valid () or
			lhs.GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ()
			) {
			result.InvalidateStyle_SubOrSuperScript ();
		}
	}
	#if		qMacOS
	{
		if (not lhs.GetStyle_Outline_Valid () or not rhs.GetStyle_Outline_Valid () or
			lhs.GetStyle_Outline () != rhs.GetStyle_Outline ()
			) {
			result.InvalidateStyle_Outline ();
		}
	}
	{
		if (not lhs.GetStyle_Shadow_Valid () or not rhs.GetStyle_Shadow_Valid () or
			lhs.GetStyle_Shadow () != rhs.GetStyle_Shadow ()
			) {
			result.InvalidateStyle_Shadow ();
		}
	}
	{
		if (not lhs.GetStyle_Condensed_Valid () or not rhs.GetStyle_Condensed_Valid () or
			lhs.GetStyle_Condensed () != rhs.GetStyle_Condensed ()
			) {
			result.InvalidateStyle_Condensed ();
		}
	}
	{
		if (not lhs.GetStyle_Extended_Valid () or not rhs.GetStyle_Extended_Valid () or
			lhs.GetStyle_Extended () != rhs.GetStyle_Extended ()
			) {
			result.InvalidateStyle_Extended ();
		}
	}
	#elif	qWindows
	{
		if (not lhs.GetStyle_Strikeout_Valid () or not rhs.GetStyle_Strikeout_Valid () or
			lhs.GetStyle_Strikeout () != rhs.GetStyle_Strikeout ()
			) {
			result.InvalidateStyle_Strikeout ();
		}
	}
	#endif

	
	// Font Color Info
	{
		if (not lhs.GetTextColor_Valid () or not rhs.GetTextColor_Valid () or
			lhs.GetTextColor () != rhs.GetTextColor ()
			) {
			result.InvalidateTextColor ();
		}
	}

	// Size Info
	{
		// careful - cuz InvalidatePointSizeIncrement and InvalidatePointSize both
		// invalidate the same thing...
		// Must check that if ANY specification - its the same on both sides (lhs & rhs)
		bool	needsInval	=	false;
		if (lhs.GetPointSizeIncrement_Valid () != rhs.GetPointSizeIncrement_Valid () or
			(lhs.GetPointSizeIncrement_Valid () and lhs.GetPointSizeIncrement () != rhs.GetPointSizeIncrement ())
			) {
			needsInval = true;
		}
		if (lhs.GetPointSize_Valid () != rhs.GetPointSize_Valid () or
			(lhs.GetPointSize_Valid () and lhs.GetPointSize () != rhs.GetPointSize ())
			) {
			needsInval = true;
		}
		if (needsInval) {
			result.InvalidatePointSize ();
		}
	}

	return result;
}







#if		qWindows
/*
 ********************************************************************************
 *************************** Led_Tablet_::RecolorHelper *************************
 ********************************************************************************
 */
class	Led_Tablet_::RecolorHelper {
	public:
		RecolorHelper (HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);
		~RecolorHelper ();

	public:
		static	RecolorHelper*	CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);

	public:
		nonvirtual	void	DoRecolor (const Led_Rect& hilightArea);
	private:
		nonvirtual	void	DoRecolor_SimpleDSTINVERT (const Led_Rect& hilightArea);
		nonvirtual	void	DoRecolor_SimplePATINVERT (const Led_Rect& hilightArea);


	protected:
		nonvirtual	void	DoRecolor_CopyTo8BitManualMungePixAndBack (const Led_Rect& hilightArea);

	private:
		nonvirtual	void	MakeMappingTable ();
	private:
		unsigned char	fMappingTable[256];

	private:
		nonvirtual	size_t	FindClosestColorInColorTable (COLORREF c) const;

	private:
		nonvirtual	COLORREF	MapColor (COLORREF c) const;	// use fHilightBackColor etc to map color
		nonvirtual	COLORREF	MapColor (RGBQUAD c) const;

	private:
		HDC			fBaseDC;
		Led_Size	fSize;
		RGBQUAD		fColorTable[256];
		HBITMAP		fDibSection;
		LPBYTE		fDibData;
		size_t		fDibDataByteCount;
		HDC			fHMemDC;
		HBITMAP		fOldBitmap;
		COLORREF	fHilightBackColor;
		COLORREF	fHilightForeColor;
		COLORREF	fOldBackColor;
		COLORREF	fOldForeColor;
};
	inline	COLORREF	Led_Tablet_::RecolorHelper::MapColor (COLORREF c) const
		{
			float fIntrp;
			fIntrp = (float)(255 - GetRValue (c))/256.0f;
			BYTE	red = static_cast<BYTE> (
						GetRValue (fHilightBackColor) +
						fIntrp * GetRValue (fHilightForeColor) -
						fIntrp * GetRValue (fHilightBackColor)
					);

			fIntrp = (float)(255 - GetGValue (c))/256.0f;
			BYTE	green = static_cast<BYTE> (
						GetGValue (fHilightBackColor) +
						fIntrp * GetGValue (fHilightForeColor) -
						fIntrp * GetGValue (fHilightBackColor)
					);

			fIntrp = (float)(255 - GetBValue (c))/256.0f;
			BYTE	blue = static_cast<BYTE> (
						GetBValue (fHilightBackColor) +
						fIntrp * GetBValue (fHilightForeColor) -
						fIntrp * GetBValue (fHilightBackColor)
					);
			return RGB (red, green, blue);
		}
	inline	COLORREF	Led_Tablet_::RecolorHelper::MapColor (RGBQUAD c) const
		{
			return MapColor (RGB (c.rgbRed, c.rgbGreen, c.rgbBlue));
		}


Led_Tablet_::RecolorHelper::RecolorHelper (HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor):
	//fMappingTable (),
	fDibData (NULL),
	fDibDataByteCount (0),
	fHMemDC (NULL),
	fBaseDC (baseHDC),
	fSize (size),
	fColorTable (),
	fDibSection (NULL),
	fOldBitmap (NULL),
	fHilightBackColor (hilightBackColor.GetOSRep ()),
	fHilightForeColor (hilightForeColor.GetOSRep ()),
	fOldBackColor (oldBackColor.GetOSRep ()),
	fOldForeColor (oldForeColor.GetOSRep ())
{
	CreateStandardColorTable (fColorTable, fHilightBackColor, fHilightForeColor, fOldBackColor, fOldForeColor);
	fDibSection	= Create8BitDIBSection (baseHDC, size.h, size.v, fColorTable, &fDibData);
	BITMAP      bm;
	Led_Verify (::GetObject (fDibSection, sizeof (BITMAP), &bm) == sizeof (BITMAP));
	fDibDataByteCount = bm.bmWidthBytes * bm.bmHeight;
	fHMemDC = ::CreateCompatibleDC (fBaseDC);
	fOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (fHMemDC, fDibSection));
	MakeMappingTable ();
}

Led_Tablet_::RecolorHelper::~RecolorHelper ()
{
	if (fDibSection != NULL) {
		::SelectObject (fHMemDC, fOldBitmap);
		::DeleteDC (fHMemDC);
		::DeleteObject (fDibSection);
	}
}

Led_Tablet_::RecolorHelper*	Led_Tablet_::RecolorHelper::CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
{
	if (_THIS_ == NULL or
		size.h > _THIS_->fSize.h or
		size.v > _THIS_->fSize.v or
		baseHDC != _THIS_->fBaseDC or
		hilightBackColor.GetOSRep () != _THIS_->fHilightBackColor or
		hilightForeColor.GetOSRep () != _THIS_->fHilightForeColor or
		oldBackColor.GetOSRep () != _THIS_->fOldBackColor or
		oldForeColor.GetOSRep () != _THIS_->fOldForeColor
		) {
		Led_Size		areaSize	=	size;
		if (_THIS_ != NULL) {
			areaSize.v = max (size.v, _THIS_->fSize.v);
			areaSize.h = max (size.h, _THIS_->fSize.h);
		}
		RecolorHelper*	tmp	=	new RecolorHelper (baseHDC, areaSize, hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
		delete _THIS_;
		return tmp;
	}
	return _THIS_;
}

void	Led_Tablet_::RecolorHelper::MakeMappingTable ()
{
	for (size_t i = 0; i < 256; ++i) {
		fMappingTable[i] = FindClosestColorInColorTable (MapColor (fColorTable[i]));
	}
}

size_t	Led_Tablet_::RecolorHelper::FindClosestColorInColorTable (COLORREF c) const
{
	// walk through the color table and see which color is closest to 'c'
	size_t			closest	=	0;
	unsigned int	closestDistance	=	0xffffffff;	// big distance
	for (size_t i = 0; i < 256; ++i) {
		unsigned int	thisDist	=	Distance_Squared (c, RGB (fColorTable[i].rgbRed, fColorTable[i].rgbGreen, fColorTable[i].rgbBlue));
		if (thisDist < closestDistance) {
			closest = i;
			closestDistance = thisDist;
			if (closestDistance == 0) {
				break;	// not needed, but COULD be a slight speed tweek
			}
		}
	}
	return closest;
}

void	Led_Tablet_::RecolorHelper::DoRecolor (const Led_Rect& hilightArea)
{
	HPALETTE    hPal	=	NULL;
	HPALETTE	hOldPal	=	NULL;
	if (::GetDeviceCaps (fBaseDC, RASTERCAPS) & RC_PALETTE) {
		// if it's a palette device, select and realize a palette
		// as a background palette (won't cause a problem is the
		// palette was not selected in the foreground in the main app
		hPal = CreatePaletteForColorTable (fColorTable);
		hOldPal = ::SelectPalette (fBaseDC, hPal, TRUE);
		::RealizePalette (fBaseDC);
	}

	DoRecolor_CopyTo8BitManualMungePixAndBack (hilightArea);
	
	if (::GetDeviceCaps (fBaseDC, RASTERCAPS) & RC_PALETTE) {
		::SelectPalette (fBaseDC, hOldPal, TRUE);
		::DeleteObject (hPal);
	}
}

void	Led_Tablet_::RecolorHelper::DoRecolor_SimpleDSTINVERT (const Led_Rect& hilightArea)
{
	// Does proper inverse video, but seems to ignore the TextColor/BkColor/Pen/Brush colors.
	// Really should fix this to behave like Mac - replacing the background color with the text hilight color.
	// See SPR#1271
	::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
						fBaseDC, hilightArea.left, hilightArea.top, DSTINVERT
		);
}

void	Led_Tablet_::RecolorHelper::DoRecolor_SimplePATINVERT (const Led_Rect& hilightArea)
{
	// Attempt at solving SPR#1271. Works decently - producing the right background - but the text is colored YELLOW instead of WHITE - and so
	// doesn't look very good (not enough contrast).
	Led_Color			useColor	=	Led_Color::kWhite - Led_Color (fHilightBackColor);
	HGDIOBJ				oldPen		=	::SelectObject (fBaseDC, ::GetStockObject (NULL_PEN));
	Led_Brush			backgroundBrush (useColor.GetOSRep ());
	HGDIOBJ				oldBrush	=	::SelectObject (fBaseDC, backgroundBrush);
	::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
						fBaseDC, hilightArea.left, hilightArea.top, PATINVERT
		);
	(void)::SelectObject (fBaseDC, oldPen);
	(void)::SelectObject (fBaseDC, oldBrush);
}

void	Led_Tablet_::RecolorHelper::DoRecolor_CopyTo8BitManualMungePixAndBack (const Led_Rect& hilightArea)
{
	// By commenting stuff in and out - I determinted that virtuall ALL the time is spent in this first
	// BitBlt () - LGP 2003-03-11
	// I also found that qUseDIBSectionForOffscreenBitmap made this BitBlt go much faster - to the point of acceptable speed
	// LGP - 2003-03-12

	// Copy the REAL image into our 8-bit DIBSECTION
	::BitBlt (fHMemDC, 0, 0, hilightArea.GetWidth (), hilightArea.GetHeight (),
				fBaseDC, hilightArea.left, hilightArea.top, SRCCOPY
		);

	/*
	 *
	 *	Fiddle the bits:
	 *
	 *	NB:	This code assumes we're pointing at an 8-bit COLOR-LOOKUP-TABLE based Image
	 *
	 *	Note - this also may be modifying MUCH MORE than is actaully needed. It goes all the way to the
	 *	end of the ROW of pixels - but we could be using much less.
	 *
	 *	In order to remedy that - I tried constructing the DIBSECTION on the fly - instead of caching it. That turned
	 *	out to be quite slow (on DELL Precision Workstation 420 (800mz Dual Processor)). This muning code has never shown
	 *	up as taking significant time. Its all that BitBlt above.
	 *
	 *	Anyhow - if this does someday look slow - it can easily be fixed. We can just break the loop into two nested loops,
	 *	the outer one over rows, and the inner row loop stopping NOT at teh end of the REAL row - but just at the end
	 *	of the subset we are using (easy cuz we always start at 0,0).
	 */
	Led_Verify (::GdiFlush ());	// make sure bits in sync... - not SURE if this is needed?
	{
		register const unsigned char*	kMappingTable	=	fMappingTable;
		unsigned char*					dataStart		=	fDibData;
		unsigned char*					dataEnd			=	dataStart + fDibDataByteCount;
		for (register unsigned char* i = dataStart; i < dataEnd; ++i) {
			*i = kMappingTable[*i];
		}
	}

	// Copy them back
	::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
				fHMemDC, 0, 0, SRCCOPY
		);
}
#endif








/*
 ********************************************************************************
 *********************************** Led_Tablet_ ********************************
 ********************************************************************************
 */
#if		qMacOS
Led_Tablet_::Led_Tablet_ (GrafPtr gp):
	fGrafPort (gp)
	{
		Led_RequireNotNil (gp);
	}
#elif	qWindows
Led_Tablet_::Led_Tablet_ (HDC hdc, Led_Tablet_::OwnDCControl ownsDC):
	m_hDC (hdc),
	fRecolorHelper (NULL),
	m_hAttribDC (hdc),
	m_bPrinting (false),
	fOwnsDC (ownsDC),
	fLogPixelsV (0),
	fLogPixelsH (0)
{
}
#elif	qXWindows
Led_Tablet_::Led_Tablet_ (Display* display, Drawable drawable):
	fDrawableOrigin (Led_Point (0, 0)),
	fFontCache (),
	fCurDrawLineLoc (Led_Point (0, 0)),
	fDisplay (display),
	fDrawable (drawable),
	fGC (NULL),
	fColormap (0),
	fCachedFontInfo (NULL),
	fFontMappingCache ()
{
	int	screen = DefaultScreen (display);
	fGC = ::XCreateGC (display, drawable, 0, NULL);
	::XSetForeground (display, fGC, BlackPixel (display,screen));
	::XSetBackground (display, fGC, WhitePixel (display,screen));
	XSetGraphicsExposures (display, fGC, true);
	XWindowAttributes	wa;
	(void)::memset (&wa, 0, sizeof (wa));
	/*
	 *	Since we don't know for sure the drawable is a window - catch the error and ignore it. Don't let
	 *	XErrorHandler do anything bad.
	 */
	int	(*oldErrHandler) (Display*, XErrorEvent*)	=	::XSetErrorHandler (IngoreXErrorHandler);
	Status s = ::XGetWindowAttributes (display, drawable, &wa);
	::XSetErrorHandler (oldErrHandler);
	if (s != 0 && wa.map_installed) {
		fColormap = wa.colormap;
	}
	else {
		fColormap = DefaultColormap (fDisplay, DefaultScreen (fDisplay));
//			Led_Assert (false);//???
			// make new colormap...call XGetWMColormap ()...
		// CALL XSetWindowColormap ().... if not gotten
		// .
	}
}
#endif

Led_Tablet_::~Led_Tablet_ ()
{
	#if		qWindows
		delete fRecolorHelper;
		if (m_hDC != NULL and fOwnsDC == eOwnsDC) {
			::DeleteDC (Detach ());
		}
	#elif	qXWindows
		::XFreeGC (fDisplay, fGC);
		for (map<string,XFontStruct*>::const_iterator	i = fFontCache.begin (); i != fFontCache.end (); ++i) {
			::XFreeFont (fDisplay, i->second);
		}
	#endif
}

/*
@METHOD:		Led_Tablet_::CvtFromTWIPS
@DESCRIPTION:	<p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
	<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_Point	Led_Tablet_::CvtFromTWIPS (Led_TWIPS_Point from) const
{
	return Led_Point (CvtFromTWIPSV (from.v), CvtFromTWIPSH (from.h));
}

/*
@METHOD:		Led_Tablet_::CvtToTWIPS
@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
	<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_TWIPS_Point	Led_Tablet_::CvtToTWIPS (Led_Point from) const
{
	return Led_TWIPS_Point (CvtToTWIPSV (from.v), CvtToTWIPSH (from.h));
}

/*
@METHOD:		Led_Tablet_::CvtFromTWIPS
@DESCRIPTION:	<p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
	<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_Rect		Led_Tablet_::CvtFromTWIPS (Led_TWIPS_Rect from) const
{
	return Led_Rect (CvtFromTWIPS (from.GetOrigin ()), Led_Size (CvtFromTWIPS (from.GetSize ())));
}

/*
@METHOD:		Led_Tablet_::CvtToTWIPS
@DESCRIPTION:	<p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
	<p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_TWIPS_Rect	Led_Tablet_::CvtToTWIPS (Led_Rect from) const
{
	return Led_TWIPS_Rect (CvtToTWIPS (from.GetOrigin ()), CvtToTWIPS (Led_Point (from.GetSize ())));
}

/*
@METHOD:		Led_Tablet_::ScrollBitsAndInvalRevealed
@DESCRIPTION:	<p>Scroll the given 'windowRect' by 'scrollVBy localical units. The area of the window exposed by this
			action is invalidated (so a later update event will fix it).</p>
*/
void	Led_Tablet_::ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, Led_Coordinate scrollVBy)
{
	#if		qMacOS
		Rect			qdMoveRect	=	AsQDRect (windowRect);
		RgnHandle		updateRgn	=	::NewRgn ();
		Led_ThrowIfNull (updateRgn);
		SetPort ();
		::ScrollRect (&qdMoveRect, 0, scrollVBy, updateRgn);
		#if		TARGET_CARBON
			::InvalWindowRgn (::GetWindowFromPort (fGrafPort), updateRgn);
		#else
			::InvalRgn (updateRgn);
		#endif
		::DisposeRgn (updateRgn);
	#elif	qWindows
		RECT	gdiMoveRect	=	AsRECT (windowRect);
		// NB: I used to use ScrollDC (Led 2.1 and earlier). But that code appeared to sometimes leave
		// little bits of crufy around. I never understood why. But I assume it was a windows bug.
		HWND	w	=	GetWindow ();
		Led_ThrowIfNull (w);
		::ScrollWindow(w, 0, scrollVBy, &gdiMoveRect, &gdiMoveRect);
	#elif	qXWindows
		if (scrollVBy != 0) {
			{
				/*
				 *	We cannot do a scrollbits if there are any pending update events. Ideally - we would PREVENT
				 *	this situation by having Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Update_ () work properly.
				 *	But - alas - after a day or two's efforts - I've been unable to get that code working.
				 *	Sigh. Luckily - this seems to prevent any drawing bugs, and only results in an occasional
				 *	drawing (scrolling) slowdown. I guess we can live with that. -- LGP 2001-05-18
				 */
				XEvent e;
				if (::XCheckTypedEvent (fDisplay, Expose, &e) or ::XCheckTypedEvent (fDisplay, GraphicsExpose, &e)) {
					::XPutBackEvent (fDisplay, &e);
					Led_ThrowIfNull (0);
				}
			}
			Led_Rect	srcMoveRect	=	windowRect;
			Led_Rect	exposedRect	=	windowRect;
			if (scrollVBy > 0) {
				// moving bits down (up scrollbar button)
				srcMoveRect.bottom -= scrollVBy;
				exposedRect.bottom = scrollVBy;
			}
			else {
				srcMoveRect.top -= scrollVBy;
				exposedRect.top = exposedRect.bottom + scrollVBy;
			}
			XGCValues	prevValues;
			const unsigned long	kSavedAttrs	=	GCGraphicsExposures;
			(void)::memset (&prevValues, 0, sizeof (prevValues));
			::XGetGCValues (fDisplay, fGC, kSavedAttrs, &prevValues);
			::XSetClipMask (fDisplay, fGC, None);
			::XSetGraphicsExposures (fDisplay, fGC, true);
			::XCopyArea (fDisplay, fDrawable, fDrawable, fGC,
						srcMoveRect.GetLeft (), srcMoveRect.GetTop (),
						srcMoveRect.GetWidth (), srcMoveRect.GetHeight (),
						srcMoveRect.GetLeft (), srcMoveRect.top + scrollVBy
					);
			::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);

			/*
			 *	After the scrollbits - we leave a little rectangle exposed. We must mark that as needing drawing.
			 */
			#if 1
				XEvent	event;
				(void)::memset (&event, 0, sizeof (event));
				event.type = Expose;
				event.xexpose.send_event = true;
				event.xexpose.display = fDisplay;
				event.xexpose.window = fDrawable;
				event.xexpose.x = (int)exposedRect.GetLeft ();
				event.xexpose.y = (int)exposedRect.GetTop ();
				event.xexpose.width = (int)exposedRect.GetWidth ();
				event.xexpose.height = (int)exposedRect.GetHeight ();
				event.xexpose.count = 0;
				Led_Verify (::XSendEvent (fDisplay, fDrawable, false, ExposureMask, &event) != 0);
			#else
				::XClearArea (fDisplay, fDrawable, (int)exposedRect.GetLeft (), (int)exposedRect.GetTop (),
					  (unsigned int)exposedRect.GetWidth (), (unsigned int)exposedRect.GetHeight (), true
					);
			#endif
		}
	#else
		Led_Assert (false);	//NYI
	#endif
}

/*
@METHOD:		Led_Tablet_::FrameRegion
@DESCRIPTION:	<p>Draw the outline of the given region 'r' in color 'c'.</p>
*/
void	Led_Tablet_::FrameRegion (const Led_Region& r, const Led_Color& c)
{
	#if		qMacOS
		Led_MacPortAndClipRegionEtcSaver	saver;		// unclear if this is useful/needed?
		SetPort ();
		PenMode (srcCopy);	// ???
		GDI_RGBForeColor (c.GetOSRep ());
		::FrameRgn (r.GetOSRep ());
	#elif	qWindows
		Led_Brush	brush	=	Led_Brush (c.GetOSRep ());
		(void)::FrameRgn (*this, r, brush, 1, 1);
	#else
		Led_Assert (false);	// NYI
	#endif
}

/*
@METHOD:		Led_Tablet_::FrameRectangle
@DESCRIPTION:	<p>Draw the outline of the given rectangle 'r' in color 'c' and with
			borderWidth (pen width) 'borderWidth'. This function does NOT use the currently selected
			pen or brush, or anything like that. It draws a border just INSIDE the rectangle specified
			by 'r'.
				</p>
*/
void	Led_Tablet_::FrameRectangle (const Led_Rect& r, Led_Color c, Led_Distance borderWidth)
{
	/*
	 *	Almost certainly can implement much more efficiently, but leave like this for now to assure pixel-for-pixel
	 *	equiv across GDIs.
	 */
	Led_Rect	topBar	=	Led_Rect (r.top, r.left, borderWidth, r.GetWidth ());
	Led_Rect	leftBar	=	Led_Rect (r.top, r.left, r.GetHeight (), borderWidth);
	EraseBackground_SolidHelper (topBar, c);												// TOP
	EraseBackground_SolidHelper (leftBar, c);												// LEFT
	EraseBackground_SolidHelper (topBar + Led_Point (r.GetHeight () - borderWidth, 0), c);	// BOTTOM
	EraseBackground_SolidHelper (leftBar + Led_Point (0, r.GetWidth () - borderWidth), c);	// RIGHT
}

/*
@METHOD:		Led_Tablet_::MeasureText
@DESCRIPTION:	<p>Measure the widths of the given argument @'Led_tChar's. Assign those widths into
			the array 'charLocations'. (EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
			zero case and offset rules, handling of tabs, etc)</p>
				<p>Note that the resulting measured text must come out in non-descreasing order (there can be zero
			character widths, but never negative).</p>
*/
void	Led_Tablet_::MeasureText (const Led_FontMetrics& precomputedFontMetrics,
											const Led_tChar* text, size_t nTChars, Led_Distance* charLocations
										)
{
	Led_RequireNotNil (text);
	Led_RequireNotNil (charLocations);
	#if		qMacOS
		SetPort ();
	#endif

	#if		qMacOS
		const	Led_Distance	kMaxTextWidthResult	=	0x7fff;
	#elif	qWindows
		Led_Distance	kMaxTextWidthResult	=	kRunning32BitGDI? 0x7fffffff: 0x7fff;
		if (IsPrinting ()) {
			// See SPR#0435
			SIZE	ve	=	GetViewportExt ();
			SIZE	we	=	GetWindowExt ();
			kMaxTextWidthResult = ::MulDiv (kMaxTextWidthResult, we.cx, ve.cx) - 1;
		}
	#elif	qXWindows
		const	Led_Distance	kMaxTextWidthResult	=	0x7fff;		//X-TMP-HACK-LGP991213
	#endif
	size_t	kMaxChars	=	kMaxTextWidthResult/precomputedFontMetrics.GetMaxCharacterWidth ();
	#if		qUseUniscribeToImage
		if (kMaxChars > kMaxUNISCRIBECharacters) {
			kMaxChars = kMaxUNISCRIBECharacters;
		}
	#endif
	Led_Assert (kMaxChars > 1);

	Led_Distance	runningCharCount = 0;
	for (size_t charsToGo = nTChars; charsToGo > 0; ) {
		size_t	i				=	nTChars - charsToGo;
		Led_Assert (i < nTChars);

		while (text[i] == '\t') {
			Led_Assert (charsToGo > 0);

			charLocations[i] = runningCharCount;
			i++;
			if (--charsToGo == 0) {
				break;
			}
		}
		if (charsToGo == 0) {
			break;
		}
		
		size_t	charsThisTime	=	Led_Min (charsToGo, kMaxChars);
		for (size_t tabIndex = 1; tabIndex < charsThisTime; tabIndex++) {
			if (text[i+tabIndex] == '\t') {
				charLocations[i+tabIndex] = runningCharCount;
				charsThisTime = tabIndex;
				break;
			}
		}

		#if		qMultiByteCharacters
			/*
			 *	See if i+charsThisTime is on a real character boundary - and if not - then step back one so it is.
			 */
			if (Led_FindPrevOrEqualCharBoundary (&text[0], &text[i+charsThisTime]) != &text[i+charsThisTime]) {
				Led_Assert (charsThisTime > 0);
				charsThisTime--;
			}
		#endif

		#if		qMacOS
			Led_SmallStackBuffer<short>	shortOffsets (charsThisTime + 1);
			Led_Assert (Led_GetCurrentGDIPort () == *this);
			::MeasureText (charsThisTime, &text[i], shortOffsets);
			for (size_t j = 0; j < charsThisTime; j++) {
				charLocations[i+j] = shortOffsets[j+1] + runningCharCount;		// Silly Apple defines shortOffsets[0] always to be zero!
			}
		#elif	qWindows
			SIZE	size;
			Led_Assert (sizeof (int) == sizeof (Led_Distance));
			#if		qUseUniscribeToImage && qWideCharacters
			{
				if (sUniscribeDLL.IsAvail ()) {
					SCRIPT_CONTROL			scriptControl;
					memset (&scriptControl, 0, sizeof (scriptControl));

					SCRIPT_STATE			scriptState;
					memset (&scriptState, 0, sizeof (scriptState));
// Important to ALLOW ScriptStringAnalyse to REORDER (so dont set this true) cuz otherwise it won't get right measurements
// for arabic font substition (shaping)---LGP 2003-01-02

// MAYBE THIS IS WRONG - AND COVERING UP ANOTHER BUG??? DUNNO? MUST BE CAREFUL ABOUT MIRRORING (SYMSWAP). If done HERE,
// then I must NOT inhibit symswap. I DONT THINK I CAN DO it here cuz the draw code gets done in RUNS... HMMM
// LGP 2003-01-02...
				//	scriptState.fOverrideDirection  = true;		// I THINK This is how I say already in display order
					scriptState.fInhibitSymSwap = true;

					SCRIPT_STRING_ANALYSIS	ssa;
					memset (&ssa, 0, sizeof (ssa));

					Led_Verify (sUniscribeDLL.ScriptStringAnalyse (m_hAttribDC, &text[i], charsThisTime, 0, -1, SSA_GLYPHS|SSA_FALLBACK, -1, &scriptControl, &scriptState, NULL, NULL, NULL, &ssa) == S_OK);

#if		qTryScriptToCPX
					for (size_t j = 0; j < charsThisTime; j++) {
						int	leadingEdge		=	0;
						int	trailingEdge	=	0;
						Led_Verify (sUniscribeDLL.ScriptStringCPtoX (ssa, j, false, &leadingEdge) == S_OK);
						Led_Verify (sUniscribeDLL.ScriptStringCPtoX (ssa, j, true, &trailingEdge) == S_OK);

						int	logicalWidth	=	abs (trailingEdge-leadingEdge);		// can be zero-width - but never negative...
						if (j == 0) {
							charLocations[i+j] = runningCharCount + logicalWidth;
						}
						else {
							charLocations[i+j] = charLocations[i+j-1] + logicalWidth;
						}
					}
#else
					Led_SmallStackBuffer<int> logicalWidths (charsThisTime);
					Led_Verify (sUniscribeDLL.ScriptStringGetLogicalWidths (ssa, logicalWidths) == S_OK);

					Led_Assert (charsThisTime > 0);
					Led_Assert (logicalWidths[0] >= 0);	// can be zero-width - but never negative...
					charLocations[i] = runningCharCount + logicalWidths[0];
					for (size_t j = 1; j < charsThisTime; j++) {
						Led_Assert (logicalWidths[j] >= 0);	// can be zero-width - but never negative...
						charLocations[i+j] = charLocations[i+j-1] + logicalWidths[j];
					}
#endif
					Led_Verify (sUniscribeDLL.ScriptStringFree (&ssa) == S_OK);
					goto Succeeded;
				}
			}
			#endif

			// Default code - if UNISCRIBE not compiled for or not dynamically loaded
			Win32_GetTextExtentExPoint (m_hAttribDC, &text[i], charsThisTime, kMaxTextWidthResult, NULL, (int*)&charLocations[i], &size);
			for (size_t j = 0; j < charsThisTime; j++) {
				charLocations[i+j] += runningCharCount;
			}

			#if		qUseUniscribeToImage && qWideCharacters
	Succeeded:
			#endif
		#elif	qXWindows
			Led_ThrowIfNull (fCachedFontInfo);
			// Gross hack - sloppy implementation (SLOW). But I'm not sure what in the X SDK allows this to be done faster! -- LGP 2000-09-05
			// Actually - not TOO bad since whole computation is done client-side. Seems to be working OK - at least for now - LGP 2001-05-05
			for (size_t j = 0; j < charsThisTime; ++j) {
				charLocations[i+j] = runningCharCount + ::XTextWidth (const_cast<XFontStruct*> (fCachedFontInfo), &text[i], j+1);
			}
		#endif

		runningCharCount = charLocations[i+charsThisTime-1];
		
		Led_Assert (charsToGo >= charsThisTime);
		charsToGo -= charsThisTime;
	}

// LGP-991220 - This is generating asserts elsewhere - and seems like such a hack. Not sure why needed. Try getting rid of and see what happens?
	#if		qWindows && 0
	// This gross hack is cuz we do a GetTextExtent() at the end of the
	// DrawText for PC, to see how much we drew. This is the only hack
	// I could think of to assure we get consistent results (which is very important).
	// This REALLY shold be done better - fix may not be here, but in DrawCode below...
	// LGP 960509

	// Now, special hack- assert no qMultiByteCharacters
	if (nTChars > 0) {
		int	lastWidth = (nTChars==1)?charLocations[0]: (charLocations[nTChars-1]-charLocations[nTChars-2]);
		SIZE	size;
		Led_AssertNotNil (m_hAttribDC);
		Win32_GetTextExtentPoint (m_hAttribDC, &text[nTChars-1], 1, &size);
		int	sbWidth = size.cx;
		if (sbWidth != lastWidth) {
			charLocations[nTChars-1] = ((nTChars==1)? 0: charLocations[nTChars-2]) + sbWidth;
		}
	}
	#endif

	#if		qDebug
	{
		// Assure charLocations are in non-decreasing order (OK to have some zero width - but never negative).
		Led_Distance	d	=	0;
		for (size_t i = 0; i < nTChars; ++i) {
			Led_Ensure (d <= charLocations[i]);
			d = charLocations[i];
		}
	}
	#endif
}

/*
@METHOD:		Led_Tablet_::TabbedTextOut
@DESCRIPTION:	<p>Draw the given text (@'Led_tChars') to this tablet object, at the given logical coordinates. Use the given
			tabstop origin, and tabStopList object to compute where tabs go. Return the amountDrawn (number of pixels used by draw).
			(EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
			zero case and offset rules, handling of tabs, AND MUCH MORE etc)</p>
				<p>Note - for BIDI text - the VIRTUAL (display) text is what should be passed in. Text will be imaged
			left to right. This means that for RTL text such as Arabic or Hebrew - it should have already been
			re-ordered in the argument passed in, and any mirroring should have already been done. This routine WILL
			take care of any contextual shaping required (glyph selection based on context - as with Arabic).</p>
*/
void	Led_Tablet_::TabbedTextOut (const Led_FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
												TextDirection direction,
												Led_Point outputAt, Led_Coordinate hTabOrigin, const Led_TabStopList& tabStopList,
												Led_Distance* amountDrawn, Led_Coordinate hScrollOffset
											)
{
	#if		!qWideCharacters
		Led_Arg_Unused (direction);
	#endif
	#if		qMacOS
		SetPort ();
	#endif

	Led_Distance		widthSoFar		=	0;
	const	Led_tChar*	textCursor		=	text;
	const	Led_tChar*	textEnd			=	text + nBytes;
	while (textCursor < textEnd) {
		// Skip over tabs
		const Led_tChar* nextTabAt = textCursor;
		for (; nextTabAt < textEnd; ) {
			if (*nextTabAt == '\t') {
				break;
			}

			// We can get away with nextTabAt++ even under SJIS so long as...
			#if		qMultiByteCharacters
				Led_Assert (not Led_IsValidSecondByte ('\t'));
			#endif
			nextTabAt++;
		}

		// Actually image the characters
		#if		qMacOS
			Led_Assert (Led_GetCurrentGDIPort () == *this);
			Led_Point	cursor	=	Led_Point (outputAt.v + precomputedFontMetrics.GetAscent (), outputAt.h - hScrollOffset);	// ascent - goto baseline...
			::MoveTo (cursor.h+widthSoFar, cursor.v);
			::TextMode (srcOr);
			::DrawText (textCursor, 0, nextTabAt-textCursor);
			#if		TARGET_CARBON
				{
					Point	junk;
					widthSoFar = ::GetPortPenLocation (Led_GetCurrentGDIPort (), &junk)->h - cursor.h;
				}
			#else
				widthSoFar = Led_GetCurrentGDIPort ()->pnLoc.h - cursor.h;
			#endif
		#elif	qWindows
			int oldBkMode = SetBkMode (TRANSPARENT);
			
			#if		qUseUniscribeToImage && qWideCharacters
				{
					#if		qTryToOptimizeLongUNISCRIBEScriptOutCalls
						const	size_t	kMaxCharsToDrawAtATime	=	500;
					#endif
					size_t							len = nextTabAt-textCursor;
					if (len == 0) {
						goto Succeeded;		// UNISCRIBE barfs on zero-length strings. Nothing todo anyhow...
					}
					if (sUniscribeDLL.IsAvail ()) {
						SCRIPT_CONTROL			scriptControl;
						memset (&scriptControl, 0, sizeof (scriptControl));

						SCRIPT_STATE			scriptState;
						memset (&scriptState, 0, sizeof (scriptState));
						scriptState.fOverrideDirection  = true;		// I THINK This is how I say already in display order
						scriptState.fInhibitSymSwap = true;

						const Led_tChar* thisChunkPtr = textCursor;
						for (size_t thisChunkLen = len; thisChunkLen > 0; ) {
							if (thisChunkLen > kMaxUNISCRIBECharacters) {
								thisChunkLen = kMaxUNISCRIBECharacters;
							}
							#if		qTryToOptimizeLongUNISCRIBEScriptOutCalls
								if (thisChunkLen > kMaxCharsToDrawAtATime) {
									thisChunkLen = kMaxCharsToDrawAtATime;
								}
							#endif
							{
								SCRIPT_STRING_ANALYSIS	ssa;
								memset (&ssa, 0, sizeof (ssa));
								if (not SUCCEEDED (sUniscribeDLL.ScriptStringAnalyse (m_hDC, thisChunkPtr, thisChunkLen, 0, -1, SSA_GLYPHS|SSA_FALLBACK, -1, &scriptControl, &scriptState, NULL, NULL, NULL, &ssa))) {
									goto UniscribeFailure;	// Can happen - for example - during ColeControl::DrawMetaFile ()
															// call - see SPR#1447 - fallback on older draw code...
								}
								Led_Verify (sUniscribeDLL.ScriptStringOut (ssa, outputAt.h+ int (widthSoFar) - hScrollOffset, outputAt.v, 0, NULL, 0, 0, false) == S_OK);
								const SIZE*	sizep	=	sUniscribeDLL.ScriptString_pSize (ssa);
								Led_AssertNotNil (sizep);
								widthSoFar +=	sizep->cx;
								Led_Verify (sUniscribeDLL.ScriptStringFree (&ssa) == S_OK);
							}

							#if		qTryToOptimizeLongUNISCRIBEScriptOutCalls
								// only rarely (tune this) - check if we've already drawn past the end of the HDC.
								// (not REALLY doing a great/reliable test for that either???
								if (len > kMaxCharsToDrawAtATime) {
									POINT	vpOrg;
									Led_Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
									POINT	wOrg;
									Led_Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
									int	deviceWidth	=	GetDeviceCaps (HORZRES);
									POINT	x	=	vpOrg;
									x.x += deviceWidth;
									Led_Verify (::DPtoLP (m_hAttribDC, &x, 1));
									if (x.x < outputAt.h+int (widthSoFar) - hScrollOffset) {
										// assume we're done - and can break out...
										break;
									}
								}
							#endif

							thisChunkPtr += thisChunkLen;
							thisChunkLen = (textCursor + len - thisChunkPtr);	// set length left to go to be end of REAL buf minus new start ptr
																				// at TOP of loop - it will be trimmed down to kMaxUNISCRIBECharacters
						}
						goto Succeeded;
					}
				}
UniscribeFailure:
			#endif

			if (direction==eLeftToRight) {
				Win32_TextOut (m_hDC, outputAt.h+widthSoFar - hScrollOffset, outputAt.v, textCursor, nextTabAt-textCursor);

				// Geez! There must be SOME API within Win32 to give me this info (like SetTextAlign (UPDATE_CP))
				// without recomputing it. But there doesn't appear to be. So we must recompute!
				// LGP 960503
				
				// A SLIGHT optimization is now possible here - we don't need to compute the last GetTextExtent() if amountDrawn
				// is NULL (typically TRUE) - LGP 960521
				if (amountDrawn != NULL or (nextTabAt < textEnd)) {
					SIZE	size;
					Win32_GetTextExtentPoint (m_hAttribDC, textCursor, nextTabAt-textCursor, &size);
					widthSoFar +=	size.cx;
				}
			}
			else {
				/*
				 *	A bunch of different ways to get the RTL code emitted. Try them each in order (some ifdefed out). When
				 *	one succeeds - just to the succcess label.
				 */
				#if		qUseGetCharPlacementToImage && qWideCharacters
				{
					size_t							len = nextTabAt-textCursor;
					Led_SmallStackBuffer<wchar_t>	glyphs (len);
					GCP_RESULTSW					gcpResult;
					memset (&gcpResult, 0, sizeof (gcpResult));
					gcpResult.lStructSize = sizeof(GCP_RESULTS);
					gcpResult.lpGlyphs = glyphs;
					gcpResult.nGlyphs = len;
					if (::GetCharacterPlacementW (m_hDC, textCursor, len, 0, &gcpResult, GCP_GLYPHSHAPE | GCP_LIGATE) != 0) {
						Led_Verify (::ExtTextOutW (m_hDC, outputAt.h+widthSoFar - hScrollOffset, outputAt.v, ETO_GLYPH_INDEX, NULL, gcpResult.lpGlyphs, gcpResult.nGlyphs, NULL));
						goto Succeeded_But_Need_To_Adjust_Width;
					}
				}
				#endif

				#if		qUseFakeTTGetWPlacementToImage && qWideCharacters
				{
					size_t							len = nextTabAt-textCursor;
					Led_SmallStackBuffer<wchar_t>	glyphs (len);
					if (Win9x_Workaround_GetCharPlacementFunction (m_hDC, textCursor, len, glyphs) != 0) {
						Led_Verify (::ExtTextOutW (m_hDC, outputAt.h+widthSoFar - hScrollOffset, outputAt.v, ETO_GLYPH_INDEX, NULL, glyphs, len, NULL));
						goto Succeeded_But_Need_To_Adjust_Width;
					}
				}
				#endif

				{
					size_t							len = nextTabAt-textCursor;
					// Fallback - if the above fails...
					// Displays the text in the right order, but doesn't do contextual shaping (tested on WinXP and WinME) - LGP 2002-12-10
					#if		qWideCharacters
						Led_Verify (::ExtTextOutW (m_hDC, outputAt.h+widthSoFar - hScrollOffset, outputAt.v, 0, NULL, textCursor, len, NULL));
					#else
						Led_Verify (::ExtTextOutA (m_hDC, outputAt.h+widthSoFar - hScrollOffset, outputAt.v, 0, NULL, textCursor, len, NULL));
					#endif
				}


Succeeded_But_Need_To_Adjust_Width:
				// Geez! There must be SOME API within Win32 to give me this info (like SetTextAlign (UPDATE_CP))
				// without recomputing it. But there doesn't appear to be. So we must recompute!
				// LGP 960503
				
				// A SLIGHT optimization is now possible here - we don't need to compute the last GetTextExtent() if amountDrawn
				// is NULL (typically TRUE) - LGP 960521
				if (amountDrawn != NULL or (nextTabAt < textEnd)) {
					SIZE	size;
					Win32_GetTextExtentPoint (m_hAttribDC, textCursor, nextTabAt-textCursor, &size);
					widthSoFar +=	size.cx;
				}
			}
#if		qUseUniscribeToImage && qWideCharacters
Succeeded:
	;
#endif

			(void)SetBkMode (oldBkMode);
		#elif	qXWindows
			Led_Point	cursor	=	Led_Point (outputAt.v + precomputedFontMetrics.GetAscent (), outputAt.h - hScrollOffset) - fDrawableOrigin;	// ascent - goto baseline...
			XTextItem	item;
			memset (&item, 0, sizeof (item));
			item.chars = const_cast<char*> (textCursor);
			item.nchars = nextTabAt-textCursor;
			item.delta = 0;
			item.font = None;
			::XDrawText (fDisplay, fDrawable, fGC, cursor.h+widthSoFar, cursor.v, &item, 1);
			Led_ThrowIfNull (fCachedFontInfo);
			widthSoFar += ::XTextWidth (const_cast<XFontStruct*> (fCachedFontInfo), item.chars, item.nchars);
		#endif

		// Now see if nextTab really pointing at a tab (otherwise at end of buffer)
		if (nextTabAt < textEnd) {
			Led_Distance	thisTabWidth;
			{
				Led_Distance	curOutputAtZeroBased	=	(outputAt.h - hTabOrigin) + widthSoFar;
				Led_Distance	tabStop = tabStopList.ComputeTabStopAfterPosition (this, curOutputAtZeroBased);
				thisTabWidth = tabStop - curOutputAtZeroBased;
				Led_Assert (thisTabWidth >= 0);
			}

			widthSoFar += thisTabWidth;
			nextTabAt++;	// since we processed that tab...
		}
		textCursor = nextTabAt;
	}
	if (amountDrawn != NULL) {
		*amountDrawn = widthSoFar;
	}
}

void	Led_Tablet_::SetBackColor (const Led_Color& backColor)
{
	#if		qMacOS
		SetPort ();
		GDI_RGBBackColor (backColor.GetOSRep ());
	#elif	qWindows
		SetBkColor (backColor.GetOSRep ());
	#elif	qXWindows
		if (backColor == Led_Color::kWhite) {
			::XSetBackground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
		}
		else if (backColor == Led_Color::kBlack) {
			::XSetBackground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
		}
		else {
			XColor		bgColorDef;
			memset (&bgColorDef, 0, sizeof (bgColorDef));
			bgColorDef.red = backColor.GetRed ();
			bgColorDef.green = backColor.GetGreen ();
			bgColorDef.blue = backColor.GetBlue ();
			Colormap	cmap	=	DefaultColormap (fDisplay, DefaultScreen (fDisplay));
			Status		s		=	XAllocColor (fDisplay, cmap, &bgColorDef);
			if (s == 0) {
				::XSetBackground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
			}
			else {
				::XSetBackground (fDisplay, fGC, bgColorDef.pixel);
			}
		}
	#endif
}

void	Led_Tablet_::SetForeColor (const Led_Color& foreColor)
{
	#if		qMacOS
		SetPort ();
		GDI_RGBForeColor (foreColor.GetOSRep ());
	#elif	qWindows
		SetTextColor (foreColor.GetOSRep ());
	#elif	qXWindows
		if (foreColor == Led_Color::kWhite) {
			::XSetForeground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
		}
		else if (foreColor == Led_Color::kBlack) {
			::XSetForeground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
		}
		else {
			XColor		fgColorDef;
			memset (&fgColorDef, 0, sizeof (fgColorDef));
			fgColorDef.red = foreColor.GetRed ();
			fgColorDef.green = foreColor.GetGreen ();
			fgColorDef.blue = foreColor.GetBlue ();
			Colormap	cmap	=	DefaultColormap (fDisplay, DefaultScreen (fDisplay));
			Status		s		=	::XAllocColor (fDisplay, cmap, &fgColorDef);
			if (s == 0) {
				::XSetForeground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
			}
			else {
				::XSetForeground (fDisplay, fGC, fgColorDef.pixel);
			}
		}
	#endif
}

/*
@METHOD:		Led_Tablet_::EraseBackground_SolidHelper
@DESCRIPTION:	<p>EraseBackground_SolidHelper () is simple helper function - usually called from subclasses which override
	@'TextImager::EraseBackground'.</p>
*/
void	Led_Tablet_::EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Led_Color& eraseColor)
{
	if (not eraseRect.IsEmpty ()) {
		#if		qMacOS
			SetPort ();
			Rect	qdEraser = AsQDRect (eraseRect);
			GDI_RGBForeColor (eraseColor.GetOSRep ());
			::FillRect (&qdEraser, &Led_Pen::kBlackPattern);
		#elif	qWindows
			Led_Rect				eraser	=	eraseRect;
			Led_Brush				backgroundBrush (eraseColor.GetOSRep ());
			Led_Win_Obj_Selector	pen (this, ::GetStockObject (NULL_PEN));
			Led_Win_Obj_Selector	brush (this, backgroundBrush);
			eraser.right++;	// lovely - windows doesn't count last pixel... See Docs for Rectangle() and rephrase!!!
			eraser.bottom++;
			Rectangle (AsRECT (eraser));
		#elif	qXWindows
			XGCValues	prevValues;
			const unsigned long	kSavedAttrs	=	GCForeground;
			Colormap	cmap	=	DefaultColormap (fDisplay, 0);
			XColor		fgColorDef;
			memset (&fgColorDef, 0, sizeof (fgColorDef));
			fgColorDef.red = eraseColor.GetRed ();
			fgColorDef.green = eraseColor.GetGreen ();
			fgColorDef.blue = eraseColor.GetBlue ();
			Status	s	=	::XAllocColor (fDisplay, cmap, &fgColorDef);
			if (s != 0) {
				::XSetForeground (fDisplay, fGC, fgColorDef.pixel);
			}
			Led_Rect	adjustedEraseRect	=	eraseRect - fDrawableOrigin;
			::XFillRectangle (fDisplay, fDrawable, fGC, adjustedEraseRect.GetLeft (), adjustedEraseRect.GetTop (), adjustedEraseRect.GetWidth (), adjustedEraseRect.GetHeight ());
			::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);
		#endif
	}
}

#if		qSupportLed30CompatAPI
/*
@METHOD:		Led_Tablet_::HilightARectangle__SolidHelper
@DESCRIPTION:	<p>HilightARectangle__SolidHelper () is simple helper function - usually called from subclasses which override
			@'TextImager::HilightARectangle'.</p>
				<p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
			platform UI conventionally calls for) inverting the	text via a simple XOR.</p>
				<p>OBSOLETE - Use @'Led_Tablet_::HilightArea__SolidHelper' instead.</p>
*/
void	Led_Tablet_::HilightARectangle_SolidHelper (const Led_Rect& hilightRect, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
{
	if (not hilightRect.IsEmpty ()) {
		#if		qMacOS
			Led_Arg_Unused (hilightBackColor);
			Led_Arg_Unused (hilightForeColor);
			Led_Arg_Unused (oldForeColor);
			SetPort ();
			LMSetHiliteMode (LMGetHiliteMode () & 0x7F);
			GDI_RGBBackColor (oldBackColor.GetOSRep ());	// Mac HilightMode code already knows the hilightBackColor - and exchanges it with the given backColor
		//	GDI_RGBForeColor (foreColor.GetOSRep ());		// See IM V-61- docs on "The Hilite Mode".
			Rect	qdHiliteRect = AsQDRect (hilightRect);
			::InvertRect (&qdHiliteRect);
		#elif	qWindows
			// Does proper inverse video, but seems to ignore the TextColor/BkColor/Pen/Brush colors.
			// Really should fix this to behave like Mac - replacing the background color with the text hilight color.
			// See SPR#???
			BitBlt (hilightRect.left, hilightRect.top, hilightRect.GetWidth (), hilightRect.GetHeight (),
								this, hilightRect.left, hilightRect.top, DSTINVERT
				);
		#elif	qXWindows
			/*
			 *	Quick and dirty primitive version. Should probably take into account backColor/foreColor args.
			 *			--	LGP 2001-04-30
			 */
			XGCValues	prevValues;
			const unsigned long	kSavedAttrs	=	GCFunction | GCForeground | GCBackground;
			(void)::memset (&prevValues, 0, sizeof (prevValues));
			::XGetGCValues (fDisplay, fGC, kSavedAttrs, &prevValues);
			::XSetFunction (fDisplay, fGC, GXxor);
			long	whiteP	=	WhitePixel (fDisplay, DefaultScreen (fDisplay));
			long	blackP	=	BlackPixel (fDisplay, DefaultScreen (fDisplay)) ^ whiteP;
			::XSetBackground (fDisplay, fGC, whiteP);
			::XSetForeground (fDisplay, fGC, blackP);
			Led_Rect	adjustedRect	=	hilightRect - fDrawableOrigin;
			::XFillRectangle (fDisplay, fDrawable, fGC, adjustedRect.GetLeft (), adjustedRect.GetTop (), adjustedRect.GetWidth (), adjustedRect.GetHeight ());
			::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);
		#endif
	}
}
#endif

/*
@METHOD:		Led_Tablet_::HilightArea_SolidHelper
@DESCRIPTION:	<p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which override
			@'TextImager::HilightArea'.</p>
				<p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
			platform UI conventionally calls for) inverting the	text via a simple XOR.</p>
				<p>Note also that as of Led 3.1b4, there is new code to properly respect the hilight fore/back colors. This code
			should generally be fast enough, but may - on some hardware - NOT be fast enough. By setting the text color to black,
			the background color to while, and the hilight colors the reverse of this (fore=black/back=white), this code will revert
			to the old algorithm, and run much faster.</p>
*/
void	Led_Tablet_::HilightArea_SolidHelper (const Led_Rect& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
{
	if (not hilightArea.IsEmpty ()) {
		#if		qMacOS
			Led_Arg_Unused (hilightBackColor);
			Led_Arg_Unused (hilightForeColor);
			Led_Arg_Unused (oldForeColor);
			SetPort ();
			LMSetHiliteMode (LMGetHiliteMode () & 0x7F);
			GDI_RGBBackColor (oldBackColor.GetOSRep ());	// Mac HilightMode code already knows the hilightBackColor - and exchanges it with the given backColor
		//	GDI_RGBForeColor (foreColor.GetOSRep ());		// See IM V-61- docs on "The Hilite Mode".
			Rect	qdHiliteRect = AsQDRect (hilightArea);
			::InvertRect (&qdHiliteRect);
		#elif	qWindows
			/*
			 *	SPR#1271 - major reworking using DIB sections etc, to get much better display of hilighted text.
			 */
			if (hilightBackColor.GetOSRep () == Led_Color::kBlack.GetOSRep () and
				hilightForeColor.GetOSRep () == Led_Color::kWhite.GetOSRep () and
				oldBackColor.GetOSRep () == Led_Color::kWhite.GetOSRep () and
				oldForeColor.GetOSRep () == Led_Color::kBlack.GetOSRep ()
				) {
				// This is much faster (on some/most hardware) than the RecolorHelper algorithms. For this special case of of B&W fore/back/hilight
				// colors - this code is MUCH faster as well. So - for people for whom the default algorithm is too slow - they can just specify
				// these colors for hilight and back/fore-color, and they'll get the faster hilight.
				// See SPR#1271
				BitBlt (hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
									this, hilightArea.left, hilightArea.top, DSTINVERT
					);
			}
			else {
#if 1
				fRecolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (fRecolorHelper,
						m_hDC, Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
						hilightBackColor, hilightForeColor, oldBackColor, oldForeColor
					);
				fRecolorHelper->DoRecolor (hilightArea);
#else
				static	RecolorHelper*	recolorHelper	=	NULL;
				recolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (recolorHelper,
						m_hDC, Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
						hilightBackColor, hilightForeColor, oldBackColor, oldForeColor
					);
				recolorHelper->DoRecolor (hilightArea);
#endif
			}
		#elif	qXWindows
			/*
			 *	Quick and dirty primitive version. Should probably take into account backColor/foreColor args.
			 *			--	LGP 2001-04-30
			 */
			XGCValues	prevValues;
			const unsigned long	kSavedAttrs	=	GCFunction | GCForeground | GCBackground;
			(void)::memset (&prevValues, 0, sizeof (prevValues));
			::XGetGCValues (fDisplay, fGC, kSavedAttrs, &prevValues);
			::XSetFunction (fDisplay, fGC, GXxor);
			long	whiteP	=	WhitePixel (fDisplay, DefaultScreen (fDisplay));
			long	blackP	=	BlackPixel (fDisplay, DefaultScreen (fDisplay)) ^ whiteP;
			::XSetBackground (fDisplay, fGC, whiteP);
			::XSetForeground (fDisplay, fGC, blackP);
			Led_Rect	adjustedRect	=	hilightArea - fDrawableOrigin;
			::XFillRectangle (fDisplay, fDrawable, fGC, adjustedRect.GetLeft (), adjustedRect.GetTop (), adjustedRect.GetWidth (), adjustedRect.GetHeight ());
			::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);
		#endif
	}
}

/*
@METHOD:		Led_Tablet_::HilightArea_SolidHelper
@DESCRIPTION:	<p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which override
			@'TextImager::HilightArea'.</p>
				<p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
			platform UI conventionally calls for) inverting the	text via a simple XOR.</p>
*/
void	Led_Tablet_::HilightArea_SolidHelper (const Led_Region& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
{
	if (not hilightArea.IsEmpty ()) {
		#if		qMacOS
			Led_Arg_Unused (hilightBackColor);
			Led_Arg_Unused (hilightForeColor);
			Led_Arg_Unused (oldForeColor);
			SetPort ();
			LMSetHiliteMode (LMGetHiliteMode () & 0x7F);
			GDI_RGBBackColor (oldBackColor.GetOSRep ());	// Mac HilightMode code already knows the hilightBackColor - and exchanges it with the given backColor
		//	GDI_RGBForeColor (foreColor.GetOSRep ());		// See IM V-61- docs on "The Hilite Mode".
			::InvertRgn (hilightArea.GetOSRep ());
		#elif	qWindows
			Led_Assert (false);	// probably not hard - bit not totally obvious how todo and since not called yet - ignore for now... LGP 2002-12-03
		#elif	qXWindows
			Led_Assert (false);	 // I have no XWin region implementation yet... LGP 2002-12-03
		#endif
	}
}


/*
@METHOD:		Led_Tablet_::GetFontMetrics
@DESCRIPTION:	<p>Retrieve the (@'Led_FontMetrics') associated with the current tablet (based on the last SetFont call).</p>
*/
Led_FontMetrics	Led_Tablet_::GetFontMetrics () const
{
	#if		qMacOS
		FontInfo	fontInfo;
		::GetFontInfo (&fontInfo);
		return (fontInfo);
	#elif	qWindows
		Led_RequireNotNil (m_hAttribDC);
		TEXTMETRIC	tms;
		Led_Verify (::GetTextMetrics (m_hAttribDC, &tms) != 0);
		return tms;
	#elif	qXWindows
		Led_FontMetrics::PlatformSpecific	result;
		memset (&result, 0, sizeof (result));
		Led_ThrowIfNull (fCachedFontInfo);
		result.fAscent = fCachedFontInfo->ascent;
		result.fDescent = fCachedFontInfo->descent;
		result.fLeading = 0;	// NOT SURE WHAT THIS IS in X-terminology. Maybe just not supported in XFonts? - LGP 2001-05-07
		result.fMaxCharWidth = fCachedFontInfo->max_bounds.width;
		return result;
	#endif
}

#if		qXWindows
void	Led_Tablet_::SetFont (const Led_FontSpecification& fontSpec)
{
	/*
	 * First, see if the XFontStruct* is already cached. If so - all we need todo is (maybe) an XSetFont call.
	 */
	{
		map<string,XFontStruct*>::const_iterator	i	=	fFontCache.find (fontSpec.GetOSRep ());
		if (i != fFontCache.end ()) {
			XFontStruct*	newFontStruct	=	i->second;
			if (newFontStruct != fCachedFontInfo) {
				fCachedFontInfo = i->second;
				Led_AssertNotNil (fCachedFontInfo);
				::XSetFont (fDisplay, fGC, fCachedFontInfo->fid);
			}
			return;
		}
		/*
		 *	If a cache miss, then assure cache not too big.
		 */
		if (fFontCache.size () >= kMaxFontCacheSize) {
			// remove a random elt
			::XFreeFont (fDisplay, fFontCache.begin ()->second);
			fFontCache.erase (fFontCache.begin ());
		}
	}

	/*
	 *	The font is not already cached. We must try to find it (maybe finding the name in the 
	 *	fFontMappingCache cache, maybe not.
	 */
	fCachedFontInfo = NULL;
	fCachedFontInfo	=	::XLoadQueryFont (fDisplay, fontSpec.GetOSRep ().c_str ());
	if (fCachedFontInfo == NULL) {
		/*
		 *	Look and see if the font is in the cache.
		 */
		map<string,string>::const_iterator	i	=	fFontMappingCache.find (fontSpec.GetOSRep ());

		string	useFontName;
		if (i != fFontMappingCache.end ()) {
			useFontName = i->second;
			Led_Assert (not useFontName.empty ());
		}
		else {
			// try font-matching algorithm...
			char	pointSize[1024];
			(void)::sprintf (pointSize, "%d", fontSpec.GetPointSize () * 10);
			const	string	kMatchAny	=	"*";
			string	tryFontRep	=	fontSpec.mkOSRep (kMatchAny, fontSpec.GetFontNameSpecifier (), kMatchAny, kMatchAny, kMatchAny);
			int		nFonts		=	0;
			char**	fontList	=	::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
			#if		qDebugFontDetails
				bool	nameMatchFailure	=	false;
			#endif
			if (fontList == NULL) {
				#if		qDebugFontDetails
					nameMatchFailure	=	true;
				#endif
				// Try a few name mappings/aliases (apx equal fonts - generalize this!!!)
				if (fontSpec.GetFontNameSpecifier () == "Times New Roman") {
					tryFontRep	=	fontSpec.mkOSRep (kMatchAny, "times", kMatchAny, kMatchAny, kMatchAny);
					fontList	=	::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
				}
			}
			if (fontList == NULL) {
				#if		qDebugFontDetails
					nameMatchFailure	=	true;
				#endif
				tryFontRep	=	fontSpec.mkOSRep (kMatchAny, kMatchAny, kMatchAny, kMatchAny, kMatchAny);
				fontList	=	::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
			}
			Led_ThrowIfNull (fontList);
			vector<string>	vFontList;
			{
				vFontList.reserve (nFonts);
				for (size_t i = 0; i < nFonts; ++i) {
					vFontList.push_back (fontList[i]);
				}
			}
			::XFreeFontNames (fontList); fontList = NULL;
			string	bestMatchingName	=	BestMatchFont (fontSpec, vFontList);
			#if		qDebugFontDetails
				if (nameMatchFailure) {
					fprintf (stderr, "Couldn't find fontName '%s'- using BestMatchSpec = '%s'\r\n", fontSpec.GetFontNameSpecifier ().c_str (), bestMatchingName.c_str ());
				}
			#endif
			useFontName = bestMatchingName;
			Led_Assert (not useFontName.empty ());
			#if		qDebugFontDetails
				fprintf (stderr, "Adding mapping to  fFontMappingCache: '%s'- ==> '%s'\r\n", fontSpec.GetOSRep ().c_str (), useFontName.c_str ());
			#endif
			fFontMappingCache.insert (map<string,string>::value_type (fontSpec.GetOSRep (), useFontName));
		}
		fCachedFontInfo	=	::XLoadQueryFont (fDisplay, useFontName.c_str ());
		Led_ThrowIfNull (fCachedFontInfo);
	}
	fFontCache.insert (map<string,XFontStruct*>::value_type (fontSpec.GetOSRep (), fCachedFontInfo));
	Led_AssertNotNil (fCachedFontInfo);
	::XSetFont (fDisplay, fGC, fCachedFontInfo->fid);
}

void	Led_Tablet_::SetDrawableOrigin (const Led_Point& origin)
{
	fDrawableOrigin = origin;
}
#endif

#if		qXWindows
	static	bool	FontNamesEqual (const string& lhs, const string& rhs)
		{
			if (lhs.length () != rhs.length ()) {
				return false;
			}
			for (size_t i = 0; i < lhs.length (); ++i) {
				if (lhs[i] != rhs[i]) {
					return false;
				}
			}
			return true;
		}
Led_SDK_String	Led_Tablet_::BestMatchFont (const Led_FontSpecification& fsp, const vector<Led_SDK_String>& fontsList)
{
	Led_SDK_String	bestAnswer;
	float			bestScore		=	0.0f;
	Led_SDK_String	fspName			=	fsp.GetFontName ();
	int				fspPointSize	=	fsp.GetPointSize ();
	Led_SDK_String	fspWeight		=	fsp.GetStyle_Bold ()? "bold": "medium";
	Led_SDK_String	fspItalics		=	fsp.GetStyle_Italic ()? "i": "r";
	const			string	kMatchAny	=	"*";
	for (vector<string>::const_iterator i = fontsList.begin (); i != fontsList.end (); ++i) {
		Led_SDK_String	name;
		Led_SDK_String	size;
		Led_SDK_String	weight;
		Led_SDK_String	slant;
		ParseFontName (*i, &name, &size, &weight, &slant);
		bool	rightFontName	= (FontNamesEqual (fspName, name));

		float	thisScore	=	1;
		if (rightFontName) {
			thisScore += 10;
		}
		int	thisPointSize	=	0;
		if (::sscanf (size.c_str (), "%d", &thisPointSize) == 1) {
			int		pointSizeDiff	=	abs (thisPointSize - (fspPointSize * 10));
			float	scoreAdj		=	(100.0f - (pointSizeDiff/10.0f))/10.0f;
			scoreAdj = Led_Max (0.0f, scoreAdj);
			thisScore += scoreAdj;
		}
		if (weight == fspWeight) {
			thisScore += 5.0f;
		}
		if (slant == fspItalics) {
			thisScore += 4.0f;
		}

		if (thisScore > bestScore) {
			bestScore = thisScore;
			bestAnswer = Led_FontSpecification::mkOSRep (kMatchAny, name, weight, slant, size);
		}
	}
	return bestAnswer;
}

int	Led_Tablet_::IngoreXErrorHandler (Display* /*display*/, XErrorEvent* /*error*/)
{
	return 0;
}

void	Led_Tablet_::ParseFontName (const Led_SDK_String& fontName, Led_SDK_String* familyName, Led_SDK_String* fontSize, Led_SDK_String* fontWeight, Led_SDK_String* fontSlant)
{
	Led_RequireNotNil (familyName);
	Led_RequireNotNil (fontSize);
	Led_RequireNotNil (fontWeight);

	Led_SDK_String	foundry;
	Led_SDK_String	family;
	Led_SDK_String	weight;
	Led_SDK_String	slant;
	Led_SDK_String	setwidth;
	Led_SDK_String	pixels;
	Led_SDK_String	points;
	Led_SDK_String	hRes;
	Led_SDK_String	vRes;
	Led_SDK_String	spacing;
	Led_SDK_String	aveWidth;
	Led_SDK_String	charset;

	size_t	start	=	1;
	size_t	end		=	fontName.find ('-', start);
	foundry = fontName.substr (start, end-start);
	
	start = end + 1;
	end = 	fontName.find ('-', start);
	family = fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	weight = fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	slant = fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	setwidth = fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	string	ignored	=	fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	pixels	=	fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	points	=	fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	hRes	=	fontName.substr (start, end-start);

	start = end + 1;
	end = 	fontName.find ('-', start);
	vRes	=	fontName.substr (start, end-start);

	*familyName = family;
	*fontSize = points;
	*fontWeight = weight;
	*fontSlant = slant;
}
#endif






/*
 ********************************************************************************
 ***************************** OffscreenTablet::OT ******************************
 ********************************************************************************
 */
#if		qMacOS
OffscreenTablet::OT::OT (GrafPtr gp):
	inherited (gp)
{
}
#elif	qWindows
OffscreenTablet::OT::OT (HDC hdc, Led_Tablet_::OwnDCControl ownsDC):
	inherited (hdc, ownsDC)
{
}
#elif	qXWindows
OffscreenTablet::OT::OT (Display* display, Drawable drawable):
	inherited (display, drawable)
{
}
#endif






/*
 ********************************************************************************
 ********************************* OffscreenTablet ******************************
 ********************************************************************************
 */
OffscreenTablet::OffscreenTablet ():
	fOrigTablet (NULL),
	fOffscreenRect (Led_Rect (0, 0, 0, 0)),
	fOffscreenTablet (NULL)
	#if		qMacOS
		,fOrigDevice (NULL),
		fOrigPort (NULL),
		fOffscreenGWorld (NULL)
	#elif	qWindows
		,fMemDC (),
		fMemoryBitmap (),
		fOldBitmapInDC (NULL)
	#elif	qXWindows
		,fPixmap (0)
	#endif
{
}

OffscreenTablet::~OffscreenTablet ()
{
	#if		qMacOS
		if (fOrigPort != NULL) {
			::SetGWorld (fOrigPort, fOrigDevice);	// restore gworld
		}
		if (fOffscreenGWorld != NULL) {
			::DisposeGWorld (fOffscreenGWorld);
		}
		delete fOffscreenTablet;
	#elif	qWindows
		if (fOldBitmapInDC != NULL) {
			(void)fMemDC.SelectObject (fOldBitmapInDC);
		}
	#elif	qXWindows
		if (fPixmap != 0) {
			::XFreePixmap (fOrigTablet->fDisplay, fPixmap);
		}
	#endif
}

/*
@METHOD:		OffscreenTablet::Setup
@DESCRIPTION:	<p>Prepare a new offscreen drawing environment given the starting basis 'originalTablet' (typically from a window).</p>
				<p>Later call @'OffscreenTablet::PrepareRect' before any actual drawing can be done. This should be called once before
			calling @'OffscreenTablet::PrepareRect'.</p>
*/
void	OffscreenTablet::Setup (Led_Tablet origTablet)
{
	Led_Require (fOrigTablet == NULL);	// can only call once.
	Led_RequireNotNil (origTablet);

	fOrigTablet = origTablet;
	#if		qMacOS
		// Save the old gworld info
		Led_Assert (fOrigPort == NULL);
		Led_Assert (fOrigDevice == NULL);
		::GetGWorld (&fOrigPort, &fOrigDevice);

		// Create our gworld (may have to cache this if it turns out to be expensive to re-create...
		Led_Assert (fOffscreenGWorld == NULL);
		{
			Rect	bounds	=	AsQDRect (Led_Rect (0, 0, 1, 1));	// size appropriately on a row-by-row basis below...
			OSErr	theErr	=	SafeNewGWorld (&fOffscreenGWorld, 0, &bounds, NULL, NULL, noNewDevice | useTempMem);
			if (theErr != noErr) {
				fOffscreenGWorld = NULL;	// no biggie, we just don't use it...
			}
		}
		if (fOffscreenGWorld != NULL) {
			fOffscreenTablet = new OT (reinterpret_cast<GrafPtr> (fOffscreenGWorld));
		}
	#elif	qWindows
		if (fMemDC.CreateCompatibleDC (fOrigTablet)) {
			fOffscreenTablet = &fMemDC;
		}
	#elif	qXWindows
		Led_Assert (fPixmap == 0);
		// Nothing todo yet - create the pixmap when we know the RowRect.
	#endif
}

/*
@METHOD:		OffscreenTablet::PrepareRect
@DESCRIPTION:	<p>Prepare the offscreen drawing environment for the given 'currentRowRect'. This can only be safely called
			after the call to @'OffscreenTablet::Setup' - but can be called multiple times. Note that calls to this
			will typically 'destroy' the bits in the offscreen tablet.</p>
*/
Led_Tablet	OffscreenTablet::PrepareRect (const Led_Rect& currentRowRect, Led_Distance extraToAddToBottomOfRect)
{
	Led_Tablet	result	=	fOrigTablet;
	#if		qMacOS
		if (fOffscreenTablet != NULL) {
			fOffscreenRect	=	currentRowRect;
			fOffscreenRect.bottom += extraToAddToBottomOfRect;
			Rect	bounds	=	AsQDRect (fOffscreenRect);
			::OffsetRect (&bounds, -bounds.left, -bounds.top);
			#if		TARGET_CARBON
				Led_Size	curOffscreenGWorldSize;
				{
					Rect	junk;
					curOffscreenGWorldSize = AsLedSize (GetRectSize (*::GetPixBounds (::GetPortPixMap (fOffscreenGWorld), &junk)));
				}
			#else
				Led_Size	curOffscreenGWorldSize	=	AsLedSize (GetRectSize ((*fOffscreenGWorld->portPixMap)->bounds));
			#endif
			if ((fOffscreenRect.GetSize () == curOffscreenGWorldSize) or
				SafeUpdateGWorld (&fOffscreenGWorld, 0, &bounds, NULL, NULL, 0) >= 0
				) {
				Led_AssertNotNil (::GetGWorldPixMap (fOffscreenGWorld));
				if (::LockPixels (::GetGWorldPixMap (fOffscreenGWorld))) {
					// UpdateGWorld () can change grafPortPTR!
					delete fOffscreenTablet;
					fOffscreenTablet = new OT (reinterpret_cast<GrafPtr> (fOffscreenGWorld));
					result = fOffscreenTablet;
					::SetGWorld (fOffscreenGWorld, NULL);
					::SetOrigin (fOffscreenRect.left, fOffscreenRect.top);
					goto good;
				}
			}
			bad:
				::SetGWorld (fOrigPort, fOrigDevice);	// restore gworld
				if (fOffscreenGWorld != NULL) {
					::DisposeGWorld (fOffscreenGWorld);
					fOffscreenGWorld = NULL;
				}
				delete fOffscreenTablet;
				fOffscreenTablet = NULL;
			good:
				;
		}
	#elif	qWindows
		if (fOffscreenTablet != NULL) {
			fOffscreenRect	=	currentRowRect;
			fOffscreenRect.bottom += extraToAddToBottomOfRect;
			// See if we need to re-allocate the bitmap
			if (fMemoryBitmap == NULL or 
				(fOffscreenRect.GetSize () != fMemoryBitmap.GetImageSize())
				) {
				// deselect our new memory bitmap before changing its size - not sure needed, but lets be paranoid - 
				// this is Windows after all ... LGP 960513
				if (fOldBitmapInDC != NULL) {
					(void)fMemDC.SelectObject (fOldBitmapInDC);
				}
				fMemoryBitmap.DeleteObject ();	// lose previous contents, if any...
				#if		qUseDIBSectionForOffscreenBitmap
					if (fMemoryBitmap.CreateCompatibleDIBSection (Led_Tablet (fOrigTablet)->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
						fOffscreenTablet = NULL;	// OK, just don't use...
					}
				#else
					if (fMemoryBitmap.CreateCompatibleBitmap (Led_Tablet (fOrigTablet)->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
						fOffscreenTablet = NULL;	// OK, just don't use...
					}
				#endif
				if (fOffscreenTablet != NULL) {
					fOldBitmapInDC = fMemDC.SelectObject (fMemoryBitmap);
					if (fOldBitmapInDC == NULL) {
						fOffscreenTablet = NULL;	// OK, just don't use...
					}
				}
			}
			if (fOffscreenTablet != NULL) {
				result = fOffscreenTablet;	// Draw into offscreen bitmap
			}
			if (fOffscreenTablet != NULL) {
				fMemDC.SetWindowOrg (fOffscreenRect.left, fOffscreenRect.top);
			}
		}
	#elif	qXWindows
		Led_Size	pixmapSize	=	fOffscreenRect.GetSize ();
		fOffscreenRect	=	currentRowRect;
		fOffscreenRect.bottom += extraToAddToBottomOfRect;
		if (fPixmap == 0 or pixmapSize != fOffscreenRect.GetSize ()) {
			// Destroy old pixmap, and create new one
			delete fOffscreenTablet;
			fOffscreenTablet = NULL;
			if (fPixmap != 0) {
				::XFreePixmap (fOrigTablet->fDisplay, fPixmap);
				fPixmap = 0;
			}
			unsigned	int	depth	=	1;	// default - cuz should always be supported
			{
				// Try to get it from the drawable. Only works (I believe) if the drawable is a window.
				XWindowAttributes	winAttrs;
				(void)::memset (&winAttrs, 0, sizeof (winAttrs));
				/*
				 *	Since we don't know for sure the drawable is a window - catch the error and ignore it. Don't let
				 *	XErrorHandler do anything bad.
				 */
				int	(*oldErrHandler) (Display*, XErrorEvent*)	=	::XSetErrorHandler (Led_Tablet_::IngoreXErrorHandler);
				Status	s	=	::XGetWindowAttributes (fOrigTablet->fDisplay, fOrigTablet->fDrawable, &winAttrs);
				::XSetErrorHandler (oldErrHandler);
				if (s == 0) {
					// if call failed - no biggie. Just pick the DefaultDepthOfScreen (could have used XListDepths ()?).
					depth = ::XDefaultDepthOfScreen (::XScreenOfDisplay (fOrigTablet->fDisplay, DefaultScreen (fOrigTablet->fDisplay)));
				}
				else {
					depth = winAttrs.depth;
				}
			}
			fPixmap = ::XCreatePixmap (fOrigTablet->fDisplay, fOrigTablet->fDrawable,
										fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (), depth
									);
			Led_Assert (fPixmap != 0);
			try {
				fOffscreenTablet = new OT (fOrigTablet->fDisplay, fPixmap);
				fOffscreenTablet->fColormap = fOrigTablet->fColormap;
				fOffscreenTablet->fFontMappingCache = fOrigTablet->fFontMappingCache;
			}
			catch (...) {
				delete fOffscreenTablet;
				fOffscreenTablet = NULL;
				throw;
			}
		}
		if (fOffscreenTablet != NULL) {
			fOffscreenTablet->SetDrawableOrigin (fOffscreenRect.GetTopLeft ());
			result = fOffscreenTablet;	// Draw into offscreen bitmap
		}
	#endif
	return result;
}

/*
@METHOD:		OffscreenTablet::BlastBitmapToOrigTablet
@DESCRIPTION:	<p>Copy the bits which have been saved away into this offscreen tablet back to the original tablet specified in
			@'OffscreenTablet::Setup' and to coordinates specified in the last call to @'OffscreenTablet::PrepareRect'.</p>
*/
void	OffscreenTablet::BlastBitmapToOrigTablet ()
{
	if (fOffscreenTablet != NULL) {
		#if		qMacOS
			Rect	bounds	=	AsQDRect (fOffscreenRect);
			::SetGWorld (fOrigPort, fOrigDevice);	// restore gworld
			GDI_RGBForeColor (Led_Color::kBlack.GetOSRep ());
			GDI_RGBBackColor (Led_Color::kWhite.GetOSRep ());
			GrafPtr	tabletGrafPort	=	*fOffscreenTablet;
			#if		TARGET_CARBON
				{
					Rect	tmp;
					::CopyBits (::GetPortBitMapForCopyBits (tabletGrafPort), ::GetPortBitMapForCopyBits (fOrigPort), ::GetPortBounds (tabletGrafPort, &tmp), &bounds, srcCopy, NULL);
				}
			#else
				::CopyBits (&tabletGrafPort->portBits, &((GrafPtr)fOrigPort)->portBits, &tabletGrafPort->portRect, &bounds, srcCopy, NULL);
			#endif
			::UnlockPixels (::GetGWorldPixMap (fOffscreenGWorld));
		#elif	qWindows
			Led_Tablet	screenDC	=	fOrigTablet;
			screenDC->BitBlt (fOffscreenRect.left, fOffscreenRect.top, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (),
								fOffscreenTablet, fOffscreenRect.left, fOffscreenRect.top, SRCCOPY
								);
		#elif	qXWindows
			Led_Assert (fPixmap != 0);
			::XCopyArea (fOrigTablet->fDisplay, fOffscreenTablet->fDrawable, fOrigTablet->fDrawable, fOrigTablet->fGC,
						0, 0,
						fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (),
						(int)fOffscreenRect.GetLeft (), (int)fOffscreenRect.GetTop ()
					);
		#endif
	}
}











/*
 ********************************************************************************
 ********************************* Led_InstalledFonts ***************************
 ********************************************************************************
 */
Led_InstalledFonts::Led_InstalledFonts (
											#if		qXWindows
												Display* display,
											#endif
											FilterOptions filterOptions
										):
	fFilterOptions (filterOptions),
	fFontNames ()
{
#if		qWindows
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = DEFAULT_CHARSET;
	Led_WindowDC	screenDC (NULL);
	::EnumFontFamiliesEx (screenDC.m_hDC, &lf, (FONTENUMPROC)FontFamilyAdderProc, (long)this, 0);
	sort (fFontNames.begin (), fFontNames.end ());
	vector<Led_SDK_String>::iterator rest = unique (fFontNames.begin (), fFontNames.end ());
	fFontNames.erase (rest, fFontNames.end ());	// remove the duplicates
#elif	qXWindows
	int fontListSize= 0;
	char** fontList = ::XListFonts (display, "*", 200000, &fontListSize);
	set<string>	fontNames;
	for (int i = 0; i < fontListSize; ++i) {
		string	longFontName	=	fontList[i];
		string	tmp				=	longFontName;
		if (tmp.length () > 0 and tmp[0] == '-') {
			size_t	nextDash	=	tmp.find ('-', 1);
			if (nextDash != string::npos and nextDash > 1) {
				tmp = tmp.substr (nextDash + 1);
			}
			nextDash	=	tmp.find ('-');	// OK - even if end of string
			string	fontFamilyName	=	tmp.substr (0, nextDash);
			if (not fontFamilyName.empty ()) {
				fontNames.insert (fontFamilyName);
			}
		}
	}
	::XFreeFontNames (fontList); fontList = NULL;
	fFontNames = vector<string> (fontNames.begin (), fontNames.end ());
#else
	Led_Assert (false);		// NYI for other platforms
#endif
}

#if		qWindows
BOOL	FAR	PASCAL	Led_InstalledFonts::FontFamilyAdderProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int fontType, LPVOID pThis)
{
	Led_InstalledFonts*	thisP	=	reinterpret_cast<Led_InstalledFonts*> (pThis);
	
	if (thisP->fFilterOptions & eSkipRasterFonts) {
		// don't put in non-printer raster fonts (cuz WordPad doesn't and CFontDialog doesn't appear to -
		// LGP 971215)
		if (fontType & RASTER_FONTTYPE)
			return 1;
	}
	if (thisP->fFilterOptions & eSkipAtSignFonts) {
		if (pelf->elfLogFont.lfFaceName[0] == '@')
			return 1;
	}
	thisP->fFontNames.push_back (pelf->elfLogFont.lfFaceName);
	return 1;
}
#endif








/*
 ********************************************************************************
 ********************************* Led_GDIGlobals *******************************
 ********************************************************************************
 */

Led_GDIGlobals*	Led_GDIGlobals::sThe	=	NULL;

// Somewhat silly hack so Led_GDIGlobals gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class	Led_GDIGlobals::_Global_DESTRUCTOR_ {
	public:
		~_Global_DESTRUCTOR_ ()
			{
				delete (Led_GDIGlobals::sThe);
				Led_GDIGlobals::sThe = NULL;
			}
}	sTheLed_GDIGlobalsDESTRUCTOR_;

Led_GDIGlobals::Led_GDIGlobals ():
	fLogPixelsH (0),
	fLogPixelsV (0)
{
	InvalidateGlobals ();
}

void	Led_GDIGlobals::InvalidateGlobals ()
{
	// From the name, it would appear we invalidated, and re-validate later. But I think this implematnion is a bit
	// simpler, and should perform fine given its expected usage.
	#if		qMacOS
		fLogPixelsH = 72;
		fLogPixelsV = 72;
	#elif	qWindows
		Led_WindowDC	screenDC (NULL);
		fLogPixelsH = ::GetDeviceCaps (screenDC, LOGPIXELSX);
		fLogPixelsV = ::GetDeviceCaps (screenDC, LOGPIXELSY);
	#elif	qXWindows
		/*
		 *	Either 75 or 100??? Not sure which is best
		 *
		 *	AbiWord has comments (in gr_UnixGraphix.cpp) that though most X-Servers return a resolution of 75, 100 seems to
		 *	look best. I tried on XWinPro 5.1 (a Win32-based X-Server) and on the one that comes with RedHat Linux 6.1, and both
		 *	looked better when I set this to 100. So try that for now...
		 */
		//const	int	kResToUse	=	75;
		const	int	kResToUse	=	100;
		fLogPixelsH = kResToUse;
		fLogPixelsV = kResToUse;
	#endif
}







/*
 ********************************************************************************
 ************************************ AddRectangleToRegion **********************
 ********************************************************************************
 */
void	AddRectangleToRegion (Led_Rect addRect, Led_Region* toRgn)
{
	Led_RequireNotNil (toRgn);
	*toRgn = *toRgn + Led_Region (addRect);
}







/*
 ********************************************************************************
 ********************************* Led_GetDIBImageSize **************************
 ********************************************************************************
 */

/*
@METHOD:		Led_GetDIBImageSize
@DESCRIPTION:	<p>Return the size in pixels of the given argument DIB</p>
*/
Led_Size	Led_GetDIBImageSize (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	Led_Assert (sizeof (BITMAPINFOHEADER) == 40);	// just to make sure we have these defined right on other platforms
	Led_Assert (sizeof (BITMAPCOREHEADER) == 12);	// ''
	Led_Assert (sizeof (RGBTRIPLE) == 3);			// ''

	if (IS_WIN30_DIB (dib)) {
		const BITMAPINFOHEADER&	hdr	=	dib->bmiHeader;
		return (Led_Size (Led_Abs (Led_ByteSwapFromWindows (hdr.biHeight)), Led_Abs (Led_ByteSwapFromWindows (hdr.biWidth))));
	}
	else {
		const BITMAPCOREHEADER&	hdr	=	*(reinterpret_cast<const BITMAPCOREHEADER*> (dib));
		return (Led_Size (Led_ByteSwapFromWindows (hdr.bcHeight), Led_ByteSwapFromWindows (hdr.bcWidth)));
	}
}





/*
 ********************************************************************************
 *************************** Led_GetDIBPalletByteCount **************************
 ********************************************************************************
 */
size_t		Led_GetDIBPalletByteCount (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	/*
	 *	Logic from MSFT DibLook sample in MSVC.Net 2003, plus:
	 *		MSVC.Net 2003 SDK docs mention this case - that:
	 *			BI_BITFIELDS:	Specifies that the bitmap is not compressed and that the
	 *							color table consists of three DWORD color masks that specify
	 *							the red, green, and blue components, respectively, of each pixel.
	 *							This is valid when used with 16- and 32-bpp bitmaps
	 */
	if (IS_WIN30_DIB (dib)) {
		size_t					byteCount	=	DIBNumColors (dib) * sizeof (RGBQUAD);
		const BITMAPINFOHEADER&	hdr			=	dib->bmiHeader;
		unsigned short			bitCount	=	Led_ByteSwapFromWindows (hdr.biBitCount);
		if (Led_ByteSwapFromWindows (hdr.biCompression) == BI_BITFIELDS) {
			#if		qWindows
				Led_Assert (sizeof (DWORD) == sizeof (unsigned int));
			#endif
			Led_Assert (4 == sizeof (unsigned int));
			byteCount += 3 * sizeof (unsigned int);
		}
		return (byteCount);
	}
	else {
		Led_Assert (sizeof (RGBTRIPLE) == 3);		// make sure we have this defined right on each platform
		return (DIBNumColors (dib) * sizeof (RGBTRIPLE));
	}
}






/*
 ********************************************************************************
 ************************* Led_GetDIBImageRowByteCount **************************
 ********************************************************************************
 */
/*
@METHOD:		Led_GetDIBImageRowByteCount
@DESCRIPTION:	<p>Return the size in bytes of a single ROW of pixels in the given argument DIB.</p>
*/
size_t		Led_GetDIBImageRowByteCount (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	Led_Size				imageSize	=	Led_GetDIBImageSize (dib);
	const BITMAPINFOHEADER&	hdr			=	dib->bmiHeader;

	unsigned short	bitCount	=	Led_ByteSwapFromWindows (hdr.biBitCount);
	return (((imageSize.h * bitCount + 31) & ~31) >> 3);
}





/*
 ********************************************************************************
 *************************** Led_GetDIBImageByteCount ***************************
 ********************************************************************************
 */
/*
@METHOD:		Led_GetDIBImageByteCount
@DESCRIPTION:	<p>Return the size in bytes of the given argument DIB. DIBs are contiguous chunks of RAM.</p>
*/
size_t		Led_GetDIBImageByteCount (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	Led_Size				imageSize	=	Led_GetDIBImageSize (dib);
	const BITMAPINFOHEADER&	hdr			=	dib->bmiHeader;
	size_t					byteCount	=	Led_ByteSwapFromWindows (hdr.biSize);

	byteCount += Led_GetDIBPalletByteCount (dib);

	unsigned long	imageByteSize	=	Led_ByteSwapFromWindows (hdr.biSizeImage);
	if (imageByteSize == 0) {
		unsigned short	bitCount	=	Led_ByteSwapFromWindows (hdr.biBitCount);
		// often zero for uncompressed images, so we compute ourselves...
		//imageByteSize = imageSize.v * ((imageSize.h * bitCount + 31)/32)*4;
		imageByteSize = imageSize.v * (((imageSize.h * bitCount + 31) & ~31) >> 3);
	}
	byteCount += imageByteSize;
	return byteCount;
}




/*
 ********************************************************************************
 ********************************* Led_CloneDIB *********************************
 ********************************************************************************
 */
/*
@METHOD:		Led_CloneDIB
@DESCRIPTION:	<p>Make a copy of the given @'Led_DIB' object using ::operator new (). Just use normal C++ ::operator delete ()
			to destroy the result.</p>
*/
Led_DIB*	Led_CloneDIB (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	size_t	nBytes	=	Led_GetDIBImageByteCount (dib);
	Led_DIB*	newDIB	=	reinterpret_cast<Led_DIB*> (new char [nBytes]);
	(void)::memcpy (newDIB, dib, nBytes);
	return newDIB;
}





/*
 ********************************************************************************
 ***************************** Led_GetDIBBitsPointer ****************************
 ********************************************************************************
 */
/*
@METHOD:		Led_GetDIBBitsPointer
@DESCRIPTION:	<p></p>
*/
const void*		Led_GetDIBBitsPointer (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);
	const BITMAPINFOHEADER&	hdr	=	dib->bmiHeader;
	return	reinterpret_cast<const char*> (dib) +
				Led_ByteSwapFromWindows (hdr.biSize) +
				Led_GetDIBPalletByteCount (dib);
}




#if		qWindows
/*
 ********************************************************************************
 ****************************** Led_DIBFromHBITMAP ******************************
 ********************************************************************************
 */
Led_DIB*	Led_DIBFromHBITMAP (HDC hDC, HBITMAP hbm)
{
	Led_RequireNotNil (hbm);
	BITMAP bm;
	Led_Verify (::GetObject (hbm, sizeof(BITMAP), (LPVOID)&bm));

	Led_DIB*	dibResult	=	NULL;
	{
		BITMAPINFOHEADER	bmiHdr;
		memset (&bmiHdr, 0, sizeof (bmiHdr));
		bmiHdr.biSize = sizeof(BITMAPINFOHEADER);
		bmiHdr.biWidth = bm.bmWidth;
		bmiHdr.biHeight = bm.bmHeight;
		bmiHdr.biPlanes = 1;
		bmiHdr.biBitCount = 24;
		bmiHdr.biCompression = BI_RGB;
		bmiHdr.biSizeImage = ((((bmiHdr.biWidth * bmiHdr.biBitCount) + 31) & ~31) >> 3) * bmiHdr.biHeight;
		size_t	nBytes	=	Led_GetDIBImageByteCount (reinterpret_cast<Led_DIB*> (&bmiHdr));
//		dibResult = reinterpret_cast<Led_DIB*> (::operator new (nBytes));
		dibResult = reinterpret_cast<Led_DIB*> (new char [nBytes]);
		Led_Assert (nBytes > sizeof (BITMAPINFOHEADER));
		(void)::memcpy (dibResult, &bmiHdr, sizeof (bmiHdr));
	}

	int	nScanLinesCopied = ::GetDIBits (hDC, hbm, 0, dibResult->bmiHeader.biHeight, reinterpret_cast<char*>(dibResult) + Led_GetDIBPalletByteCount (dibResult) + sizeof (BITMAPINFOHEADER), dibResult, DIB_RGB_COLORS);
	Led_Assert (nScanLinesCopied == dibResult->bmiHeader.biHeight);
	return dibResult;		
}
#endif







#if		qProvideIMESupport
#include	<ime.h>

/*
 ********************************************************************************
 ************************************** Led_IME *********************************
 ********************************************************************************
 */
Led_IME*	Led_IME::sThe	=	NULL;



#ifndef	qUseNewIMECode
#define	qUseNewIMECode	1
#endif


// Somewhat silly hack so Led_IME gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class	Led_IME::_Global_DESTRUCTOR_ {
	public:
		~_Global_DESTRUCTOR_ ()
			{
				delete (Led_IME::sThe);
				Led_IME::sThe = NULL;
			}
}	sTheLed_IME_DESTRUCTOR_;


Led_IME::Led_IME ():
	fSendIMEMessageProc (NULL),
	fIMEEnableProc (NULL),
	fImmGetContext (NULL),
	fImmSetCompositionFont (NULL),
	fImmReleaseContext (NULL),
	fImmGetCompositionStringW (NULL),
	fImmSetCompositionWindow (NULL),
	fImmSetOpenStatus (NULL),
	fWinNlsAvailable (false),
	fLastX (-1),
	fLastY (-1)	 	
{
	Led_Assert (sThe == NULL);
	sThe = this;

	#ifdef _UNICODE
		const	char	IMEPROCNAME[]	=    "SendIMEMessageExW";
	#else
		const	char	IMEPROCNAME[]	=    "SendIMEMessageExA";
	#endif
    HINSTANCE hNLS = ::GetModuleHandle (_T ("USER32.DLL"));
    if (hNLS != NULL) {
        fSendIMEMessageProc = (short (FAR PASCAL *)(HWND, DWORD))::GetProcAddress (hNLS, IMEPROCNAME);
		fIMEEnableProc = (BOOL (FAR PASCAL *)(HWND,BOOL))::GetProcAddress (hNLS, "WINNLSEnableIME"); 
    }
    fWinNlsAvailable = fSendIMEMessageProc != NULL and fIMEEnableProc != NULL;

    HINSTANCE hIMM = ::GetModuleHandle (_T ("IMM32.DLL"));
    if (hIMM != NULL) {
		#ifdef _UNICODE
			const	char	ImmSetCompositionFontNAME[]	=    "ImmSetCompositionFontW";
		#else
			const	char	ImmSetCompositionFontNAME[]	=    "ImmSetCompositionFontA";
		#endif
		fImmGetContext = (DWORD (FAR PASCAL *)(HWND))::GetProcAddress (hIMM, "ImmGetContext"); 
		fImmSetCompositionFont = (BOOL (FAR PASCAL *)(DWORD,const LOGFONT*))::GetProcAddress (hIMM, ImmSetCompositionFontNAME); 
		fImmReleaseContext = (BOOL (FAR PASCAL *)(HWND,DWORD))::GetProcAddress (hIMM, "ImmReleaseContext");
		fImmGetCompositionStringW = (LONG (FAR PASCAL *)(DWORD, DWORD, LPVOID, DWORD))::GetProcAddress (hIMM, "ImmGetCompositionStringW");
		fImmSetCompositionWindow = (BOOL (FAR PASCAL *)(DWORD,const void*))::GetProcAddress (hIMM, "ImmSetCompositionWindow"); 
		fImmSetOpenStatus = (BOOL (FAR PASCAL *)(DWORD,BOOL))::GetProcAddress (hIMM, "ImmSetOpenStatus"); 
   }
}

void	Led_IME::NotifyPosition (HWND hWnd, const SHORT x, const SHORT y)
{
	if (x != fLastX || y != fLastY)  {
		UpdatePosition (hWnd, x, y);
	}
}

void	Led_IME::NotifyOfFontChange (HWND hWnd, const LOGFONT& lf)
{
	if (fImmGetContext != NULL and fImmSetCompositionFont != NULL and fImmReleaseContext != NULL) {
		DWORD	hImc	=	NULL;
		if ((hImc = fImmGetContext (hWnd)) != NULL) {
			fImmSetCompositionFont (hImc, &lf);
			fImmReleaseContext (hWnd, hImc);
		}
	}
}

#if		!qUseNewIMECode
void	Led_IME::SendSimpleMessage (HWND hWnd, UINT fnc, WPARAM wParam)
{
	if (fSendIMEMessageProc != NULL) {
		HANDLE		hime =	::GlobalAlloc (GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE, (DWORD)sizeof(IMESTRUCT));
		LPIMESTRUCT lpime;
		if (hime) lpime = (LPIMESTRUCT)GlobalLock(hime);
		else return;
	
		if (lpime == NULL) {
			GlobalFree(hime);
			return;
		}
		lpime->fnc = fnc;
		lpime->wParam = wParam;
		fSendIMEMessageProc (hWnd, (LONG)hime);
		wParam = lpime->wParam;
		::GlobalUnlock(hime);
		::GlobalFree(hime);
	}
}
#endif

void	Led_IME::IMEOn (HWND hWnd)
{
#if		qUseNewIMECode
		if (fImmGetContext != NULL and fImmSetOpenStatus != NULL and fImmReleaseContext != NULL) {
			DWORD	hImc	=	NULL;
			if ((hImc = fImmGetContext (hWnd)) != NULL) {
				Led_Verify (fImmSetOpenStatus (hImc, true));
				fImmReleaseContext (hWnd, hImc);
			}
		}
#else
	SendSimpleMessage (hWnd, IME_SETOPEN, 1);
#endif
}

void	Led_IME::IMEOff (HWND hWnd)
{
#if		qUseNewIMECode
		if (fImmGetContext != NULL and fImmSetOpenStatus != NULL and fImmReleaseContext != NULL) {
			DWORD	hImc	=	NULL;
			if ((hImc = fImmGetContext (hWnd)) != NULL) {
				Led_Verify (fImmSetOpenStatus (hImc, false));
				fImmReleaseContext (hWnd, hImc);
			}
		}
#else
	SendSimpleMessage (hWnd, IME_SETOPEN, 0);
#endif
}

void	Led_IME::UpdatePosition (const HWND hWnd, const SHORT x, const SHORT y)
{
	if (fSendIMEMessageProc != NULL) {
#if		qUseNewIMECode
		if (fImmGetContext != NULL and fImmSetCompositionWindow != NULL and fImmReleaseContext != NULL) {
			DWORD	hImc	=	NULL;
			if ((hImc = fImmGetContext (hWnd)) != NULL) {
				COMPOSITIONFORM	compForm;
				memset (&compForm, 0, sizeof (compForm));
				compForm.dwStyle = CFS_FORCE_POSITION;
				compForm.ptCurrentPos.x = x;
				compForm.ptCurrentPos.y = y;
				Led_Verify (fImmSetCompositionWindow (hImc, &compForm));
				fImmReleaseContext (hWnd, hImc);
				fLastX = x;
				fLastY = y;
			}
		}
#else
		HANDLE		hime	= ::GlobalAlloc (GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE, (DWORD)sizeof(IMESTRUCT));
		LPIMESTRUCT lpime	=	NULL;
		if (hime != NULL) {
			lpime = (LPIMESTRUCT)GlobalLock(hime);
		}
		else {
			return;
		}
	
		if (lpime == NULL) {
			return;
		}

		lpime->fnc		 = IME_SETCONVERSIONWINDOW; // called IME_MOVECONVERTWINDOW in Win3.1
		lpime->wParam	 = MCW_WINDOW;
		lpime->wCount	 = 0;
		lpime->dchSource = 0;
		lpime->dchDest	 = 0;
		lpime->lParam1	 = MAKELONG(x, y);
		lpime->lParam2	 = 0L;
		lpime->lParam3	 = 0L;
	
		// SendIMEMessageProc returns 0 if there is an error, in which case
		// the error code is returned in IMESTRUCT.wParam;
		short ret = fSendIMEMessageProc (hWnd, (LONG)hime);
		ret = ret ? 0 : (short)lpime->wParam;
		
		::GlobalUnlock (hime);
		::GlobalFree (hime);
		if (!ret) {
			fLastX = x;
			fLastY = y;
		}
#endif

		// Should I redo this taking the LOGFONT as an arg to Led_IME::UpdatePosition ()??? LGP 980714
		if (fImmGetContext != NULL and fImmSetCompositionFont != NULL and fImmReleaseContext != NULL) {
			HFONT	hFont	=	NULL;
			if ((hFont = (HFONT)::SendMessage (hWnd, WM_GETFONT, 0, 0L)) != NULL) {
				LOGFONT	lFont;
				if (::GetObject (hFont, sizeof (LOGFONT), &lFont)) {
					DWORD	hImc	=	NULL;
					if ((hImc = fImmGetContext (hWnd)) != NULL) {
						fImmSetCompositionFont (hImc, &lFont);
						fImmReleaseContext (hWnd, hImc);
					}
				}
			}
		}
	}
}

wstring	Led_IME::GetCompositionResultStringW (HWND hWnd)
{
	wstring	result;
	if (fImmGetCompositionStringW != NULL and fImmGetContext != NULL and fImmReleaseContext != NULL) {
		DWORD	hImc	=	NULL;
		if ((hImc = fImmGetContext (hWnd)) != NULL) {
			wchar_t	curIMEString[2048];
			LONG nChars  =	fImmGetCompositionStringW (hImc, GCS_RESULTSTR, curIMEString, Led_NEltsOf (curIMEString));

			nChars /= sizeof (wchar_t);		// why???? LGP 991214
			if (nChars >= 0 and nChars < Led_NEltsOf (curIMEString)) {
				curIMEString[nChars] = '\0';
			}
			else {
				curIMEString[0] = '\0';
			}
			result = curIMEString;
			fImmReleaseContext (hWnd, hImc);
		}
	}
	return result;
}
#endif



Led_Rect	CenterRectInRect (const Led_Rect& r, const Led_Rect& centerIn)
{
	Led_Coordinate	xLeft	=	(centerIn.left + centerIn.right) / 2 - r.GetWidth () / 2;
	Led_Coordinate	yTop	=	(centerIn.top + centerIn.bottom) / 2 - r.GetHeight () / 2;
	return Led_Rect (yTop, xLeft, r.GetHeight (), r.GetWidth ());
}

#if		qWindows
void	Led_CenterWindowInParent (HWND w)
{
	Led_Assert (::IsWindow (w));
	HWND hWndCenter = ::GetWindow (w, GW_OWNER);
	if (hWndCenter == NULL) {
		hWndCenter = ::GetDesktopWindow ();
	}
	Led_Assert (::IsWindow (hWndCenter));

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect (w, &rcDlg);
	RECT rcCenter;
	::GetWindowRect (hWndCenter, &rcCenter);

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - AsLedRect (rcDlg).GetWidth() / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - AsLedRect (rcDlg).GetHeight() / 2;

	// map screen coordinates to child coordinates
	::SetWindowPos (w, NULL, xLeft, yTop, -1, -1,	SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
#endif












////////////////////////////// PRIVATE UTILITIES

#if		qWindows && qWideCharacters && qUseFakeTTGetWPlacementToImage

///////////////////////////////////////////////////////////////////////////////////
////////////// CODE FROM Microsoft Knowledge Base Article - 241020  ///////////////
///////////////////////////////////////////////////////////////////////////////////
///////// http://support.microsoft.com/default.aspx?scid=kb;en-us;241020 //////////
///////////////////////////////////////////////////////////////////////////////////


#pragma pack(1)     // for byte alignment
// We need byte alignment to be structure compatible with the
// contents of a TrueType font file


// Macros to swap from Big Endian to Little Endian
#define SWAPWORD(x) MAKEWORD( \
    HIBYTE(x), \
    LOBYTE(x) \
    )
#define SWAPLONG(x) MAKELONG( \
    SWAPWORD(HIWORD(x)), \
    SWAPWORD(LOWORD(x)) \
    )


typedef struct _CMap4   // From the TrueType Spec. revision 1.66
{
    USHORT format;          // Format number is set to 4. 
    USHORT length;          // Length in bytes. 
    USHORT version;         // Version number (starts at 0).
    USHORT segCountX2;      // 2 x segCount.
    USHORT searchRange;     // 2 x (2**floor(log2(segCount)))
    USHORT entrySelector;   // log2(searchRange/2)
    USHORT rangeShift;      // 2 x segCount - searchRange

    USHORT Arrays[1];       // Placeholder symbol for address of arrays following
} CMAP4, *LPCMAP4;


/*  CMAP table Data
    From the TrueType Spec revision 1.66

    USHORT  Table Version #
    USHORT  Number of encoding tables
*/ 
#define     CMAPHEADERSIZE  (sizeof(USHORT)*2)


/*  ENCODING entry Data aka CMAPENCODING
    From the TrueType Spec revision 1.66

    USHORT  Platform Id
    USHORT  Platform Specific Encoding Id
    ULONG   Byte Offset from beginning of table
*/ 
#define     ENCODINGSIZE    (sizeof(USHORT)*2 + sizeof(ULONG))

typedef struct _CMapEncoding
{
    USHORT  PlatformId;
    USHORT  EncodingId;
    ULONG   Offset;
} CMAPENCODING;


// Macro to pack a TrueType table name into a DWORD
#define     MAKETABLENAME(ch1, ch2, ch3, ch4) (\
    (((DWORD)(ch4)) << 24) | \
    (((DWORD)(ch3)) << 16) | \
    (((DWORD)(ch2)) << 8) | \
    ((DWORD)(ch1)) \
    )

/* public functions */ 
static	USHORT GetTTUnicodeGlyphIndex(HDC hdc, USHORT ch);
static	USHORT GetTTUnicodeCharCount(HDC hdc);


// DWORD packed four letter table name for each GetFontData()
// function call when working with the CMAP TrueType table
static	DWORD dwCmapName = MAKETABLENAME( 'c','m','a','p' );

static	USHORT *GetEndCountArray(LPBYTE pBuff)
{
    return (USHORT *)(pBuff + 7 * sizeof(USHORT));  // Per TT spec
}

static	USHORT *GetStartCountArray(LPBYTE pBuff)
{
    DWORD   segCount = ((LPCMAP4)pBuff)->segCountX2/2;
    return (USHORT *)( pBuff + 
        8 * sizeof(USHORT) +        // 7 header + 1 reserved USHORT
        segCount*sizeof(USHORT) );  // Per TT spec
}

static	USHORT *GetIdDeltaArray(LPBYTE pBuff)
{
    DWORD   segCount = ((LPCMAP4)pBuff)->segCountX2/2;
    return (USHORT *)( pBuff + 
        8 * sizeof(USHORT) +        // 7 header + 1 reserved USHORT
        segCount * 2 * sizeof(USHORT) );    // Per TT spec
}

static	USHORT *GetIdRangeOffsetArray(LPBYTE pBuff)
{
    DWORD   segCount = ((LPCMAP4)pBuff)->segCountX2/2;
    return (USHORT *)( pBuff + 
        8 * sizeof(USHORT) +        // 7 header + 1 reserved USHORT
        segCount * 3 * sizeof(USHORT) );    // Per TT spec
}


static	void SwapArrays( LPCMAP4 pFormat4 )
{
    DWORD   segCount = pFormat4->segCountX2/2;  // Per TT Spec
    DWORD   i;
    USHORT  *pGlyphId, 
            *pEndOfBuffer, 
            *pstartCount    = GetStartCountArray( (LPBYTE)pFormat4 ), 
            *pidDelta       = GetIdDeltaArray( (LPBYTE)pFormat4 ), 
            *pidRangeOffset = GetIdRangeOffsetArray( (LPBYTE)pFormat4 ), 
            *pendCount      = GetEndCountArray( (LPBYTE)pFormat4 );

    // Swap the array elements for Intel.
    for (i=0; i < segCount; i++)
    {
        pendCount[i] = SWAPWORD(pendCount[i]);
        pstartCount[i] = SWAPWORD(pstartCount[i]);
        pidDelta[i] = SWAPWORD(pidDelta[i]);
        pidRangeOffset[i] = SWAPWORD(pidRangeOffset[i]);
    }

    // Swap the Glyph Id array
    pGlyphId = pidRangeOffset + segCount;   // Per TT spec
    pEndOfBuffer = (USHORT*)((LPBYTE)pFormat4 + pFormat4->length);
    for (;pGlyphId < pEndOfBuffer; pGlyphId++)
    {
        *pGlyphId = SWAPWORD(*pGlyphId);
    }
} /* end of function SwapArrays */ 


static	BOOL GetFontEncoding ( 
    HDC hdc, 
    CMAPENCODING * pEncoding, 
    int iEncoding 
    )
/*
    Note for this function to work correctly, structures must 
    have byte alignment.
*/ 
{
    DWORD   dwResult;
    BOOL    fSuccess = TRUE;

    // Get the structure data from the TrueType font
    dwResult = GetFontData ( 
        hdc, 
        dwCmapName, 
        CMAPHEADERSIZE + ENCODINGSIZE*iEncoding, 
        pEncoding, 
        sizeof(CMAPENCODING) );
    fSuccess = (dwResult == sizeof(CMAPENCODING));

    // swap the Platform Id for Intel
    pEncoding->PlatformId = SWAPWORD(pEncoding->PlatformId);

    // swap the Specific Id for Intel
    pEncoding->EncodingId = SWAPWORD(pEncoding->EncodingId);

    // swap the subtable offset for Intel
    pEncoding->Offset = SWAPLONG(pEncoding->Offset);

    return fSuccess;

} /* end of function GetFontEncoding */ 

static	BOOL GetFontFormat4Header ( 
    HDC hdc, 
    LPCMAP4 pFormat4, 
    DWORD dwOffset 
    )
/*
    Note for this function to work correctly, structures must 
    have byte alignment.
*/ 
{
    BOOL    fSuccess = TRUE;
    DWORD   dwResult;
    int     i;
    USHORT  *pField;

    // Loop and Alias a writeable pointer to the field of interest
    pField = (USHORT *)pFormat4;

    for (i=0; i < 7; i++)
    {
        // Get the field from the subtable
        dwResult = GetFontData ( 
            hdc, 
            dwCmapName, 
            dwOffset + sizeof(USHORT)*i, 
            pField, 
            sizeof(USHORT) );
        
        // swap it to make it right for Intel.
        *pField = SWAPWORD(*pField);
        // move on to the next
        pField++;
        // accumulate our success
        fSuccess = (dwResult == sizeof(USHORT)) && fSuccess;
    }

    return fSuccess;

} /* end of function GetFontFormat4Header */ 

static	BOOL GetFontFormat4Subtable ( 
    HDC hdc,                    // DC with TrueType font
    LPCMAP4 pFormat4Subtable,   // destination buffer
    DWORD   dwOffset            // Offset within font
    )
{
    DWORD   dwResult;
    USHORT  length;
    

    // Retrieve the header values in swapped order
    if (!GetFontFormat4Header ( hdc, 
        pFormat4Subtable, 
        dwOffset ))
    {
        return FALSE;
    }

    // Get the rest of the table
    length = pFormat4Subtable->length - (7 * sizeof(USHORT));
    dwResult = GetFontData( hdc, 
        dwCmapName,
        dwOffset + 7 * sizeof(USHORT),      // pos of arrays
        (LPBYTE)pFormat4Subtable->Arrays,   // destination
        length );       

    if ( dwResult != length)
    {
        // We really shouldn't ever get here
        return FALSE;
    }
    
    // Swamp the arrays
    SwapArrays( pFormat4Subtable );

    return TRUE;
}

static	USHORT GetFontFormat4CharCount (
    LPCMAP4 pFormat4    // pointer to a valid Format4 subtable
    )
{
    USHORT  i,
            *pendCount = GetEndCountArray((LPBYTE) pFormat4),
            *pstartCount = GetStartCountArray((LPBYTE) pFormat4),
            *idRangeOffset = GetIdRangeOffsetArray( (LPBYTE) pFormat4 );

    // Count the # of glyphs
    USHORT nGlyphs = 0;

    if ( pFormat4 == NULL )
        return 0;

    // by adding up the coverage of each segment
    for (i=0; i < (pFormat4->segCountX2/2); i++)
    {

        if ( idRangeOffset[i] == 0)
        {
            // if per the TT spec, the idRangeOffset element is zero,
            // all of the characters in this segment exist.
            nGlyphs += pendCount[i] - pstartCount[i] +1;
        }
        else
        {
            // otherwise we have to test for glyph existence for
            // each character in the segment.
            USHORT idResult;    //Intermediate id calc.
            USHORT ch;

            for (ch = pstartCount[i]; ch <= pendCount[i]; ch++)
            {
                // determine if a glyph exists
                idResult = *(
                    idRangeOffset[i]/2 + 
                    (ch - pstartCount[i]) + 
                    &idRangeOffset[i]
                    );  // indexing equation from TT spec
                if (idResult != 0)
                    // Yep, count it.
                    nGlyphs++;
            }
        }
    }

    return nGlyphs;
} /* end of function GetFontFormat4CharCount */ 

static	BOOL GetTTUnicodeCoverage ( 
    HDC hdc,            // DC with TT font
    LPCMAP4 pBuffer,    // Properly allocated buffer
    DWORD cbSize,       // Size of properly allocated buffer
    DWORD *pcbNeeded    // size of buffer needed
    )
/*
    if cbSize is to small or zero, or if pBuffer is NULL the function
    will fail and return the required buffer size in *pcbNeeded.

    if another error occurs, the function will fail and *pcbNeeded will
    be zero.

    When the function succeeds, *pcbNeeded contains the number of bytes 
    copied to pBuffer.
*/ 
{
    USHORT          nEncodings;     // # of encoding in the TT font
    CMAPENCODING    Encoding;       // The current encoding
    DWORD           dwResult;
    DWORD           i, 
                    iUnicode;       // The Unicode encoding
    CMAP4           Format4;        // Unicode subtable format
    LPCMAP4         pFormat4Subtable;   // Working buffer for subtable

    // Get the number of subtables in the CMAP table from the CMAP header
    // The # of subtables is the second USHORT in the CMAP table, per the TT Spec.
    dwResult = GetFontData ( hdc, dwCmapName, sizeof(USHORT), &nEncodings, sizeof(USHORT) );
    nEncodings = SWAPWORD(nEncodings);
    
    if ( dwResult != sizeof(USHORT) )
    {
        // Something is wrong, we probably got GDI_ERROR back
        // Probably this means that the Device Context does not have
        // a TrueType font selected into it.
        return FALSE;
    }

    // Get the encodings and look for a Unicode Encoding
    iUnicode = nEncodings;
    for (i=0; i < nEncodings; i++)
    {
        // Get the encoding entry for each encoding
        if (!GetFontEncoding ( hdc, &Encoding, i ))
        {
            *pcbNeeded = 0;
            return FALSE;
        }
        
        // Take note of the Unicode encoding.
        // 
        // A Unicode encoding per the TrueType specification has a
        // Platform Id of 3 and a Platform specific encoding id of 1
        // Note that Symbol fonts are supposed to have a Platform Id of 3 
        // and a specific id of 0. If the TrueType spec. suggestions were
        // followed then the Symbol font's Format 4 encoding could also
        // be considered Unicode because the mapping would be in the
        // Private Use Area of Unicode. We assume this here and allow 
        // Symbol fonts to be interpreted. If they do not contain a 
        // Format 4, we bail later. If they do not have a Unicode 
        // character mapping, we'll get wrong results.
        // Code could infer from the coverage whether 3-0 fonts are 
        // Unicode or not by examining the segments for placement within
        // the Private Use Area Subrange.
        if (Encoding.PlatformId == 3 && 
            (Encoding.EncodingId == 1 || Encoding.EncodingId == 0) )
        {
            iUnicode = i;       // Set the index to the Unicode encoding
        }
    }

    // index out of range means failure to find a Unicode mapping
    if (iUnicode >= nEncodings)
    {
        // No Unicode encoding found.
        *pcbNeeded = 0;
        return FALSE;
    }

    // Get the header entries(first 7 USHORTs) for the Unicode encoding.
    if ( !GetFontFormat4Header ( hdc, &Format4, Encoding.Offset ) )
    {
        *pcbNeeded = 0;
        return FALSE;
    }

    // Check to see if we retrieved a Format 4 table 
    if ( Format4.format != 4 )
    {
        // Bad, subtable is not format 4, bail.
        // This could happen if the font is corrupt
        // It could also happen if there is a new font format we
        // don't understand.
        *pcbNeeded = 0;
        return FALSE;
    }

    // Figure buffer size and tell caller if buffer to small
    *pcbNeeded = Format4.length;    
    if (*pcbNeeded > cbSize || pBuffer == NULL)
    {
        // Either test indicates caller needs to know
        // the buffer size and the parameters are not setup
        // to continue.
        return FALSE;
    }

    // allocate a full working buffer
    pFormat4Subtable = (LPCMAP4)malloc ( Format4.length );
    if ( pFormat4Subtable == NULL)
    {
        // Bad things happening if we can't allocate memory
        *pcbNeeded = 0;
        return FALSE;
    }

    // get the entire subtable
    if (!GetFontFormat4Subtable ( hdc, pFormat4Subtable, Encoding.Offset ))
    {
        // Bad things happening if we can't allocate memory
        *pcbNeeded = 0;
        return FALSE;
    }

    // Copy the retrieved table into the buffer
    CopyMemory( pBuffer, 
        pFormat4Subtable, 
        pFormat4Subtable->length );

    free ( pFormat4Subtable );
    return TRUE;
} /* end of function GetTTUnicodeCoverage */ 

static	BOOL FindFormat4Segment (
    LPCMAP4 pTable,     // a valid Format4 subtable buffer
    USHORT ch,          // Unicode character to search for
    USHORT *piSeg       // out: index of segment containing ch
    )
/*
    if the Unicode character ch is not contained in one of the 
    segments the function returns FALSE.

    if the Unicode character ch is found in a segment, the index
    of the segment is placed in*piSeg and the function returns
    TRUE.
*/ 
{
    USHORT  i, 
            segCount = pTable->segCountX2/2;
    USHORT  *pendCount = GetEndCountArray((LPBYTE) pTable);
    USHORT  *pstartCount = GetStartCountArray((LPBYTE) pTable);

    // Find segment that could contain the Unicode character code
    for (i=0; i < segCount && pendCount[i] < ch; i++);

    // We looked in them all, ch not there
    if (i >= segCount)
        return FALSE;
    
    // character code not within the range of the segment
    if (pstartCount[i] > ch)
        return FALSE;

    // this segment contains the character code
    *piSeg = i;
    return TRUE;
} /* end of function FindFormat4Segment */ 

static	USHORT GetTTUnicodeCharCount ( 
    HDC hdc
    )
/*
    Returns the number of Unicode character glyphs that 
    are in the TrueType font that is selected into the hdc.
*/ 
{
    LPCMAP4 pUnicodeCMapTable;
    USHORT  cChar;
    DWORD   dwSize;

    // Get the Unicode CMAP table from the TT font
    GetTTUnicodeCoverage( hdc, NULL, 0, &dwSize );
    pUnicodeCMapTable = (LPCMAP4)malloc( dwSize );
    if (!GetTTUnicodeCoverage( hdc, pUnicodeCMapTable, dwSize, &dwSize ))
    {
        // possibly no Unicode cmap, not a TT font selected,...
        free( pUnicodeCMapTable );
        return 0;
    }

    cChar = GetFontFormat4CharCount( pUnicodeCMapTable );
    free( pUnicodeCMapTable );

    return cChar;
} /* end of function GetTTUnicodeCharCount */ 


static	USHORT GetTTUnicodeGlyphIndex (
    HDC hdc,        // DC with a TrueType font selected
    USHORT ch       // Unicode character to convert to Index
    )
/*
    When the TrueType font contains a glyph for ch, the
    function returns the glyph index for that character.

    If an error occurs, or there is no glyph for ch, the
    function will return the missing glyph index of zero.
*/ 
{
    LPCMAP4 pUnicodeCMapTable;
    DWORD   dwSize;
    USHORT  iSegment;
    USHORT  *idRangeOffset;
    USHORT  *idDelta;
    USHORT  *startCount;
    USHORT  GlyphIndex = 0;     // Initialize to missing glyph

    // How big a buffer do we need for Unicode CMAP?
    GetTTUnicodeCoverage( hdc, NULL, 0, &dwSize );
    pUnicodeCMapTable = (LPCMAP4)malloc( dwSize );
    if (!GetTTUnicodeCoverage( hdc, pUnicodeCMapTable, dwSize, &dwSize ))
    {
        // Either no Unicode cmap, or some other error occurred
        // like font in DC is not TT.
        free( pUnicodeCMapTable );
        return 0;       // return missing glyph on error
    }

    // Find the cmap segment that has the character code.
    if (!FindFormat4Segment( pUnicodeCMapTable, ch, &iSegment ))
    {
        free( pUnicodeCMapTable );
        return 0;       // ch not in cmap, return missing glyph
    }

    // Get pointers to the cmap data
    idRangeOffset = GetIdRangeOffsetArray( (LPBYTE) pUnicodeCMapTable );
    idDelta = GetIdDeltaArray( (LPBYTE) pUnicodeCMapTable );
    startCount = GetStartCountArray( (LPBYTE) pUnicodeCMapTable );
    
    // Per TT spec, if the RangeOffset is zero,
    if ( idRangeOffset[iSegment] == 0)
    {
        // calculate the glyph index directly
        GlyphIndex = (idDelta[iSegment] + ch) % 65536;
    }
    else
    {
        // otherwise, use the glyph id array to get the index
        USHORT idResult;    //Intermediate id calc.

        idResult = *(
            idRangeOffset[iSegment]/2 + 
            (ch - startCount[iSegment]) + 
            &idRangeOffset[iSegment]
            );  // indexing equation from TT spec
        if (idResult)
            // Per TT spec, nonzero means there is a glyph
            GlyphIndex = (idDelta[iSegment] + idResult) % 65536;
        else
            // otherwise, return the missing glyph
            GlyphIndex = 0;
    }

    free( pUnicodeCMapTable );
    return GlyphIndex;
} /* end of function GetTTUnicodeGlyphIndex */ 


static	bool	Win9x_Workaround_GetCharPlacementFunction (HDC hdc, const wchar_t* srcText, size_t len, wchar_t* glyphImagesOut)
{
// Should check if really using a true-type font and as the doc "Microsoft Knowledge Base Article - 241020" says:
// This sample code was written for clarity of explanation. It is not optimized for repeated use because
// it allocates and retrieves TrueType tables each time a public function is called. For real applications,
// a good optimization would be to cache the Unicode encoding for the TrueType font file as long as it remained
// in the DC. An application can compare to see whether the font selected into a DC is the same TrueType font file
// by caching and comparing the checksum value of the font file. This checksum is located in the Table Directory
// of the TrueType font file at the beginning of the file and can be retrieved by using the GetFontData function.
// See the TrueType specification's discussion of "The Table Directory" under the Data Types chapter to locate
// the checksum of a font file. 
	for (size_t i = 0; i < len; ++i) {
		glyphImagesOut[i] = GetTTUnicodeGlyphIndex (hdc, srcText[i]);
	}
	return true;
}
#endif


#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

