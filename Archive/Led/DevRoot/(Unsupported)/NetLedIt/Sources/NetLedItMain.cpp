/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Sources/NetLedItMain.cpp,v 1.1 2004/01/01 04:20:39 lewis Exp $
 *
 * Changes:
 *	$Log: NetLedItMain.cpp,v $
 *	Revision 1.1  2004/01/01 04:20:39  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.54  2003/05/22 13:23:57  lewis
 *	SPR#1501: minor tweeks to get compiling again (TextInteractor& arg to DoUndo/DoRedo)
 *	
 *	Revision 1.53  2003/03/25 15:59:58  lewis
 *	SPR#1375- because of recent change to Led - must call InteractiveSetFont() instead of SetDefaultFont in most places I was calling it
 *	
 *	Revision 1.52  2003/03/20 22:29:45  lewis
 *	SPR#1362 - IdleManagerOSImpl_MacNL - tested on Windows - but not yet tested on MacOS
 *	
 *	Revision 1.51  2003/03/11 02:36:03  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.50  2003/01/23 01:28:12  lewis
 *	SPR#1257 - try/catch for Led_URLManager::Open call in about box
 *	
 *	Revision 1.49  2002/09/04 12:56:40  lewis
 *	static_cast<> to silence warnings
 *	
 *	Revision 1.48  2002/05/06 21:35:06  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.47  2001/11/27 00:33:04  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.46  2001/09/26 15:53:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.45  2001/09/26 14:37:05  lewis
 *	SPR#1046- must reset windowrect on each event passed to plugin on MacOS in case
 *	Netscape window has scrolled (else we dont seem to get notififed)
 *	
 *	Revision 1.44  2001/09/24 14:27:42  lewis
 *	SPR#1042 - region code cleanups. SPR#0989 - flicker cleanups. SetOrigin () no
 *	longer being done to funny origin (except to zero and restoring it to what
 *	Netscape suspects). None of this should have been relevant to fixing the bugs but
 *	if this code still works as well, its cleaner an implementation
 *	
 *	Revision 1.43  2001/09/19 16:00:45  lewis
 *	SPR#1030- applied tmpmem / malloc / debug new fixes I had in LedIt! to
 *	LedLineIt (still must test)- MACOS
 *	
 *	Revision 1.42  2001/09/18 14:03:49  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 1.41  2001/09/06 23:13:34  lewis
 *	SPR#1003- Mac about box picture AND SPR#0998- new decent icon for NetLedIt
 *	
 *	Revision 1.40  2001/09/06 20:28:55  lewis
 *	SPR#1003/SPR#0998- cleanup about box and add new decent netledit icon (Win only so far)
 *	
 *	Revision 1.39  2001/08/30 00:43:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.38  2001/08/20 22:23:15  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text).
 *	Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 1.37  2001/08/17 17:03:05  lewis
 *	fix Windows to include unknown etc embedding picts/bmps. SPR#0959 - GetInfo support
 *	Mac and Windows (untested on mac yet)
 *	
 *	Revision 1.36  2001/05/30 15:26:00  lewis
 *	SPR#0945- Added GetVersionNumber/GetShortVersionString () APIs, and testing code to
 *	demo them, and docs in the ref manual
 *	
 *	Revision 1.35  2001/05/10 00:10:28  lewis
 *	Fix Get/SetSelColor APIs to work on MacOS
 *	
 *	Revision 1.34  2001/05/08 22:59:02  lewis
 *	SPR#0907- G(S)etSelFontFace/FontSize/FontColor routines were not implemented- just
 *	stubbed out. Now implemented correctly
 *	
 *	Revision 1.33  2001/04/12 19:46:40  lewis
 *	SPR#0861- Expiration support for demo-mode
 *	
 *	Revision 1.32  2001/03/21 15:19:54  lewis
 *	SPR#0856- fix GetHeight() method of ActiveX control
 *	
 *	Revision 1.31  2001/02/12 15:25:17  lewis
 *	SPR#0854- added java Automation 'GetHeight' method - to compute pixelheight of
 *	a given range of text
 *	
 *	Revision 1.30  2000/10/19 16:00:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.29  2000/10/19 15:52:12  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 1.28  2000/10/17 18:16:28  lewis
 *	mac aboutbox work
 *	
 *	Revision 1.27  2000/10/16 22:49:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.26  2000/10/16 00:17:22  lewis
 *	fix PluginInstance::native_Find(). Add support for windows about box - by using
 *	new Led_StdDialog module
 *	
 *	Revision 1.25  2000/10/14 14:31:30  lewis
 *	start implementing help/aboutbox for Mac/Windows. Got the cmds hooked in. Only
 *	tested on Windows (help). Still to write is aboutbox code (for both platoforms)
 *	
 *	Revision 1.24  2000/10/06 03:35:50  lewis
 *	misc fixes (macos), plus new unified API for SetScrollBarType across mac/pc. And
 *	hooked it into Javscript calls
 *	
 *	Revision 1.23  2000/10/05 17:39:40  lewis
 *	(bad prev checkin message). Add StandardURLStyleMarker::sURLPict etc resource loads/var
 *	specs. Then fixup calls to SetHasScrollBar () to be compat with new Led_MacOS API for scrollbar
 *	
 *	Revision 1.22  2000/10/05 17:36:25  lewis
 *	Added kUnsupportedDIBFormatPictID, kUnknownEmbeddingPictID, kURLPictID
 *	
 *	Revision 1.21  2000/10/05 15:14:27  lewis
 *	fixup so compiles on mac - and added PRESERVE_NETSCAPE_CONTEXT macro to always
 *	fixup mac-port so netscape doesnt get screwed up visually
 *
 *	Revision 1.20  2000/10/05 02:40:25  lewis
 *	last message wrong - really added a bunch more support for Java-called methods
 *	(from javascript/netscape). And much more
 *	
 *	Revision 1.19  2000/10/05 02:38:53  lewis
 *	fix javascript to by syntactic, and tested (most) all the functions here
 *	
 *	Revision 1.18  2000/10/04 20:20:56  lewis
 *	cleanups. And added support for NEW_STREAM/WRITESTREAM (<embed src= param>) support.
 *	
 *	Revision 1.17  2000/09/30 19:37:24  lewis
 *	more work on context menus
 *	
 *	Revision 1.16  2000/09/29 18:06:39  lewis
 *	cleanup first cut at mac context menu code, and did first cut at PC context menu
 *	code - still less than halfway done
 *	
 *	Revision 1.15  2000/09/29 00:19:42  lewis
 *	Added very quickie prelim context menu support for mac code
 *	
 *	Revision 1.14  2000/09/28 21:03:40  lewis
 *	Create separeate LedItView module, and move stuff in there, and start cloning/copying
 *	stuff from ActiveLedIt!
 *	
 *	Revision 1.13  2000/09/28 20:03:43  lewis
 *	Call ::ValidRgn () to work around/eliminate major source of Mac Netscape flicker
 *	
 *	Revision 1.12  2000/09/28 19:50:37  lewis
 *	Finished stubs for (ActiveLedIt! based) Java API for NetLedIt!.
 *	
 *	Revision 1.11  2000/09/28 15:11:22  lewis
 *	changed boolean to jbool- java bool type. And keep track of failure loading java class -
 *	and if so - dont call NetLedIt_PrintToStdOut() from within my Displaymessage () routine -
 *	so that on MSIE 5.5 - we now work (though javasccript methods/controls dont work)
 *	
 *	Revision 1.10  2000/09/28 13:56:18  lewis
 *	Got about 1/3 of the ActiveLedIt! OLE Automation API into NetLedIt!(on pc = mim
 *	testing - about to test on mac)
 *	
 *	Revision 1.9  2000/09/26 13:59:14  lewis
 *	Call fEditor->HandleMouseDownEvent/fEdgitor->HandleAdjustCursorEvent
 *	
 *	Revision 1.8  2000/09/25 21:59:55  lewis
 *	Use Led_MacOS support for drawing, and similarly use Led_MacPortAndClipRegionEtcSaver.
 *	Use a bunch of other Led_MacOS new methods for handling activate/deactiveate/ETC.
 *	
 *	Revision 1.7  2000/09/25 00:47:32  lewis
 *	Mac code cleanups / fixes - now mac code handles keystrokes properly (and tablet / refresh code).
 *	Still alot more todo. Was all maconly fixes
 *	
 *	Revision 1.6  2000/09/23 21:16:07  lewis
 *	more code cleanups - no new functionality
 *	
 *	Revision 1.5  2000/09/23 20:58:32  lewis
 *	lots more code cleanup. Used this as first (very primitive) build to send to RH
 *	
 *	Revision 1.4  2000/09/22 21:35:16  lewis
 *	massive code re-orgs and cleanups. More C++/object oriented now. Still a long way to go.
 *	No new functionality (except now using the WOrdProcessor class and moved some mac specifc
 *	stuff to new Led_MacOS.h file)
 *	
 *
 *
 *
 *
 */

#if		_MSC_VER
	#pragma	warning (disable : 4786)
#endif

#include	<cstddef>
#include	<cstring>

#if		qMacOS
#include	<TextUtils.h>
#endif

#include	"ChunkedArrayTextStore.h"
#include	"LedStdDialogs.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_PlainText.h"
#include	"StyledTextIO_RTF.h"
#include	"WordProcessor.h"


#if		qWindows
#include	"Led_Win32.h"
#elif	qMacOS
#include	"Led_MacOS.h"
#endif


#include	"Resource.h"






/*
 *	Profiling/Mac memory Management.
 */
#if		qMacOS

	#if		qUseMacTmpMemForAllocs
		inline	char*	DoSysAlloc (size_t n)
			{
				#if		DEBUG_NEW
				gDebugNewFlags &= ~dnCheckBlocksInApplZone;
				#endif
				OSErr err;	// ingored...
				Handle	h	=	::TempNewHandle (n, &err);	// try temp mem, and use our local mem if no temp mem left
				if (h == NULL) {
					h = ::NewHandle (n);
					// Don't use up the last few K of heap-memory cuz mac toolbox doesn't like it! -LGP 961021
					if (h != NULL) {
						Handle	x	=	::NewHandle (4*1024);
						if (x == NULL) {
							::DisposeHandle (h);
							h = NULL;
						}
						else {
							::DisposeHandle (x);
						}
					}
				}
				if (h == NULL) {
					return NULL;
				}
				::HLock (h);
				return *h;
			}
		inline	void	DoSysFree (void* p)
			{
				Handle	h	=	::RecoverHandle (reinterpret_cast<Ptr> (p));
				::HUnlock (h);
				::DisposeHandle (h);
			}
		inline	size_t	DoGetPtrSize (void* p)
			{
				Handle	h	=	::RecoverHandle (reinterpret_cast<Ptr> (p));
				return ::GetHandleSize (h);
			}
	#endif

	#if		qUseMacTmpMemForAllocs
		/*
		 *	In CW5Pro and earlier - I hooked into the Metrowerks memory allocation system via #including "New.cpp" and
		 *	doing #defines of NewPtr and DisposePtr () etc. But that no longer works with CWPro7 (I'm not sure about
		 *	CWPro6). As of CWPro7 - there are several malloc implementations. This below code seems to work for all of
		 *	them (from purusing the code in alloc.c, pool_alloc.c, pool_alloc.h, pool_alloc.mac.c). And empiricly it
		 *	works fine for the default allocation algorithm -- LGP 2001-09-19
		 */
		#include	"pool_alloc.h"
		extern	"C" {
			void*	__sys_alloc (std::size_t n)
				{
					return DoSysAlloc (n);
				}
			void	__sys_free (void* p)
				{
					DoSysFree (p);
				}
			std::size_t	__sys_pointer_size (void* p)
				{
					return ::DoGetPtrSize (p);
				}
		}
	#endif

	#if		DEBUG_NEW
	#define	NEWMODE	NEWMODE_MALLOC
	#include	"DebugNew.cp"
	#endif

#endif



#if		qLedUsesNamespaces
using	namespace	Led;
#endif






#include	"npapi.h"

/*------------------------------------------------------------------------------
 * Define IMPLEMENT_NetLedIt before including NetLedIt.h to state that we're
 * implementing the native methods of this plug-in here, and consequently
 * need to access it's protected and private memebers.
 *----------------------------------------------------------------------------*/
#define IMPLEMENT_NetLedIt

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4244)
	#pragma	warning (disable : 4250)		//qQuiteAnnoyingDominanceWarnings
#endif

//GROSS HACK - Orig SDK which came with mac declared base classes as NON-virtual, but the PC javah compiler makes them virtual bases,
// which causes LiveConnect (on mac at least) to barf! - LGP 2000-09-14
#if 1
	#include "jri.h"
	#define virtual
	#include "NetLedIt.h"
	#include "netscape_plugin_Plugin.h"
	#undef virtual
#else
	#include "NetLedIt.h"
	#include "netscape_plugin_Plugin.h"
#endif

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif



#define	CATCH_AND_HANDLE_EXCEPTIONS()\
	catch (...) {\
		/*Not sure how to handle these when called from Java/Netscape - think I must ignore them..*/\
	}\


#include	"LedItView.h"





#if		qWindows
HINSTANCE	ghInstance;
#endif





#ifndef	qDumpTraceMessages
#define	qDumpTraceMessages	qDebug
#endif

#if		qDumpTraceMessages
	#define	TRACEMSG(I,M,L)		DisplayJavaMessage (I, M, L)
#else
	#define	TRACEMSG(I,M,L)
#endif
static	void		DisplayJavaMessage (NPP instance, char* msg, int len);












class	PluginInstance : public MarkerOwner, public LedItViewController	{
	public:
		PluginInstance (NPP instance, uint16 mode);
		~PluginInstance ();


	private:
		NPP	fNPPInstance;


	protected:
		override	void		DidUpdateText (const UpdateInfo& updateInfo) throw ()
			{
				fDataDirty = true;
				MarkerOwner::DidUpdateText (updateInfo);
			}
		override	TextStore*	PeekAtTextStore () const
			{
				return &const_cast<PluginInstance*> (this)->fTextStore;
			}


		virtual	void	OnBrowseHelpCommand ()
			{
				//Led_URLManager::Get ().Open (kNetLedItUserDocsURL);
				Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/NetLedIt/UserDocs.asp", kAppName, kURLDemoFlag));
			}
		virtual	void	OnAboutBoxCommand ()
			{
				class	MyAboutBox : public Led_StdDialogHelper_AboutBox {
					private:
						typedef	Led_StdDialogHelper_AboutBox	inherited;
					public:
						#if		qMacOS
						MyAboutBox ():
							#if		qDemoMode && 0
								inherited (kAlternateAboutBoxResID)
							#else
								inherited ()
							#endif
						{
						}
						#elif	qWindows
							MyAboutBox (HINSTANCE hInstance, HWND parentWnd):
								inherited (hInstance, parentWnd)
							{
							}
						#endif
					public:
	#if		qMacOS
						override	void	PreDoModalHook ()
							{
								inherited::PreDoModalHook ();
								const	short	kPictWidth	=	437;	// must agree with ACTUAL bitmap size
								#if		qDemoMode
								const	short	kPictHeight	=	393;
								#else
								const	short	kPictHeight	=	318;
								#endif
								Led_SDK_String	verStr	=	Led_SDK_String (qLed_ShortVersionString) + " (" + __DATE__ + ")";
								SimpleLayoutHelper (kPictHeight, kPictWidth, Led_Rect (159, 15, 17, 142), Led_Rect (159, 227, 17, 179), verStr);
							}
	#elif	qWindows
						override	BOOL	OnInitDialog ()
							{
								BOOL	result	=	inherited::OnInitDialog ();

								// Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
								// we must resize our dlog on the fly based on pict resource size...
								const	kPictWidth	=	437;	// must agree with ACTUAL bitmap size
								const	kButHSluff	=	17;
								#if		qDemoMode
									const	kButVSluff	=	136;
									const	kPictHeight	=	393;
								#else
									const	kButVSluff	=	61;
									const	kPictHeight	=	318;
								#endif
								{
									RECT	windowRect;
									::GetWindowRect (GetHWND (), &windowRect);
									// figure size of non-client area...
									int	ncWidth		=	0;
									int	ncHeight	=	0;
									{
										RECT	clientRect;
										::GetClientRect (GetHWND (), &clientRect);
										ncWidth = AsLedRect (windowRect).GetWidth () - AsLedRect (clientRect).GetWidth ();
										ncHeight = AsLedRect (windowRect).GetHeight () - AsLedRect (clientRect).GetHeight ();
									}
									::MoveWindow (GetHWND (), windowRect.left, windowRect.top, kPictWidth+ncWidth, kPictHeight+ncHeight, false);
								}

								// Place and fill in version information
								{
									HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_VersionFieldID);
									Led_AssertNotNil (w);
									const	int	kVERWidth	=	230;
									#if		qDemoMode
										::MoveWindow (w, kPictWidth/2 - kVERWidth/2, 35, 230, 2*14, false);
									#else
										::MoveWindow (w, kPictWidth/2 - kVERWidth/2, 35, 230, 14, false);
									#endif
									#if		_UNICODE
										#define	kUNICODE_NAME_ADORNER	L" [UNICODE]"
									#elif	qLed_CharacterSet == qUNICODE_CharacterSet
										#define	kUNICODE_NAME_ADORNER	" [Internal UNICODE]"
									#else
										#define	kUNICODE_NAME_ADORNER
									#endif
									#if		qDemoMode
										TCHAR	nDaysString[1024];
										(void)::_stprintf (nDaysString, _T("%d"), DemoPrefs ().GetDemoDaysLeft ());
									#endif
									::SetWindowText (w,
														(
															Led_SDK_String (_T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"))
															#if		qDemoMode
															 + Led_SDK_String (_T("\r\nDemo expires in ")) + nDaysString + _T(" days.")
															#endif
														).c_str ()
										);
								}

								// Place hidden buttons which map to URLs
								{
									HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
									Led_AssertNotNil (w);
									::MoveWindow (w, 15, 159, 142, 17, false);
									w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
									Led_AssertNotNil (w);
									::MoveWindow (w, 227, 159, 179, 17, false);
								}

								// Place OK button
								{
									HWND	w	=	::GetDlgItem (GetHWND (), IDOK);
									Led_AssertNotNil (w);
									RECT	tmp;
									::GetWindowRect (w, &tmp);
									::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight ()-kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false);	// width/height we should presevere
								}

								::SetWindowText (GetHWND (), _T ("About NetLedIt!"));

								return (result);
							}
#endif
					public:
						override	void	OnClickInInfoField ()
							{
								try {
									Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
								}
								catch (...) {
									// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
								}
								inherited::OnClickInInfoField ();
							}

						override	void	OnClickInLedWebPageField ()
							{
								try {
									//Led_URLManager::Get ().Open (kNetLedItUserPageURL);
									Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/NetLedIt/Default.asp", kAppName, kURLDemoFlag));
								}
								catch (...) {
									// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
								}
								inherited::OnClickInLedWebPageField ();
							}
				};
				#if		qMacOS
					MyAboutBox	dlg;
				#elif	qWindows
					MyAboutBox	dlg (ghInstance, fhWnd);
				#endif
				dlg.DoModal ();
			}

	#if		qMacOS
	public:
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const
			{
				if (not windowRectArea.IsEmpty ()) {
					PluginInstance * THIS = const_cast<PluginInstance*> (this);
					NP_Port*	port = (NP_Port*) fWindow->window;
					WindowRef	macWin	=	reinterpret_cast<WindowRef> (port->port);
					WindowPeek	macWinP	=	reinterpret_cast<WindowPeek> (macWin);
					fEditor->SetupCurrentGrafPort ();
					RgnHandle	winLocalUpdateRgn	=	::NewRgn ();
					::CopyRgn (macWinP->updateRgn, winLocalUpdateRgn);
					{
						Point	p;
						memset (&p, 0, sizeof (p));
						::GlobalToLocal (&p);
						::MacOffsetRgn (winLocalUpdateRgn, p.h, p.v);
					}
					Rect	clipRect;
					clipRect.top = fWindow->clipRect.top ;
					clipRect.left = fWindow->clipRect.left ;
					clipRect.bottom = fWindow->clipRect.bottom ;
					clipRect.right = fWindow->clipRect.right;
					if (not EmptyRgn (winLocalUpdateRgn)) {
						::ClipRect (&clipRect);
						fEditor->Handle_Draw (winLocalUpdateRgn);
						::ValidRgn (winLocalUpdateRgn);
					}
					::DisposeRgn (winLocalUpdateRgn);
				}
			}
		override	void	SetupCurrentGrafPort () const
			{
				const_cast<PluginInstance*> (this)->DoFocus ();
			}
	#endif



	// Netscape API hooks
	public:
		nonvirtual	NPError		Handle_SetWindow (NPWindow* window);
		nonvirtual	int16		Handle_HandleEvent (void* event);

	public:
		nonvirtual	void		Handle_WritePartial (const string& input);
		nonvirtual	void		Handle_WriteCompete ();
	private:
		string	fTmpStreamWriteInput;


	// Automation (LiveScript/Java) public API hooks
	public:
		nonvirtual	JRI_PUBLIC_API(jint)				native_GetVersionNumber (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetShortVersionString (JRIEnv* env);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetDirty (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetDirty (JRIEnv* env, jbool dirty);
	
		nonvirtual	JRI_PUBLIC_API(jint)				native_GetHasVerticalScrollBar (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetHasVerticalScrollBar (JRIEnv* env, jint scrollBarFlag);
		nonvirtual	JRI_PUBLIC_API(jint)				native_GetHasHorizontalScrollBar (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetHasHorizontalScrollBar (JRIEnv* env, jint scrollBarFlag);
		
		nonvirtual	JRI_PUBLIC_API(jint)				native_GetMaxUndoLevel (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetMaxUndoLevel (JRIEnv* env, jint maxUndoLevel);
		
		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetCanUndo (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetCanRedo (JRIEnv* env);
		
		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetText (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetText (JRIEnv* env, java_lang_String* a);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetTextCRLF (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetTextCRLF (JRIEnv* env, java_lang_String* a);

		nonvirtual	string								GetBufferTextAsRTF_ ();
		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetTextRTF (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetTextRTF (JRIEnv* env, java_lang_String* a);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetTextHTML (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetTextHTML (JRIEnv* env, java_lang_String* a);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetLength (JRIEnv* env);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetMaxLength (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetMaxLength (JRIEnv* env, jint maxLength);
		
		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetSupportContextMenu (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSupportContextMenu (JRIEnv* env, jbool supportContextMenu);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetSmartCutAndPaste (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSmartCutAndPaste (JRIEnv* env, jbool smartCutAndPaste);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetWrapToWindow (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetWrapToWindow (JRIEnv* env, jbool wrapToWindow);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetShowParagraphGlyphs (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetShowParagraphGlyphs (JRIEnv* env, jbool showParagraphGlyphs);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetShowTabGlyphs (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetShowTabGlyphs (JRIEnv* env, jbool showTabGlyphs);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetShowSpaceGlyphs (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetShowSpaceGlyphs (JRIEnv* env, jbool showSpaceGlyphs);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetShowHidableText (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetShowHidableText (JRIEnv* env, jbool showHidableText);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetHidableTextColor (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetHidableTextColor (JRIEnv* env, jint hidableTextColor);

		nonvirtual	JRI_PUBLIC_API(jbool)				native_GetHidableTextColored (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetHidableTextColored (JRIEnv* env, jbool hidableTextColored);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelStart (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelStart (JRIEnv* env, jint selStart);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelLength (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelLength (JRIEnv* env, jint selLength);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetSelText (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelText (JRIEnv* env, java_lang_String* selText);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetSelTextRTF (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelTextRTF (JRIEnv* env, java_lang_String* selTextRTF);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetSelTextHTML (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelTextHTML (JRIEnv* env, java_lang_String* selTextHTML);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelColor (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelColor (JRIEnv* env, jint selColor);

		nonvirtual	JRI_PUBLIC_API(java_lang_String*)	native_GetSelFontFace (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelFontFace (JRIEnv* env, java_lang_String* selFontFace);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelFontSize (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelFontSize (JRIEnv* env, jint selFontSize);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelBold (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelBold (JRIEnv* env, jint selBold);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelItalic (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelItalic (JRIEnv* env, jint selItalic);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelUnderline (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelUnderline (JRIEnv* env, jint selUnderline);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelJustification (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelJustification (JRIEnv* env, jint selJustification);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetSelHidable (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_SetSelHidable (JRIEnv* env, jint selHidable);

		nonvirtual	JRI_PUBLIC_API(void)				native_AboutBox (JRIEnv* env);

		nonvirtual	JRI_PUBLIC_API(void)				native_LoadFile (JRIEnv* env, java_lang_String* fileName);
		nonvirtual	JRI_PUBLIC_API(void)				native_SaveFile (JRIEnv* env, java_lang_String* fileName);
		nonvirtual	JRI_PUBLIC_API(void)				native_SaveFileCRLF (JRIEnv* env, java_lang_String* fileName);
		nonvirtual	JRI_PUBLIC_API(void)				native_SaveFileRTF (JRIEnv* env, java_lang_String* fileName);
		nonvirtual	JRI_PUBLIC_API(void)				native_SaveFileHTML (JRIEnv* env, java_lang_String* fileName);

		nonvirtual	JRI_PUBLIC_API(void)				native_Refresh (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_ScrollToSelection (JRIEnv* env);

		nonvirtual	JRI_PUBLIC_API(void)				native_Undo (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_Redo (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(void)				native_CommitUndo (JRIEnv* env);

		nonvirtual	JRI_PUBLIC_API(void)				native_LaunchFindDialog (JRIEnv* env);
		nonvirtual	JRI_PUBLIC_API(jint)				native_Find (JRIEnv* env, jint searchFrom, java_lang_String* findText, jbool wrapSearch, jbool wholeWordSearch, jbool caseSensativeSearch);

		nonvirtual	JRI_PUBLIC_API(jint)				native_GetHeight (JRIEnv* env, jint from, jint to);



#if		qXWindows
	private:
		static	void	Redraw (Widget w, XtPointer closure, XEvent *event);
#endif

#if		qMacOS
	public:
		nonvirtual	void	DoFocus ();
#endif

	private:
		NPWindow*		fWindow;
		uint16			fMode;
		LedItView*		fEditor;
		bool			fDataDirty;
	#if		qWindows
		HWND			fhWnd;
		WNDPROC			fDefaultWindowProc;
	#elif	qXWindows
		Window 			window;
		Display *		display;
		uint32 			x, y;
		uint32 			width, height;
	#endif
};









static	java_lang_Class* myJavaClass	=	NULL;	// Under MSIE 5.5 - we fail to find our Java-class, and so some functionality broken.
													// checking this as a flag prevents us from using the Java stuff - and causing a crash







#if		qMacOS
	#define	PRESERVE_NETSCAPE_CONTEXT\
		Led_MacPortAndClipRegionEtcSaver	contextSaver;
#else
	#define	PRESERVE_NETSCAPE_CONTEXT
#endif






#if		qMacOS
/*
@CLASS:			IdleManagerOSImpl_MacNL
@BASES:			@'IdleManager::IdleManagerOSImpl'
@ACCESS:		public
@DESCRIPTION:	<p>Implemenation detail of the idle-task management system. This can generally be ignored by Led users.
			</p>
*/
class	IdleManagerOSImpl_MacNL : public IdleManager::IdleManagerOSImpl {
	public:
		IdleManagerOSImpl_MacNL ():
			fSuggestedFrequency (0)
			{
			}
		~IdleManagerOSImpl_MacNL ()
			{
			}

	public:
		override	void	StartSpendTimeCalls ()
			{
			}
		override	void	TerminateSpendTimeCalls ()
			{
			}
		override	float	GetSuggestedFrequency () const
			{
				return fSuggestedFrequency;
			}
		override	void	SetSuggestedFrequency (float suggestedFrequency)
			{
				// Unfortunately - for now - it appears we must ignore the suggestedFrequency...
				fSuggestedFrequency = suggestedFrequency;
			}

	public:
		nonvirtual	void	SpendTime ()
			{
				CallSpendTime ();
			}


	private:
		float	fSuggestedFrequency;
};



namespace	{
	/*
	 *	Code to automatically install and remove our idle manager.
	 */
	struct	IdleMangerSetter {
		IdleMangerSetter ()
			{
				IdleManager::Get ().SetIdleManagerOSImpl (&fIdleManagerOSImpl);
			}
		~IdleMangerSetter ()
			{
				IdleManager::Get ().SetIdleManagerOSImpl (NULL);
			}
		IdleManagerOSImpl_MacNL	fIdleManagerOSImpl;
	};
	struct	IdleMangerSetter	sIdleMangerSetter;
}


#endif






#if		qWindows
/*
 *	Hook DllMain just so we can grab the hInstance - so we can use that later to grab our resources.
 */
extern "C"	BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	switch (dwReason) {
		case	DLL_PROCESS_ATTACH: {
			ghInstance = hInstance;
		}
		break;
	}
	return TRUE;
}
#endif



#if		qWindows
inline	const	void*	LoadAppResource (long resID, LPCTSTR resType)
	{
		HRSRC	hrsrc	=	::FindResource (ghInstance, MAKEINTRESOURCE (resID), resType);
		Led_AssertNotNil (hrsrc);
		HGLOBAL	hglobal	=	::LoadResource (ghInstance, hrsrc);
		const void*	lockedData	=	::LockResource (hglobal);
		Led_EnsureNotNil (lockedData);
		return (lockedData);
	}
#endif





 
#if		qXWindows
char*	NPP_GetMIMEDescription ()
{
	return("application/x-netledit-plugin:rtf:NetLedIt! Plug-in");
}

NPError	NPP_GetValue (void *future, NPPVariable variable, void *value)
{
	const	char	PLUGIN_NAME[]			=	"NetLedIt Plug-in";
	const	char	PLUGIN_DESCRIPTION[]	=	"Word processing Netscape plug-in.";
	NPError err = NPERR_NO_ERROR;
	if (variable == NPPVpluginNameString)
		*((char **)value) = PLUGIN_NAME;
	else if (variable == NPPVpluginDescriptionString)
		*((char **)value) = PLUGIN_DESCRIPTION;
	else
		err = NPERR_GENERIC_ERROR;

	return err;
}
#endif




/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_Initialize:
 * Provides global initialization for a plug-in, and returns an error value. 
 *
 * This function is called once when a plug-in is loaded, before the first instance
 * is created. You should allocate any memory or resources shared by all
 * instances of your plug-in at this time. After the last instance has been deleted,
 * NPP_Shutdown will be called, where you can release any memory or
 * resources allocated by NPP_Initialize. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError	NPP_Initialize ()
{
	EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

	// Tell Led about the picture resources it needs to render some special embedding markers
	#if		qMacOS
		#if		!qURLStyleMarkerNewDisplayMode
			StandardURLStyleMarker::sURLPict = (Picture**)::GetResource ('PICT', kURLPictID);
		#endif
		StandardUnknownTypeStyleMarker::sUnknownPict = (Picture**)::GetResource ('PICT', kUnknownEmbeddingPictID);
		StandardDIBStyleMarker::sUnsupportedFormatPict = (Picture**)::GetResource ('PICT', kUnsupportedDIBFormatPictID);
	#elif	qWindows
		#if		!qURLStyleMarkerNewDisplayMode
			StandardURLStyleMarker::sURLPict = (const Led_DIB*)::LoadAppResource (kURLPictID, RT_BITMAP);
		#endif
		StandardUnknownTypeStyleMarker::sUnknownPict = (const Led_DIB*)::LoadAppResource (kUnknownEmbeddingPictID, RT_BITMAP);
		StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadAppResource (kUnsupportedPICTFormatPictID, RT_BITMAP);
	#endif

    return NPERR_NO_ERROR;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_GetJavaClass:
 * New in Netscape Navigator 3.0. 
 *
 * NPP_GetJavaClass is called during initialization to ask your plugin
 * what its associated Java class is. If you don't have one, just return
 * NULL. Otherwise, use the javah-generated "use_" function to both
 * initialize your class and return it. If you can't find your class, an
 * error will be signalled by "use_" and will cause the Navigator to
 * complain to the user.
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
jref	NPP_GetJavaClass ()
{
	JRIEnv* env = NPN_GetJavaEnv ();
	if (env == NULL)
		return NULL;		/* Java disabled */

	java_lang_Class* myClass	= use_NetLedIt (env);
	Led_Assert (myJavaClass == NULL or myJavaClass == myClass);
	myJavaClass = myClass;
	use_netscape_plugin_Plugin ( env );

	return (jref) myClass;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_Shutdown:
 * Provides global deinitialization for a plug-in. 
 * 
 * This function is called once after the last instance of your plug-in is destroyed.
 * Use this function to release any memory or resources shared across all
 * instances of your plug-in. You should be a good citizen and declare that
 * you're not using your java class any more. This allows java to unload
 * it, freeing up memory.
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
void	NPP_Shutdown ()
{
	JRIEnv* env = NPN_GetJavaEnv ();
	if (env) {
		unuse_NetLedIt (env);
		unuse_netscape_plugin_Plugin (env);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_New:
 * Creates a new instance of a plug-in and returns an error value. 
 * 
 * NPP_New creates a new instance of your plug-in with MIME type specified
 * by pluginType. The parameter mode is NP_EMBED if the instance was created
 * by an EMBED tag, or NP_FULL if the instance was created by a separate file.
 * You can allocate any instance-specific private data in instance->pdata at this
 * time. The NPP pointer is valid until the instance is destroyed. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError	NPP_New (NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved)
{
	if (instance == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
	instance->pdata = new PluginInstance (instance, mode);
	return NPERR_NO_ERROR;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_Destroy:
 * Deletes a specific instance of a plug-in and returns an error value. 

 * NPP_Destroy is called when a plug-in instance is deleted, typically because the
 * user has left the page containing the instance, closed the window, or quit the
 * application. You should delete any private instance-specific information stored
 * in instance->pdata. If the instance being deleted is the last instance created
 * by your plug-in, NPP_Shutdown will subsequently be called, where you can
 * delete any data allocated in NPP_Initialize to be shared by all your plug-in's
 * instances. Note that you should not perform any graphics operations in
 * NPP_Destroy as the instance's window is no longer guaranteed to be valid. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError		NPP_Destroy (NPP instance, NPSavedData** save)
{
	TRACEMSG (instance, "Calling NPP_Destroy.", -1);

	if (instance == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
	PluginInstance*	This	= reinterpret_cast<PluginInstance*> (instance->pdata);

	/* PLUGIN DEVELOPERS:
	 *	If desired, call NP_MemAlloc to create a
	 *	NPSavedDate structure containing any state information
	 *	that you want restored if this plugin instance is later
	 *	recreated.
	 */

	delete This;
	instance->pdata = NULL;

	return NPERR_NO_ERROR;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_SetWindow:
 * Sets the window in which a plug-in draws, and returns an error value. 
 * 
 * NPP_SetWindow informs the plug-in instance specified by instance of the
 * the window denoted by window in which the instance draws. This NPWindow
 * pointer is valid for the life of the instance, or until NPP_SetWindow is called
 * again with a different value. Subsequent calls to NPP_SetWindow for a given
 * instance typically indicate that the window has been resized. If either window
 * or window->window are NULL, the plug-in must not perform any additional
 * graphics operations on the window and should free any resources associated
 * with the window. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError	NPP_SetWindow (NPP instance, NPWindow* window)
{
	/*
	 * PLUGIN DEVELOPERS:
	 *	Before setting window to point to the
	 *	new window, you may wish to compare the new window
	 *	info to the previous window (if any) to note window
	 *	size changes, etc.
	 */

	if (instance == NULL or instance->pdata == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
	return reinterpret_cast<PluginInstance*> (instance->pdata)->Handle_SetWindow (window);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_NewStream:
 * Notifies an instance of a new data stream and returns an error value. 
 * 
 * NPP_NewStream notifies the instance denoted by instance of the creation of
 * a new stream specifed by stream. The NPStream* pointer is valid until the
 * stream is destroyed. The MIME type of the stream is provided by the
 * parameter type. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError	NPP_NewStream (NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable, uint16 *stype)
{
	TRACEMSG (instance, "Calling NPP_NewStream.", -1); 

	if (instance == NULL or instance->pdata == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
	return NPERR_NO_ERROR;
}


/* PLUGIN DEVELOPERS:
 *	These next 2 functions are directly relevant in a plug-in which
 *	handles the data in a streaming manner. If you want zero bytes
 *	because no buffer space is YET available, return 0. As long as
 *	the stream has not been written to the plugin, Navigator will
 *	continue trying to send bytes.  If the plugin doesn't want them,
 *	just return some large number from NPP_WriteReady(), and
 *	ignore them in NPP_Write().  For a NP_ASFILE stream, they are
 *	still called but can safely be ignored using this strategy.
 */

int32 STREAMBUFSIZE = 0X0FFFFFFF; /* If we are reading from a file in NPAsFile
								   * mode so we can take any size stream in our
								   * write call (since we ignore it) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_WriteReady:
 * Returns the maximum number of bytes that an instance is prepared to accept
 * from the stream. 
 * 
 * NPP_WriteReady determines the maximum number of bytes that the
 * instance will consume from the stream in a subsequent call NPP_Write. This
 * function allows Netscape to only send as much data to the instance as the
 * instance is capable of handling at a time, allowing more efficient use of
 * resources within both Netscape and the plug-in. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
int32	NPP_WriteReady (NPP instance, NPStream *stream)
{
	TRACEMSG (instance, "Calling NPP_WriteReady.", -1); 

	if (instance == NULL or instance->pdata == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}

	/* Number of bytes ready to accept in NPP_Write() */
	return STREAMBUFSIZE;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_Write:
 * Delivers data from a stream and returns the number of bytes written. 
 * 
 * NPP_Write is called after a call to NPP_NewStream in which the plug-in
 * requested a normal-mode stream, in which the data in the stream is delivered
 * progressively over a series of calls to NPP_WriteReady and NPP_Write. The
 * function delivers a buffer buf of len bytes of data from the stream identified
 * by stream to the instance. The parameter offset is the logical position of
 * buf from the beginning of the data in the stream. 
 * 
 * The function returns the number of bytes written (consumed by the instance).
 * A negative return value causes an error on the stream, which will
 * subsequently be destroyed via a call to NPP_DestroyStream. 
 * 
 * Note that a plug-in must consume at least as many bytes as it indicated in the
 * preceeding NPP_WriteReady call. All data consumed must be either processed
 * immediately or copied to memory allocated by the plug-in: the buf parameter
 * is not persistent. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
int32	NPP_Write (NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
	if (instance == NULL or instance->pdata == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
	TRACEMSG (instance, (char*)buffer, len); 
	reinterpret_cast<PluginInstance*> (instance->pdata)->Handle_WritePartial (string (reinterpret_cast<char*> (buffer), len));
	return len;		/* The number of bytes accepted */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_DestroyStream:
 * Indicates the closure and deletion of a stream, and returns an error value. 
 * 
 * The NPP_DestroyStream function is called when the stream identified by
 * stream for the plug-in instance denoted by instance will be destroyed. You
 * should delete any private data allocated in stream->pdata at this time. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
NPError	NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
	if (instance == NULL or instance->pdata == NULL) {
		return NPERR_INVALID_INSTANCE_ERROR;
	}

	TRACEMSG (instance, "Calling NPP_DestroyStream.", -1); 
	reinterpret_cast<PluginInstance*> (instance->pdata)->Handle_WriteCompete ();
	return NPERR_NO_ERROR;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_StreamAsFile:
 * Provides a local file name for the data from a stream. 
 * 
 * NPP_StreamAsFile provides the instance with a full path to a local file,
 * identified by fname, for the stream specified by stream. NPP_StreamAsFile is
 * called as a result of the plug-in requesting mode NP_ASFILEONLY or
 * NP_ASFILE in a previous call to NPP_NewStream. If an error occurs while
 * retrieving the data or writing the file, fname may be NULL. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
void	NPP_StreamAsFile (NPP instance, NPStream *stream, const char* fname)
{
	if (instance == NULL or instance->pdata == NULL) {
		return;		// no error return value??? Queer API? LGP 2000-09-22
	}

	TRACEMSG (instance, "Calling NPP_StreamAsFile.", -1); 
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_Print:
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
void	NPP_Print(NPP instance, NPPrint* printInfo)
{
	TRACEMSG (instance, "Calling NPP_Print.", -1); 

	if(printInfo == NULL)
		return;

	if (instance != NULL) {
		PluginInstance* This = (PluginInstance*) instance->pdata;
	
		if (printInfo->mode == NP_FULL) {
		    /*
		     * PLUGIN DEVELOPERS:
		     *	If your plugin would like to take over
		     *	printing completely when it is in full-screen mode,
		     *	set printInfo->pluginPrinted to TRUE and print your
		     *	plugin as you see fit.  If your plugin wants Netscape
		     *	to handle printing in this case, set
		     *	printInfo->pluginPrinted to FALSE (the default) and
		     *	do nothing.  If you do want to handle printing
		     *	yourself, printOne is true if the print button
		     *	(as opposed to the print menu) was clicked.
		     *	On the Macintosh, platformPrint is a THPrint; on
		     *	Windows, platformPrint is a structure
		     *	(defined in npapi.h) containing the printer name, port,
		     *	etc.
		     */

			void* platformPrint =
				printInfo->print.fullPrint.platformPrint;
			NPBool printOne =
				printInfo->print.fullPrint.printOne;
			
			/* Do the default*/
			printInfo->print.fullPrint.pluginPrinted = FALSE;
		}
		else {	/* If not fullscreen, we must be embedded */
		    /*
		     * PLUGIN DEVELOPERS:
		     *	If your plugin is embedded, or is full-screen
		     *	but you returned false in pluginPrinted above, NPP_Print
		     *	will be called with mode == NP_EMBED.  The NPWindow
		     *	in the printInfo gives the location and dimensions of
		     *	the embedded plugin on the printed page.  On the
		     *	Macintosh, platformPrint is the printer port; on
		     *	Windows, platformPrint is the handle to the printing
		     *	device context.
		     */

			NPWindow* printWindow =
				&(printInfo->print.embedPrint.window);
			void* platformPrint =
				printInfo->print.embedPrint.platformPrint;
		}
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_URLNotify:
 * Notifies the instance of the completion of a URL request. 
 * 
 * NPP_URLNotify is called when Netscape completes a NPN_GetURLNotify or
 * NPN_PostURLNotify request, to inform the plug-in that the request,
 * identified by url, has completed for the reason specified by reason. The most
 * common reason code is NPRES_DONE, indicating simply that the request
 * completed normally. Other possible reason codes are NPRES_USER_BREAK,
 * indicating that the request was halted due to a user action (for example,
 * clicking the "Stop" button), and NPRES_NETWORK_ERR, indicating that the
 * request could not be completed (for example, because the URL could not be
 * found). The complete list of reason codes is found in npapi.h. 
 * 
 * The parameter notifyData is the same plug-in-private value passed as an
 * argument to the corresponding NPN_GetURLNotify or NPN_PostURLNotify
 * call, and can be used by your plug-in to uniquely identify the request. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
void	NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
	/* Not used in the NetLedIt plugin. */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * NPP_HandleEvent:
 * Mac-only, but stub must be present for Windows
 * Delivers a platform-specific event to the instance. 
 * 
 * On the Macintosh, event is a pointer to a standard Macintosh EventRecord.
 * All standard event types are passed to the instance as appropriate. In general,
 * return TRUE if you handle the event and FALSE if you ignore the event. 
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
#if		qMacOS || qWindows
int16	NPP_HandleEvent (NPP instance, void* event)
{
	if (instance == NULL or instance->pdata == NULL) {
		return false;
	}
	return reinterpret_cast<PluginInstance*> (instance->pdata)->Handle_HandleEvent (event);
}
#endif







/*******************************************************************************
 * SECTION 4 - Java Native Method Implementations
 ******************************************************************************/

#define	JAVA_NATIVE_WRAPPER_GLOP(CODE)\
	if (env != NULL) {\
		NPP instance = (NPP)self->getPeer (env);\
		if (instance != NULL) {\
			PluginInstance* privateData = reinterpret_cast<PluginInstance*> (instance->pdata);\
			CODE;\
		}\
	}


JRI_PUBLIC_API(jint)	native_NetLedIt_GetVersionNumber (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetVersionNumber (env));
	return 0;
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetShortVersionString (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetShortVersionString (env));
	return NULL;
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetDirty (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetDirty (env));
	return false;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetDirty (JRIEnv* env, struct NetLedIt* self, jbool dirty)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetDirty (env, dirty));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetHasVerticalScrollBar (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetHasVerticalScrollBar (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetHasVerticalScrollBar (JRIEnv* env, struct NetLedIt* self, jint scrollBarFlag)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetHasVerticalScrollBar (env, scrollBarFlag));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetHasHorizontalScrollBar (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetHasHorizontalScrollBar (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetHasHorizontalScrollBar (JRIEnv* env, struct NetLedIt* self, jint scrollBarFlag)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetHasHorizontalScrollBar (env, scrollBarFlag));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetMaxUndoLevel (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetMaxUndoLevel (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetMaxUndoLevel (JRIEnv* env, struct NetLedIt* self, jint maxUndoLevel)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetMaxUndoLevel (env, maxUndoLevel));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetCanUndo (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetCanUndo (env));
	return 0;
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetCanRedo (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetCanRedo (env));
	return 0;
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetText (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetText (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetText (JRIEnv* env, struct NetLedIt* self, java_lang_String* a)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetText (env, a));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetTextCRLF (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetTextCRLF (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetTextCRLF (JRIEnv* env, struct NetLedIt* self, java_lang_String* a)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetTextCRLF (env, a));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetTextRTF (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetTextRTF (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetTextRTF (JRIEnv* env, struct NetLedIt* self, java_lang_String* a)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetTextRTF (env, a));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetTextHTML (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetTextHTML (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetTextHTML (JRIEnv* env, struct NetLedIt* self, java_lang_String* a)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetTextHTML (env, a));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetLength (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetLength (env));
	return 0;
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetMaxLength (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetMaxLength (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetMaxLength (JRIEnv* env, struct NetLedIt* self, jint maxLength)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetMaxLength (env, maxLength));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetSupportContextMenu (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSupportContextMenu (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSupportContextMenu (JRIEnv* env, struct NetLedIt* self, jbool supportContextMenu)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSupportContextMenu (env, supportContextMenu));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetSmartCutAndPaste (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSmartCutAndPaste (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSmartCutAndPaste (JRIEnv* env, struct NetLedIt* self, jbool smartCutAndPaste)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSmartCutAndPaste (env, smartCutAndPaste));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetWrapToWindow (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetWrapToWindow (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetWrapToWindow (JRIEnv* env, struct NetLedIt* self, jbool wrapToWindow)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetWrapToWindow (env, wrapToWindow));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetShowParagraphGlyphs (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetShowParagraphGlyphs (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetShowParagraphGlyphs (JRIEnv* env, struct NetLedIt* self, jbool showParagraphGlyphs)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetShowParagraphGlyphs (env, showParagraphGlyphs));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetShowTabGlyphs (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetShowTabGlyphs (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetShowTabGlyphs (JRIEnv* env, struct NetLedIt* self, jbool showTabGlyphs)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetShowTabGlyphs (env, showTabGlyphs));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetShowSpaceGlyphs (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetShowSpaceGlyphs (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetShowSpaceGlyphs (JRIEnv* env, struct NetLedIt* self, jbool showSpaceGlyphs)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetShowSpaceGlyphs (env, showSpaceGlyphs));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetShowHidableText (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetShowHidableText (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetShowHidableText (JRIEnv* env, struct NetLedIt* self, jbool showHidableText)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetShowHidableText (env, showHidableText));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetHidableTextColor (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetHidableTextColor (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetHidableTextColor (JRIEnv* env, struct NetLedIt* self, jint hidableTextColor)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetHidableTextColor (env, hidableTextColor));
}

JRI_PUBLIC_API(jbool)	native_NetLedIt_GetHidableTextColored (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetHidableTextColored (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetHidableTextColored (JRIEnv* env, struct NetLedIt* self, jbool hidableTextColored)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetHidableTextColored (env, hidableTextColored));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelStart (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelStart (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelStart (JRIEnv* env, struct NetLedIt* self, jint selStart)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelStart (env, selStart));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelLength (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelLength (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelLength (JRIEnv* env, struct NetLedIt* self, jint selLength)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelLength (env, selLength));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetSelText (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelText (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelText (JRIEnv* env, struct NetLedIt* self, java_lang_String* selText)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelText (env, selText));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetSelTextRTF (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelTextRTF (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelTextRTF (JRIEnv* env, struct NetLedIt* self, java_lang_String* selTextRTF)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelTextRTF (env, selTextRTF));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetSelTextHTML (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelTextHTML (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelTextHTML (JRIEnv* env, struct NetLedIt* self, java_lang_String* selTextHTML)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelTextHTML (env, selTextHTML));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelColor (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelColor (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelColor (JRIEnv* env, struct NetLedIt* self, jint selColor)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelColor (env, selColor));
}

JRI_PUBLIC_API(java_lang_String*)	native_NetLedIt_GetSelFontFace (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelFontFace (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelFontFace (JRIEnv* env, struct NetLedIt* self, java_lang_String* selFontFace)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelFontFace (env, selFontFace));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelFontSize (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelFontSize (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelFontSize (JRIEnv* env, struct NetLedIt* self, jint selFontSize)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelFontSize (env, selFontSize));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelBold (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelBold (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelBold (JRIEnv* env, struct NetLedIt* self, jint selBold)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelBold (env, selBold));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelItalic (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelItalic (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelItalic (JRIEnv* env, struct NetLedIt* self, jint selItalic)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelItalic (env, selItalic));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelUnderline (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelUnderline (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelUnderline (JRIEnv* env, struct NetLedIt* self, jint selUnderline)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelUnderline (env, selUnderline));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelJustification (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelJustification (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelJustification (JRIEnv* env, struct NetLedIt* self, jint selJustification)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelJustification (env, selJustification));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetSelHidable (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetSelHidable (env));
	return 0;
}

JRI_PUBLIC_API(void)	native_NetLedIt_SetSelHidable (JRIEnv* env, struct NetLedIt* self, jint selHidable)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SetSelHidable (env, selHidable));
}

JRI_PUBLIC_API(void)	native_NetLedIt_AboutBox (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_AboutBox (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_LoadFile (JRIEnv* env, struct NetLedIt* self, java_lang_String* fileName)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_LoadFile (env, fileName));
}

JRI_PUBLIC_API(void)	native_NetLedIt_SaveFile (JRIEnv* env, struct NetLedIt* self, java_lang_String* fileName)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SaveFile (env, fileName));
}

JRI_PUBLIC_API(void)	native_NetLedIt_SaveFileCRLF (JRIEnv* env, struct NetLedIt* self, java_lang_String* fileName)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SaveFileCRLF (env, fileName));
}

JRI_PUBLIC_API(void)	native_NetLedIt_SaveFileRTF (JRIEnv* env, struct NetLedIt* self, java_lang_String* fileName)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SaveFileRTF (env, fileName));
}

JRI_PUBLIC_API(void)	native_NetLedIt_SaveFileHTML (JRIEnv* env, struct NetLedIt* self, java_lang_String* fileName)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_SaveFileHTML (env, fileName));
}

JRI_PUBLIC_API(void)	native_NetLedIt_Refresh (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_Refresh (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_ScrollToSelection (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_ScrollToSelection (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_Undo (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_Undo (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_Redo (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_Redo (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_CommitUndo (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_CommitUndo (env));
}

JRI_PUBLIC_API(void)	native_NetLedIt_LaunchFindDialog (JRIEnv* env, struct NetLedIt* self)
{
	JAVA_NATIVE_WRAPPER_GLOP (privateData->native_LaunchFindDialog (env));
}

JRI_PUBLIC_API(jint)	native_NetLedIt_Find (JRIEnv* env, struct NetLedIt* self, jint searchFrom, java_lang_String* findText, jbool wrapSearch, jbool wholeWordSearch, jbool caseSensativeSearch)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_Find (env, searchFrom, findText, wrapSearch, wholeWordSearch, caseSensativeSearch));
	return 0;
}

JRI_PUBLIC_API(jint)	native_NetLedIt_GetHeight (JRIEnv* env, struct NetLedIt* self, jint from, jint to)
{
	JAVA_NATIVE_WRAPPER_GLOP (return privateData->native_GetHeight (env, from, to));
	return 0;
}









/*******************************************************************************
 * SECTION 5 - Utility Method Implementations
 ******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++
 * DisplayJavaMessage
 *
 * This function is a utility routine that calls back into Java to print
 * messages to the Java Console and to stdout (via the native method,
 * native_NetLedIt_printToStdout, defined below).  Sure, it's not a very
 * interesting use of Java, but it gets the point across.
 +++++++++++++++++++++++++++++++++++++++++++++++++*/
static	void	DisplayJavaMessage (NPP instance, char* msg, int len)
{
	jref str, javaPeer;
	JRIEnv* env = NPN_GetJavaEnv();

	if (!env) { /* Java failed to initialize, so do nothing. */
		return;
	}
	if (len == -1) {
		len = strlen (msg);
	}
	/*
    ** Use the JRI (see jri.h) to create a Java string from the input
    ** message:
    */
	str = (jref) JRI_NewStringUTF(env, msg, len);	// DONT I NEED TO FREE THE STRING? I GUESS JAVA DOES THAT? - LGP 2000-09-22 - GC?

	/*
    ** Use the NPN_GetJavaPeer operation to get the Java instance that
    ** corresponds to our plug-in (an instance of the NetLedIt class):
    */
	javaPeer = NPN_GetJavaPeer (instance);
	
	/*
    ** Use the NedLedIT_PrintToStdOut () method to access the Java print code. Maybe a better, more direct way, but this is what
	** followed most trivially from the Netscape plugin sample code. -- LGP 2000-09-28
    */
	if (myJavaClass != NULL) {
		NetLedIt_PrintToStdOut (env, (NetLedIt*)javaPeer, (java_lang_String*)str);
	}
}






/*
 ********************************************************************************
 ********************************** PluginInstance ******************************
 ********************************************************************************
 */
PluginInstance::PluginInstance (NPP instance, uint16 mode)
{
	/* mode is NP_EMBED, NP_FULL, or NP_BACKGROUND (see npapi.h) */
	fWindow = NULL;
	fMode = mode;
	fEditor = NULL;
	fDataDirty = false;
	fNPPInstance = instance;

	fEditor = new LedItView ();
	fEditor->SetController (this);

#if		qWindows
	fhWnd = NULL;
	fDefaultWindowProc = NULL;
#elif	qXWindows
	window = 0;
#endif
	fTmpStreamWriteInput = string ();
}

PluginInstance::~PluginInstance ()
{
	Led_AssertNotNil (fEditor);
#if		qWindows
	if (fWindow != NULL) {
		/* If we have a window, clean it up. */
		SetWindowLong( fhWnd, GWL_WNDPROC, (LONG)fDefaultWindowProc);
		fDefaultWindowProc = NULL;
		fhWnd = NULL;
		Led_AssertNotNil (fEditor);
		fEditor->SetHWND (NULL);
	}
#endif
	fEditor->SetController (NULL);
	delete fEditor;
}

NPError	PluginInstance::Handle_SetWindow (NPWindow* window)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
#if		qWindows
	if( fWindow != NULL ) /* If we already have a window, clean
								 * it up before trying to subclass
								 * the new window. */
	{
		if( (window == NULL) || ( window->window == NULL ) ) {
			/* There is now no window to use. get rid of the old
			 * one and exit. */
			SetWindowLong( fhWnd, GWL_WNDPROC, (LONG)fDefaultWindowProc);
			fDefaultWindowProc = NULL;
			fhWnd = NULL;
			fEditor->SetHWND (fhWnd);
			fWindow = window;
			return NPERR_NO_ERROR;
		}
		else if ( fhWnd == (HWND) window->window ) {
			/* The new window is the same as the old one. Redraw and get out. */

			InvalidateRect( fhWnd, NULL, TRUE );

			UpdateWindow( fhWnd );

			fEditor->SetHWND (fhWnd);

			fWindow = window;

			Led_Rect	r	=	Led_Rect (0, 0, fWindow->height, fWindow->width);
			static_cast<TextImager*>(fEditor)->SetWindowRect (r);

			return NPERR_NO_ERROR;
		}
		else {
			/* Clean up the old window, so that we can subclass the new
			 * one later. */
			SetWindowLong( fhWnd, GWL_WNDPROC, (LONG)fDefaultWindowProc);
			fDefaultWindowProc = NULL;
			fhWnd = NULL;
			fEditor->SetHWND (fhWnd);
		}
	}
	else if( (window == NULL) || ( window->window == NULL ) ) {
		/* We can just get out of here if there is no current
		 * window and there is no new window to use. */
		fEditor->SetHWND (NULL);
		fWindow = window;
		return NPERR_NO_ERROR;
	}

	/* At this point, we will subclass
	 * window->window so that we can begin drawing and
	 * receiving window messages. */
	fDefaultWindowProc = (WNDPROC)SetWindowLong( (HWND)window->window, GWL_WNDPROC, (LONG)MyBase::StaticWndProc);
	fhWnd = (HWND) window->window;
	fEditor->SetHWND (fhWnd);

	InvalidateRect( fhWnd, NULL, TRUE );
	UpdateWindow( fhWnd );
#elif	qMacOS
	Led_MacPortAndClipRegionEtcSaver	saver;		// can cause SetPort calls...
	fWindow = window;

	if (window != NULL and window->window != NULL) {
		fEditor->SetUseGrafPort (reinterpret_cast<GrafPtr> (reinterpret_cast<NP_Port*> (window->window)->port));
	}
	else {
		fEditor->SetUseGrafPort (NULL);
	}
#if 1
	if (fEditor != NULL and fWindow != NULL) {
		fEditor->SetWindowFrameRect (Led_Rect (fWindow->y, fWindow->x, fWindow->height, fWindow->width));
	}
#else
	NP_Port* port = (NP_Port*) fWindow->window;
	Led_Rect	r	=	Led_Rect (-port->porty, -port->portx, fWindow->height, fWindow->width);
	fEditor->SetWindowFrameRect (r);
#endif
	/*
	 *		For V&H scrollbars, they may not have been able to be created til now - so re-call SetHasScrollbar() to get them created.
	 */
	fEditor->SetScrollBarType (fEditor->v, fEditor->GetScrollBarType (fEditor->v));
	fEditor->SetScrollBarType (fEditor->h, fEditor->GetScrollBarType (fEditor->h));

#elif	qXWindows
	Widget netscape_widget;

	window = (Window) window->window;
	x = window->x;
	y = window->y;
	width = window->width;
	height = window->height;
	display = ((NPSetWindowCallbackStruct *)window->ws_info)->display;

    netscape_widget = XtWindowToWidget(This->display, This->window);
	XtAddEventHandler(netscape_widget, ExposureMask, FALSE, (XtEventHandler)Redraw, This);
	Redraw (netscape_widget, (XtPointer)This, NULL);
#endif
	fWindow = window;
	return NPERR_NO_ERROR;
}

int16	PluginInstance::Handle_HandleEvent (void* event)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
#if		qMacOS
	int16 eventHandled = FALSE;

	// We don't seem to be notified of scrolling explicitly, so we do this to assure our window rect is in sync.
	if (fEditor != NULL and fWindow != NULL) {
		fEditor->SetWindowFrameRect (Led_Rect (fWindow->y, fWindow->x, fWindow->height, fWindow->width));
	}

	EventRecord* ev = (EventRecord*) event;
	if (event != NULL)
	{
		switch (ev->what)
		{
			/*
			 * Draw ourselves on update events
			 */
			case updateEvt: {
				if (fWindow != NULL) {
					NP_Port*	port = (NP_Port*) fWindow->window;
					WindowRef	macWin	=	reinterpret_cast<WindowRef> (port->port);
					WindowPeek	macWinP	=	reinterpret_cast<WindowPeek> (macWin);
					
					try {
						Rect	clipRect;
						clipRect.top = fWindow->clipRect.top ;
						clipRect.left = fWindow->clipRect.left ;
						clipRect.bottom = fWindow->clipRect.bottom ;
						clipRect.right = fWindow->clipRect.right;
						fEditor->SetupCurrentGrafPort ();
						::ClipRect (&clipRect);

						RgnHandle	updateRgn	=	macWin->visRgn;
				//		RgnHandle	updateRgn	=	::NewRgn ();
			//MTPHACK - AND CATCH SO CAN CLEAUP!!!
						//::SetRectRgn (updateRgn, (short)clipRect.left, (short)clipRect.top, (short)clipRect.right, (short)clipRect.bottom);
						fEditor->Handle_Draw (updateRgn);
						// Slightly risky - but eliminates Mac flicker. Really should be better way - as Netscape plugin API should give us some other
						// way to triger draw without their erasing background!!! 
						// May need to be more careful about assuring THIS rect were using is really our drawing rectangle!!! - LGP 2000-09-28
						//::ValidRgn (updateRgn);
						//::DisposeRgn (updateRgn);	
						
					}
					catch (...) {
						// ingore - cuz no much we can do...
					}
					::SetOrigin (port->portx,port->porty);
				}
				eventHandled = true;
			}
			break;

			case	autoKey:
			case	keyDown: {
				Led_MacPortAndClipRegionEtcSaver	saver;
				if (fEditor != NULL) {
					eventHandled = fEditor->HandleKeyEvent (*ev);
				}
			}
			break;

			case	getFocusEvent: {
				Led_MacPortAndClipRegionEtcSaver	saver;
				fEditor->HandleActivateEvent ();
				eventHandled = true;
			}
			break;

			case	loseFocusEvent: {
				Led_MacPortAndClipRegionEtcSaver	saver;
				fEditor->HandleDeactivateEvent ();
				eventHandled = true;
			}
			break;

			case	adjustCursorEvent: {
				Led_MacPortAndClipRegionEtcSaver	saver;
				fEditor->HandleAdjustCursorEvent (*ev);
				eventHandled = true;
			}
			break;

			case	mouseDown: {
				Led_MacPortAndClipRegionEtcSaver	saver;
				fEditor->HandleMouseDownEvent (*ev);
				eventHandled = true;
			}
			break;

			default: {
				// find idle time message
				sIdleMangerSetter.fIdleManagerOSImpl.SpendTime ();
				Led_MacPortAndClipRegionEtcSaver	saver;
				fEditor->HandleIdleEvent ();
			}
				break;
		}
	}
	return eventHandled;
#else
	/* Windows Plugins use the Windows event call-back mechanism
	   for events. (See PluginWindowProc) */
	/* UNIX Plugins do not use HandleEvent */
	return 0;
#endif
}

void	PluginInstance::Handle_WritePartial (const string& input)
{
	fTmpStreamWriteInput += input;
}

void	PluginInstance::Handle_WriteCompete ()
{
	PRESERVE_NETSCAPE_CONTEXT;
	// For now - ASSUME input is RTF - later try guessing a bit - like we do for fileIO... LGP 2000-10-04
	try {
// NEED THIS BACK WHEN WE COPY LEDITIVIEW STUFF		fCommandHandler.Commit ();

		string											tmp		=			fTmpStreamWriteInput;
		fTmpStreamWriteInput.erase ();
		StyledTextIOSrcStream_Memory					source (tmp.c_str (), tmp.length ());
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor);
		StyledTextIOReader_RTF							textReader (&source, &sink);
		fEditor->Replace (0, fEditor->GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

#if		qXWindows
void	PluginInstance::Redraw (Widget w, XtPointer closure, XEvent *event)
{
	PluginInstance* This = (PluginInstance*)closure;
	GC gc;
	XGCValues gcv;
	size_t	len	=		fTextStore.GetLength ();
	Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
	fTextStore.CopyOut (0, len, buf);
	buf[len]=0;
	const char* text = buf;

	XtVaGetValues(w, XtNbackground, &gcv.background, XtNforeground, &gcv.foreground, 0);
	gc = XCreateGC(This->display, This->window, GCForeground|GCBackground, &gcv);
	XDrawRectangle(This->display, This->window, gc, 0, 0, This->width-1, This->height-1);
	XDrawString(This->display, This->window, gc, This->width/2 - 100, This->height/2, text, strlen(text));
}
#endif

#if		qMacOS
void	PluginInstance::DoFocus ()
{
	if (fWindow != NULL) {
		NP_Port*	port = (NP_Port*) fWindow->window;
		::SetPort((GrafPtr)port->port);
		/* Setup our drawing environment */
		::SetOrigin (0,0);
	}
}
#endif

JRI_PUBLIC_API(jint)	PluginInstance::native_GetVersionNumber (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return kNetLedItDWORDVersion;
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetShortVersionString (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		string	result	=	qLed_ShortVersionString + string (kDemoString);
		return JRI_NewStringUTF (env, result.c_str (), result.length ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return NULL;
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetDirty (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return fDataDirty;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetDirty (JRIEnv* /*env*/, jbool dirty)
{
	PRESERVE_NETSCAPE_CONTEXT;
	fDataDirty = dirty;
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetHasVerticalScrollBar (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	switch (fEditor->GetScrollBarType (fEditor->v)) {
		case	LedItView::eScrollBarNever:		return	NetLedIt_eNoScrollBar;
		case	LedItView::eScrollBarAsNeeded:	return	NetLedIt_eShowScrollbarIfNeeded;
		case	LedItView::eScrollBarAlways:	return	NetLedIt_eShowScrollBar;
	}
	return NetLedIt_eNoScrollBar;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetHasVerticalScrollBar (JRIEnv* /*env*/, jint scrollBarFlag)
{
	PRESERVE_NETSCAPE_CONTEXT;
	switch (scrollBarFlag) {
		case	NetLedIt_eNoScrollBar:				fEditor->SetScrollBarType (LedItView::v, LedItView::eScrollBarNever); break;
		case	NetLedIt_eShowScrollBar:			fEditor->SetScrollBarType (LedItView::v, LedItView::eScrollBarAlways); break;
		case	NetLedIt_eShowScrollbarIfNeeded:	fEditor->SetScrollBarType (LedItView::v, LedItView::eScrollBarAsNeeded); break;
	}
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetHasHorizontalScrollBar (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	switch (fEditor->GetScrollBarType (fEditor->h)) {
		case	LedItView::eScrollBarNever:		return	NetLedIt_eNoScrollBar;
		case	LedItView::eScrollBarAsNeeded:	return	NetLedIt_eShowScrollbarIfNeeded;
		case	LedItView::eScrollBarAlways:	return	NetLedIt_eShowScrollBar;
	}
	return NetLedIt_eNoScrollBar;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetHasHorizontalScrollBar (JRIEnv* /*env*/, jint scrollBarFlag)
{
	PRESERVE_NETSCAPE_CONTEXT;
	switch (scrollBarFlag) {
		case	NetLedIt_eNoScrollBar:				fEditor->SetScrollBarType (LedItView::h, LedItView::eScrollBarNever); break;
		case	NetLedIt_eShowScrollBar:			fEditor->SetScrollBarType (LedItView::h, LedItView::eScrollBarAlways); break;
		case	NetLedIt_eShowScrollbarIfNeeded:	fEditor->SetScrollBarType (LedItView::h, LedItView::eScrollBarAsNeeded); break;
	}
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetMaxUndoLevel (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return fCommandHandler.GetMaxUnDoLevels ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetMaxUndoLevel (JRIEnv* /*env*/, jint maxUndoLevel)
{
	PRESERVE_NETSCAPE_CONTEXT;
	// sanity check arguments
	maxUndoLevel = max (0, static_cast<int> (maxUndoLevel));
	maxUndoLevel = min (static_cast<int> (maxUndoLevel), 10);
	fCommandHandler.SetMaxUnDoLevels (maxUndoLevel);
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetCanUndo (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return fCommandHandler.CanUndo () or (fCommandHandler.GetMaxUnDoLevels ()==1 and fCommandHandler.CanRedo ());
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetCanRedo (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return fCommandHandler.CanRedo ();
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetText (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		size_t	len	=		fEditor->GetLength ();
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		fEditor->CopyOut (0, len, buf);
		buf[len] = '\0';
		return JRI_NewStringUTF (env, buf, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetText (JRIEnv* env, java_lang_String* a)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	#if		qMacOS
		Led_MacPortAndClipRegionEtcSaver	saver;		// can cause SetPort calls...
	#endif
	const char* text = JRI_GetStringUTFChars (env, a);
	try {
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::strlen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor);
		StyledTextIOReader_PlainText					textReader (&source, &sink);
		fEditor->Replace (0, fEditor->GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetTextCRLF (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		size_t	len	=		fEditor->GetLength ();
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		fEditor->CopyOut (0, len, buf);
		buf[len] = '\0';
		Led_SmallStackBuffer<Led_tChar>	buf2 (2*len + 1);
		len = Led_NLToNative (buf, len, buf2, 2*len + 1);
		buf2[len] = '\0';
		return JRI_NewStringUTF (env, buf2, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return NULL;	// cuz we dont know how (now) to really throw java-safe exception...
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetTextCRLF (JRIEnv* env, java_lang_String* a)
{
	PRESERVE_NETSCAPE_CONTEXT;
	native_SetText (env, a);
}

string	PluginInstance::GetBufferTextAsRTF_ () 
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	WordProcessor::WordProcessorTextIOSrcStream	source (fEditor);
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_RTF						textWriter (&source, &sink);
	textWriter.Write ();
	size_t	len	=	sink.GetLength ();
	Led_SmallStackBuffer<char>	buf (len + 1);
	memcpy (buf, sink.PeekAtData (), len);
	buf[len] = '\0';
	return string (static_cast<char*> (buf));
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetTextRTF (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	try {
		string	tmp	=	GetBufferTextAsRTF_ ();
		return JRI_NewStringUTF (env, tmp.c_str (), tmp.length ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return NULL;	// cuz we dont know how (now) to really throw java-safe exception...
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetTextRTF (JRIEnv* env, java_lang_String* a)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	#if		qMacOS
		Led_MacPortAndClipRegionEtcSaver	saver;		// can cause SetPort calls...
	#endif
	const char* text = JRI_GetStringUTFChars (env, a);
	try {
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::strlen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor);
		StyledTextIOReader_RTF							textReader (&source, &sink);
		fEditor->Replace (0, fEditor->GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetTextHTML (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (fEditor);
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_HTML						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return JRI_NewStringUTF (env, buf, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS ();
	return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetTextHTML (JRIEnv* env, java_lang_String* a)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	#if		qMacOS
		Led_MacPortAndClipRegionEtcSaver	saver;		// can cause SetPort calls...
	#endif
	const char* text = JRI_GetStringUTFChars (env, a);
	try {
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::strlen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor);
		StyledTextIOReader_HTML							textReader (&source, &sink);
		fTextStore.Replace (0, fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetLength (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	return fTextStore.GetLength ();
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetMaxLength (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetMaxLength ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetMaxLength (JRIEnv* /*env*/, jint maxLength)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fEditor->SetMaxLength (maxLength < 0? -1: maxLength);
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetSupportContextMenu (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetSupportContextMenu ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSupportContextMenu (JRIEnv* /*env*/, jbool supportContextMenu)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!supportContextMenu != fEditor->GetSupportContextMenu ()) {
		fEditor->SetSupportContextMenu (supportContextMenu);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetSmartCutAndPaste (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetSmartCutAndPasteMode ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSmartCutAndPaste (JRIEnv* /*env*/, jbool smartCutAndPaste)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!smartCutAndPaste != fEditor->GetSmartCutAndPasteMode ()) {
		fEditor->SetSmartCutAndPasteMode (smartCutAndPaste);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetWrapToWindow (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetWrapToWindow ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetWrapToWindow (JRIEnv* /*env*/, jbool wrapToWindow)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!wrapToWindow != fEditor->GetWrapToWindow ()) {
		fEditor->SetWrapToWindow (wrapToWindow);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetShowParagraphGlyphs (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetShowParagraphGlyphs ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetShowParagraphGlyphs (JRIEnv* /*env*/, jbool showParagraphGlyphs)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!showParagraphGlyphs != fEditor->GetShowParagraphGlyphs ()) {
		fEditor->SetShowParagraphGlyphs (showParagraphGlyphs);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetShowTabGlyphs (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetShowTabGlyphs ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetShowTabGlyphs (JRIEnv* /*env*/, jbool showTabGlyphs)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!showTabGlyphs != fEditor->GetShowTabGlyphs ()) {
		fEditor->SetShowTabGlyphs (showTabGlyphs);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetShowSpaceGlyphs (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetShowSpaceGlyphs ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetShowSpaceGlyphs (JRIEnv* /*env*/, jbool showSpaceGlyphs)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (!!showSpaceGlyphs != fEditor->GetShowSpaceGlyphs ()) {
		fEditor->SetShowSpaceGlyphs (showSpaceGlyphs);
	}
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetShowHidableText (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor->GetHidableTextDatabase ()));
	Led_AssertNotNil (uhtmo);
	return !uhtmo->IsHidden ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetShowHidableText (JRIEnv* /*env*/, jbool showHidableText)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (showHidableText) {
		fEditor->GetHidableTextDatabase ()->ShowAll ();
	}
	else {
		fEditor->GetHidableTextDatabase ()->HideAll ();
	}
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetHidableTextColor (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor->GetHidableTextDatabase ()));
	Led_AssertNotNil (uhtmo);
	Led_Color	c	=	uhtmo->GetColor ();
	unsigned	char	r	=	c.GetRed () >> 8;
	unsigned	char	g	=	c.GetGreen () >> 8;
	unsigned	char	b	=	c.GetBlue () >> 8;
	unsigned			result	=	r + (unsigned (g) << 8) + (unsigned (b) << 16);
	#if		qWindows
		Led_Assert (result == c.GetOSRep ());
	#endif
	return result;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetHidableTextColor (JRIEnv* /*env*/, jint hidableTextColor)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(jbool)	PluginInstance::native_GetHidableTextColored (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	return 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetHidableTextColored (JRIEnv* /*env*/, jbool hidableTextColored)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelStart (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	return fEditor->GetSelectionStart ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelStart (JRIEnv* /*env*/, jint selStart)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	s;
	size_t	e;
	fEditor->GetSelection (&s, &e);
	size_t	l	=	e-s;
	size_t	bufLen	=	fEditor->GetLength ();
	size_t	newStart = Led_Min (static_cast<size_t> (selStart), bufLen);
	size_t	newEnd = Led_Min (newStart + l, bufLen);
	fEditor->SetSelection (newStart, newEnd);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelLength (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	s;
	size_t	e;
	fEditor->GetSelection (&s, &e);
	return e-s;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelLength (JRIEnv* /*env*/, jint selLength)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (selLength == -1) {
		selLength = INT_MAX;
	}
	if (selLength < 0) {
		selLength = 0;
	}
	size_t	s;
	size_t	e;
	fEditor->GetSelection (&s, &e);
	size_t	l	=	e-s;
	size_t	bufLen	=	fEditor->GetLength ();
	size_t	newEnd = Led_Min (s + selLength, bufLen);
	fEditor->SetSelection (s, newEnd);
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetSelText (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		size_t	s;
		size_t	e;
		fEditor->GetSelection (&s, &e);
		size_t	len	=		e-s;
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		fEditor->CopyOut (s, len, buf);
		buf[len] = '\0';
		return JRI_NewStringUTF (env, buf, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelText (JRIEnv* env, java_lang_String* selText)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	const char* text = JRI_GetStringUTFChars (env, selText);
	try {
		size_t	len	=	::strlen (text);
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		len = Led_NativeToNL (Led_SDKString2tString (text).c_str (), len, buf, len + 1);
		size_t	s;
		size_t	e;
		fEditor->GetSelection (&s, &e);
		fEditor->Replace (s, e, buf, len);
		if (s != e) {
			fEditor->SetSelection (s, s + len);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetSelTextRTF (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (fEditor, fEditor->GetSelectionStart (), fEditor->GetSelectionEnd ());
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_RTF						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return JRI_NewStringUTF (env, buf, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelTextRTF (JRIEnv* env, java_lang_String* selTextRTF)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	const char* text = JRI_GetStringUTFChars (env, selTextRTF);
	try {
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::strlen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor, fEditor->GetSelectionStart ());
		StyledTextIOReader_RTF							textReader (&source, &sink);
		fEditor->Replace (fEditor->GetSelectionStart (), fEditor->GetSelectionEnd (), LED_TCHAR_OF (""), 0);
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();

	// SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetSelTextHTML (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (fEditor, fEditor->GetSelectionStart (), fEditor->GetSelectionEnd ());
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_HTML						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return JRI_NewStringUTF (env, buf, len);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelTextHTML (JRIEnv* env, java_lang_String* selTextHTML)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	const char* text = JRI_GetStringUTFChars (env, selTextHTML);
	try {
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::strlen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (fEditor, fEditor->GetSelectionStart ());
		StyledTextIOReader_HTML							textReader (&source, &sink);
		fEditor->Replace (fEditor->GetSelectionStart (), fEditor->GetSelectionEnd (), LED_TCHAR_OF (""), 0);
		textReader.Read ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();

	// SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelColor (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		Led_Assert (fEditor->GetSelectionEnd () >= fEditor->GetSelectionStart ());
		size_t	selectionLength	=	fEditor->GetSelectionEnd () - fEditor->GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (fEditor->GetSelectionStart (), selectionLength);
		if (not fsp.GetTextColor_Valid ()) {
			// HACK - really should throw an error - but now sure how???
			fsp = fEditor->GetContinuousStyleInfo (fEditor->GetSelectionStart (), 0);
			Led_Assert (fsp.GetTextColor_Valid ());
		}
		#define mkRGB(r,g,b)          ((long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
		jint	result	=	mkRGB (fsp.GetTextColor ().GetRed () >> 8, fsp.GetTextColor ().GetGreen () >> 8, fsp.GetTextColor ().GetBlue () >> 8);
		#if		qWindows
			Led_Ensure (result == fsp.GetTextColor ().GetOSRep ());
		#endif
		return result;
///		return fsp.GetTextColor ().GetOSRep ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelColor (JRIEnv* /*env*/, jint selColor)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fCommandHandler.Commit ();
	Led_IncrementalFontSpecification	applyFontSpec;
	#define MyGetRValue(rgb)      ((unsigned char)(rgb))
	#define MyGetGValue(rgb)      ((unsigned char)(((unsigned short)(rgb)) >> 8))
	#define MyGetBValue(rgb)      ((unsigned char)((rgb)>>16))
	Led_Color							useColor		=	Led_Color (MyGetRValue (selColor) << 8, MyGetGValue (selColor) << 8, MyGetBValue (selColor) << 8);
	#if		qWindows
		Led_Assert (useColor == Led_Color (selColor));
	#endif
	applyFontSpec.SetTextColor (useColor);
	fEditor->InteractiveSetFont (applyFontSpec);
}

JRI_PUBLIC_API(java_lang_String*)	PluginInstance::native_GetSelFontFace (JRIEnv* env)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		Led_Assert (fEditor->GetSelectionEnd () >= fEditor->GetSelectionStart ());
		size_t	selectionLength	=	fEditor->GetSelectionEnd () - fEditor->GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (fEditor->GetSelectionStart (), selectionLength);
		if (not fsp.GetFontNameSpecifier_Valid ()) {
			return JRI_NewStringUTF (env, "", 0);
		}
		return JRI_NewStringUTF (env, fsp.GetFontName ().c_str (), fsp.GetFontName ().length ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return NULL;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelFontFace (JRIEnv* env, java_lang_String* selFontFace)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		const char* fontFaceStr = JRI_GetStringUTFChars (env, selFontFace);
		applyFontSpec.SetFontName (fontFaceStr);
		fEditor->InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelFontSize (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	try {
		Led_Assert (fEditor->GetSelectionEnd () >= fEditor->GetSelectionStart ());
		size_t	selectionLength	=	fEditor->GetSelectionEnd () - fEditor->GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (fEditor->GetSelectionStart (), selectionLength);
		if (not fsp.GetPointSize_Valid ()) {
			return 0;
		}
		return fsp.GetPointSize ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelFontSize (JRIEnv* /*env*/, jint selFontSize)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fCommandHandler.Commit ();
	Led_IncrementalFontSpecification	applyFontSpec;
	int	size	=	selFontSize;
	if (size <= 0) {
		size = 2;	// minsize?
	}
	if (size >= 128) {
		size = 128;
	}
	applyFontSpec.SetPointSize (size);
	fEditor->InteractiveSetFont (applyFontSpec);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelBold (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	size_t	selectionLength	=	selEnd - selStart;
	Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (selStart, selectionLength);
	if (not fsp.GetStyle_Bold_Valid ()) {
		return 2;
	}
	return fsp.GetStyle_Bold ()? 1: 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelBold (JRIEnv* /*env*/, jint selBold)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fCommandHandler.Commit ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Bold (selBold);
	fEditor->InteractiveSetFont (applyFontSpec);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelItalic (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	size_t	selectionLength	=	selEnd - selStart;
	Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (selStart, selectionLength);
	if (not fsp.GetStyle_Italic_Valid ()) {
		return 2;
	}
	return fsp.GetStyle_Italic ()? 1: 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelItalic (JRIEnv* /*env*/, jint selItalic)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fCommandHandler.Commit ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Italic (selItalic);
	fEditor->InteractiveSetFont (applyFontSpec);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelUnderline (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	size_t	selectionLength	=	selEnd - selStart;
	Led_IncrementalFontSpecification	fsp = fEditor->GetContinuousStyleInfo (selStart, selectionLength);
	if (not fsp.GetStyle_Underline_Valid ()) {
		return 2;
	}
	return fsp.GetStyle_Underline ()? 1: 0;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelUnderline (JRIEnv* /*env*/, jint selUnderline)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fCommandHandler.Commit ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Underline (selUnderline);
	fEditor->InteractiveSetFont (applyFontSpec);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelJustification (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	Led_Justification	justification	=	eLeftJustify;
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	if (fEditor->GetJustification (selStart, selEnd, &justification)) {
		switch (justification) {
			case	eLeftJustify:	return NetLedIt_eLeftJustification;
			case	eRightJustify:	return NetLedIt_eRightJustification;
			case	eCenterJustify:	return NetLedIt_eCenterJustification;
			case	eFullyJustify:	return NetLedIt_eFullJustification;
			default:				return NetLedIt_eLeftJustification;	// what should we return here?
		}
	}
	else {
		return NetLedIt_eNoCommonJustification;
	}
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelJustification (JRIEnv* /*env*/, jint selJustification)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	Led_Justification	lh	=	eLeftJustify;
	switch (selJustification) {
		case	NetLedIt_eLeftJustification:	lh = eLeftJustify; break;
		case	NetLedIt_eCenterJustification:	lh = eCenterJustify; break;
		case	NetLedIt_eRightJustification:	lh = eRightJustify; break;
		case	NetLedIt_eFullJustification:	lh = eFullyJustify; break;
		default:	return;	// should be an error - but no error reporting supported here- should return E_INVALIDARG;
	}
	fEditor->InteractiveSetJustification (lh);
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetSelHidable (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	if (fEditor->GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, true)) {
		return true;
	}
	else if (fEditor->GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, false)) {
		return false;
	}
	return 2;
}

JRI_PUBLIC_API(void)	PluginInstance::native_SetSelHidable (JRIEnv* /*env*/, jint selHidable)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	size_t	selStart;
	size_t	selEnd;
	fEditor->GetSelection (&selStart, &selEnd);
	Led_Assert (selStart <= selEnd);
	if (selHidable) {
		fEditor->GetHidableTextDatabase ()->MakeRegionHidable (selStart, selEnd);
	}
	else {
		fEditor->GetHidableTextDatabase ()->MakeRegionUnHidable (selStart, selEnd);
	}
}

JRI_PUBLIC_API(void)	PluginInstance::native_AboutBox (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	OnAboutBoxCommand ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_LoadFile (JRIEnv* /*env*/, java_lang_String* fileName)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(void)	PluginInstance::native_SaveFile (JRIEnv* /*env*/, java_lang_String* fileName)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(void)	PluginInstance::native_SaveFileCRLF (JRIEnv* /*env*/, java_lang_String* fileName)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(void)	PluginInstance::native_SaveFileRTF (JRIEnv* /*env*/, java_lang_String* fileName)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(void)	PluginInstance::native_SaveFileHTML (JRIEnv* /*env*/, java_lang_String* fileName)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	//tmphack
	Led_Assert (false);		//NYI
}

JRI_PUBLIC_API(void)	PluginInstance::native_Refresh (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fEditor->Refresh ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_ScrollToSelection (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fEditor->ScrollToSelection ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_Undo (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (fCommandHandler.CanUndo ()) {
		fCommandHandler.DoUndo (*fEditor);
	}
	else if (fCommandHandler.GetMaxUnDoLevels ()==1 and fCommandHandler.CanRedo ()) {
		fCommandHandler.DoRedo (*fEditor);
	}
	else {
		// ignore bad undo request - no ability to return errors here til we redo this in ATL
	}
}

JRI_PUBLIC_API(void)	PluginInstance::native_Redo (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	if (fCommandHandler.CanRedo ()) {
		fCommandHandler.DoRedo (*fEditor);
	}
	else {
		// ignore bad undo request - no ability to return errors here til we redo this in ATL
	}
}

JRI_PUBLIC_API(void)	PluginInstance::native_CommitUndo (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	fCommandHandler.Commit ();
}

JRI_PUBLIC_API(void)	PluginInstance::native_LaunchFindDialog (JRIEnv* /*env*/)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	fEditor->OnFindCommand ();
}

JRI_PUBLIC_API(jint)	PluginInstance::native_Find (JRIEnv* env, jint searchFrom, java_lang_String* findText, jbool wrapSearch, jbool wholeWordSearch, jbool caseSensativeSearch)
{
	PRESERVE_NETSCAPE_CONTEXT;
	Led_AssertNotNil (fEditor);
	const char* utfFindText = JRI_GetStringUTFChars (env, findText);
	try {
		// don't have any better error checking technology here - should return E_INVALIDARG!!!
		if (searchFrom < 0) {
			return -1;
		}
		if (static_cast<size_t> (searchFrom) > fTextStore.GetEnd ()) {
			return -1;
		}
		TextStore::SearchParameters	parameters;

		#if		qWideCharacters
			parameters.fMatchString = findText.bstrVal;
		#else
			parameters.fMatchString = utfFindText;
		#endif
		parameters.fWrapSearch = wrapSearch;
		parameters.fWholeWordSearch = wholeWordSearch;
		parameters.fCaseSensativeSearch = caseSensativeSearch;

		size_t	whereTo	=	fTextStore.Find (parameters, searchFrom);

		return whereTo;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return -1;
}

JRI_PUBLIC_API(jint)	PluginInstance::native_GetHeight (JRIEnv* /*env*/, jint from, jint to)
{
	typedef	MultiRowTextImager::RowReference	RowReference;
	if (from < 0) {
		from = 0;
	}
	if (to < 0) {
		to = fTextStore.GetEnd ();
	}
	if (from > to) {
		// throw invalid input
		return -1;
	}
	if (size_t (to) > fTextStore.GetEnd ()) {
		// throw invalid input
		return -1;
	}
	RowReference	startingRow	=	fEditor->GetRowReferenceContainingPosition (from);
	RowReference	endingRow	=	fEditor->GetRowReferenceContainingPosition (to);
	/*
	 *	Always take one more row than they asked for, since they will expect if you start and end on a given row - you'll get
	 *	the height of that row.
	 */
	return fEditor->GetHeightOfRows (startingRow, fEditor->CountRowDifference (startingRow, endingRow) + 1);
}
