/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/SpelledIt/ScavangeWordsFromNet/ScavangeWordsFromNet.cpp,v 1.3 2003/06/19 13:53:52 lewis Exp $
 *
 * Changes:
 *	$Log: ScavangeWordsFromNet.cpp,v $
 *	Revision 1.3  2003/06/19 13:53:52  lewis
 *	SPR#1536: now the tool to scavange words from the net works pretty well. Could use improvements on filtering out other languages - but that doesn't appear very easy at this point (tricks to check charset and lang flags haven't worked out)
 *	
 *	Revision 1.2  2003/06/18 14:20:57  lewis
 *	cleanups - lose alot of code I wasn't using (but maybe useful in the future as code
 *	framgments - like code to fetch URLs using ServerXMLHTTPRequest)
 *	
 *	
 *	
 *
 *
 *
 */
#include	"ScavangerConfig.h"

#include	<mshtml.h>
#include	<exdisp.h>
#include	<exdispid.h>

#include	<map>
#include	<set>
#include	<vector>
#include	<string>
#include	<ctime>
#include	<cstdlib>

#include	<atlhost.h>
#include	<atlwin.h>

#include	"CodePage.h"
#include	"SpelledItDictionaryUtils.h"
#include	"TextBreaks.h"

#include	"resource.h"

#include	"ScavangeWordsFromNet_IDL.h"




#if		qLedUsesNamespaces
	using namespace	Led;
	using namespace	Led::SpelledIt;
#endif





namespace {

	const	int	kTimeout	=	100;	// milliseconds


	inline	void	ThrowIfError (HRESULT hr)
		{
			Led_ThrowIfErrorHRESULT (hr);
		}

	bool	ScanForWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
											const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
										)
		{
			TextBreaks_DefaultImpl	breaker;
			Led_RequireNotNil (startBuf);
			Led_RequireNotNil (endBuf);
			Led_RequireNotNil (cursor);
			Led_RequireNotNil (wordStartResult);
			Led_RequireNotNil (wordEndResult);
			Led_Require ((*cursor >= startBuf and *cursor <= endBuf));

			if (*cursor >= endBuf) {
				return false;
			}

			size_t	bufLen		=	endBuf-startBuf;

			size_t	initialCrs	=	*cursor-startBuf;
			size_t	p			=	initialCrs;
			size_t	wordStart	=	0;
			size_t	wordEnd		=	0;
			bool	wordReal	=	false;
			// Find a real word...
			while (not wordReal or wordStart < initialCrs) {
				breaker.FindWordBreaks (startBuf, bufLen, p, &wordStart, &wordEnd, &wordReal);
				if (not wordReal or wordStart < initialCrs) {
					if (p < bufLen) {
						p = Led_NextChar (&startBuf[p]) - startBuf;
						wordStart = p;
						wordEnd = p;
					}
					else {
						wordStart = p;
						wordEnd = p;
						break;
					}
				}
			}

			if (not wordReal) {
				// no real word to be found...
				//maybe the first 'word' was a series of spaces and there may not have been a word after (end of buf).
				*cursor = startBuf + wordEnd;
				return false;
			}

			Led_Assert (wordReal);
			Led_Assert (wordStart < wordEnd);
			p = wordEnd;

			Led_Assert (*cursor <= startBuf + p);
			Led_Assert ((*cursor < startBuf + p) or (*cursor == endBuf));
			*cursor = startBuf + p;

			*wordStartResult = startBuf + wordStart;
			*wordEndResult = startBuf + wordEnd;

			return true;
		}
}





// Child window class that will be subclassed for hosting Active X control
class CChildWindow : public CWindowImpl<CChildWindow> {
	public:
	BEGIN_MSG_MAP(CChildWindow)
	END_MSG_MAP()
};


// Helper Macro for Main Frame window class
#define DECLARE_MAINFRAME_WND_CLASS(WndClassName, style, bkgnd, menuid) \
	static ATL::CWndClassInfo& GetWndClassInfo() \
	{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), style, StartWindowProc, \
			0, 0, NULL, NULL, NULL, (HBRUSH)(bkgnd + 1), menuid, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	return wc; \
	}

class CMainWindow :
	public CWindowImpl<CMainWindow, CWindow, CFrameWinTraits>,
	public IDispEventImpl<1, CMainWindow, &__uuidof(DWebBrowserEvents2), &LIBID_SHDocVw, 1, 1>
	{
	private:
		typedef	CWindowImpl<CMainWindow, CWindow, CFrameWinTraits>	Win_inherited;
	public :
		CChildWindow m_wndChild;

	public :
		DECLARE_MAINFRAME_WND_CLASS("MainWindow", CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_WINDOW, 0)

	public :
		BEGIN_MSG_MAP(CMainWindow)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)
			MESSAGE_HANDLER(WM_SIZE, OnSize)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		END_MSG_MAP()

	public :
		BEGIN_SINK_MAP(CMainWindow)
			SINK_ENTRY_EX(1, __uuidof(DWebBrowserEvents2), DISPID_DOWNLOADCOMPLETE, OnDownloadComplete)
		END_SINK_MAP()

	private:
		enum { kTimerEvID	=	666 };
		CAxWindow2	fWebBrowserControlWindow;
		CWindow		fStatusWnd;
		CWindow		fMessageWnd;
	public:
		CComQIPtr<IWebBrowser2>	fWebBrowserCtl;
	public:
		void	DisplayNotification (const TCHAR* pszMessage)
			{
				const int	kBigNum = 0xfffffff;
				fStatusWnd.SendMessage(EM_SETSEL, (WPARAM)kBigNum, kBigNum);
				fStatusWnd.SendMessage(EM_REPLACESEL, 0, (LPARAM)pszMessage);
			}

		STDMETHOD(OnDownloadComplete) ()
			{
				#if 0
					CComBSTR	url;
					ThrowIfError (fWebBrowserCtl->get_LocationName (&url));
					DisplayNotification(_T("OnDownloadComplete\r\n"));
				#endif
				return S_OK;
			}

		LRESULT OnCreate (UINT, WPARAM, LPARAM, BOOL&)
			{
				_pAtlModule->Lock ();

				RECT rect;	// just to have some size --- resized in OnSize () message!
				GetClientRect(&rect);
			    
				// Create a Axhost directly as the child of the main window
				fWebBrowserControlWindow.Create (m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 1);
				if (fWebBrowserControlWindow.m_hWnd != NULL) {
					CComPtr<IUnknown> spControl;
					HRESULT hr = fWebBrowserControlWindow.CreateControlLicEx(
							OLESTR("http://www.sophists.com/"),
							NULL, 
							NULL, 
							&spControl, 
							__uuidof(DWebBrowserEvents2), 
							(IUnknown*)(IDispEventImpl<1, CMainWindow, &__uuidof(DWebBrowserEvents2), &LIBID_SHDocVw, 1, 1>*)this
						);
					fWebBrowserCtl = spControl;
				}
				fStatusWnd.Create(_T("Edit"), m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_HSCROLL| WS_VSCROLL | ES_MULTILINE, 0, 2);
				fMessageWnd.Create(_T("Static"), m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE, 0, 3);
				fMessageWnd.SendMessage (WM_SETTEXT, 0, reinterpret_cast<LPARAM> (_T("Scavanging for more words til you close this window...")));

				Led_Verify (::SetTimer (m_hWnd, kTimerEvID, kTimeout, NULL));

				return 0;
			}

		LRESULT OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
			{
				RECT rect;
				GetClientRect(&rect);

				RECT rect2;
				rect2 = rect;

				rect2.bottom -= 200;
				fWebBrowserControlWindow.MoveWindow (&rect2);

				rect2.top = rect2.bottom;
				rect2.bottom = rect2.top + 50;
				fMessageWnd.MoveWindow (&rect2);

				rect2.top = rect2.bottom;
				rect2.bottom = rect2.top + 150;
				fStatusWnd.MoveWindow (&rect2);

				bHandled = FALSE;
				return 0;
			}


		LRESULT OnTimer (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnDestroy (UINT, WPARAM, LPARAM, BOOL&);
};






class	CScavangeWordsFromNetModule	: public CAtlExeModuleT <CScavangeWordsFromNetModule> {
	private:
		typedef	CAtlExeModuleT <CScavangeWordsFromNetModule>	inherited;

	public :
		DECLARE_LIBID(LIBID_ScavangeWordsFromNetLib)
		DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SCAVANGEWORDSFROMNET, "{1AD44AAC-C29F-400A-B357-282B893F6C86}")

	CScavangeWordsFromNetModule ():
		inherited (),
		fAborted (false)
		{
		}

	public:
		bool	fAborted;

	public:
		nonvirtual	void	LoadDictionary ()
			{
				try {
					XMLDictionaryReader	reader;
					reader.Load (_T("indata.xml"));
					fWordMap = reader.fWordMap;
					fURLList = reader.fURLList;
					fDictionaryName = reader.fDictionaryName;
				}
				catch (...) {
					// warn - but use an empty list if we get an error on load
				}
			}

	public:
		nonvirtual	void	SaveDictionary ()
			{
				XMLDictionaryWriter	writer;
				writer.fWordMap = fWordMap;
				writer.fURLList = fURLList;
				writer.fDictionaryName = fDictionaryName;
				writer.Save (_T("dictionaryOut.xml"));
			}

	public:
		nonvirtual	bool	LookupWord (const Led_tString& checkWord, DictionaryIO::WordMap::iterator* resultIterator)
			{
				DictionaryIO::WordMap::iterator i = fWordMap.find (checkWord);
				if (i == fWordMap.end ()) {
					return false;
				}
				else {
					*resultIterator = i;
					return true;
				}
				return false;
			}

	public:
		nonvirtual	bool	OtherStringToIgnore (const Led_tString& checkWord)
			{
				return	OtherStringToIgnore_AllPunctuation (checkWord) or
						OtherStringToIgnore_Numeric (checkWord) or
						OtherStringToIgnore_FirstOrLastCharQuote (checkWord)
						;
			}

	public:
		nonvirtual	bool	OtherStringToIgnore_AllPunctuation (const Led_tString& checkWord)
			{
				for (size_t i = 0; i < checkWord.length (); ++i) {
					Led_tChar	c	=	checkWord[i];
					if (not CharacterProperties::IsPunct (c)) {
						return false;
					}
				}
				return true;
			}

	public:
		nonvirtual	bool	OtherStringToIgnore_Numeric (const Led_tString& checkWord)
			{
				// for now - throw away ANY words with ANY numbers in them...
				for (size_t i = 0; i < checkWord.length (); ++i) {
					Led_tChar	c	=	checkWord[i];
					if ('0' <= c and c <= '9') {
						return true;
					}
				}
				return false;
			}

	public:
		nonvirtual	bool	OtherStringToIgnore_FirstOrLastCharQuote (const Led_tString& checkWord)
			{
				if (checkWord.length () > 0 and 
					(checkWord[0] == '\'' or checkWord[checkWord.length () - 1] == '\'')
					) {
					return true;
				}
				return false;
			}

	public:
		nonvirtual	void	HandleReadText (const wstring& text)
			{
				const wchar_t*	startBuf			=	text.c_str ();
				const wchar_t*	endBuf				=	startBuf + text.length ();
				const wchar_t*	cursor				=	startBuf;
				const wchar_t*	wordStartResult		=	NULL;
				const wchar_t*	wordEndResult		=	NULL;
				while (ScanForWord (startBuf, endBuf, &cursor, &wordStartResult, &wordEndResult)) {
					Led_tString						lookupWord			=	Led_tString (wordStartResult, wordEndResult);
					DictionaryIO::WordMap::iterator	it;
					if (LookupWord (lookupWord, &it)) {
						it->second.fOccuranceCount++;
					}
					else if (not OtherStringToIgnore (lookupWord)) {
						// fill it in...
						WordInfo	wi;
						time_t aclock;
						time( &aclock );   // Get time in seconds
						wi.fDateFound = ACP2WideString (asctime( localtime( &aclock ) ) );
						if (not wi.fDateFound.empty ()) {
							wi.fDateFound = wi.fDateFound.substr (0, wi.fDateFound.length () - 1);	// trim trailing NL
						}
						wi.fOccuranceCount = 1;
						wi.fStatus = WordInfo::eEnabled;

						fWordMap.insert (DictionaryIO::WordMap::value_type (Led_tString (wordStartResult, wordEndResult), wi));
					}
				}
			}

	public:
		nonvirtual	void	HandleReadURL (const wstring& url)
			{
				if (url.length () < 8 or url.substr (0, 7) != L"http://") {
					return;
				}
				for (DictionaryIO::URLList::const_iterator i = fURLList.begin (); i != fURLList.end (); ++i) {
					if (url == *i) {
						return;
					}
				}
				fURLList.push_back (url);
			}

	public:
		nonvirtual	void	TryMSHTML (const wstring& url)
			{
				fMainWindow.DisplayNotification (Format (Led_SDK_TCHAROF ("Starting download of %s\n"), Led_Wide2SDKString (url).c_str ()).c_str ());
				CComQIPtr<IWebBrowser2>	pBrowser;
				pBrowser = fMainWindow.fWebBrowserCtl;
				CComVariant ve;
				CComVariant vurl (url.c_str ());
				ThrowIfError (pBrowser->Navigate2 (&vurl, &ve, &ve, &ve, &ve));

				READYSTATE	rs	=	READYSTATE_UNINITIALIZED;
				while (rs != READYSTATE_COMPLETE) {
						MSG msg;
						memset (&msg, 0, sizeof (msg));
						if (PeekMessage (&msg, 0, 0, 0, PM_REMOVE)) {
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						ThrowIfError (pBrowser->get_ReadyState (&rs));
				}
				if (fAborted) {
					return;
				}

				CComQIPtr<IHTMLDocument2>	pDoc2;
				{
					CComPtr<IDispatch>	x;
					ThrowIfError (pBrowser->get_Document (&x));
					pDoc2 = x;

					CComBSTR	charSet;
					ThrowIfError (pDoc2->get_charset (&charSet));
					if (charSet != NULL) {
						if (charSet != CComBSTR ("utf-8") and
							charSet != CComBSTR ("iso-8859-1") and
							charSet != CComBSTR ("windows-1252")
							) {
							fMainWindow.DisplayNotification (Format (Led_SDK_TCHAROF ("\t\t(ABORTING SUBPAGE BECAUSE TAG CHARSET= %s)\n"), Led_Wide2SDKString (wstring (charSet)).c_str ()).c_str ());
							return ;
						}
					}
				}

				CComQIPtr<IHTMLDocument3>	pDoc3	=	pDoc2;
				if (pDoc3 != NULL) {
					CComPtr<IHTMLElement>	docElt;
					ThrowIfError (pDoc3->get_documentElement (&docElt));
					ParseOutHTMLDocContentsAndAddToDictionaries_RECURSE (CComQIPtr<IHTMLElement> (docElt));
				}
				fMainWindow.DisplayNotification (Format (Led_SDK_TCHAROF ("\tfinished download {wordmap len = %d, urlmap len = %d)\n"), fWordMap.size (), fURLList.size ()).c_str ());
			}


	public:
		nonvirtual	void	ParseOutHTMLDocContentsAndAddToDictionaries_RECURSE (CComQIPtr<IHTMLElement> pElt)
			{
				CComBSTR	innerText;
				ThrowIfError (pElt->get_innerText (&innerText));

				CComBSTR	lang;
				ThrowIfError (pElt->get_lang (&lang));
				if (lang != NULL) {
					fMainWindow.DisplayNotification (Format (Led_SDK_TCHAROF ("\t\tDBG: LANG= %s\n"), Led_Wide2SDKString (wstring (lang)).c_str ()).c_str ());
					if (lang != CComBSTR ("en") and
						lang != CComBSTR ("EN-US") and
						lang != CComBSTR ("en-US") and
						lang != CComBSTR ("en-us")
						) {
						fMainWindow.DisplayNotification (Format (Led_SDK_TCHAROF ("\t\t(ABORTING SUBPAGE BECAUSE TAG LANG= %s)\n"), Led_Wide2SDKString (wstring (lang)).c_str ()).c_str ());
						return ;
					}
				}

				if (innerText != NULL) {
					HandleReadText (wstring (innerText));
				}

				CComBSTR	tagName;
				ThrowIfError (pElt->get_tagName (&tagName));

				if (tagName == L"!") {
					return;
				}

				if (tagName == L"SCRIPT" or tagName == L"script") {
					return;
				}

				if (tagName == L"STYLE" or tagName == L"style") {
					return;
				}

				if (tagName == L"A" or tagName == L"a") {
					CComVariant	attrVal;
					if (SUCCEEDED (pElt->getAttribute (CComBSTR ("href"), 0, &attrVal)) and
						SUCCEEDED (attrVal.ChangeType (VT_BSTR))
						) {
						if (attrVal.bstrVal != NULL) {
							HandleReadURL (wstring (attrVal.bstrVal));
						}
					}    
				}

				CComPtr<IDispatch>	childrenx;
 				ThrowIfError (pElt->get_children (&childrenx));
				CComQIPtr<IHTMLElementCollection>	children = childrenx;
				{
					long	len	=	0;
					ThrowIfError (children->get_length (&len));
					for (size_t i = 0; i < len; ++i) {
						CComPtr<IDispatch>	item;
						ThrowIfError (children->item (CComVariant (i), CComVariant (i), &item));
						CComQIPtr<IHTMLElement>	subItem	=	item;
						if (subItem != NULL) {
							ParseOutHTMLDocContentsAndAddToDictionaries_RECURSE (subItem);
						}
					}
				}
			}

	public:
		nonvirtual	void	DoRandomPageFetch ()
			{
				static	bool			sCalledBefore		=	false;
				static	size_t			sInitialURLListSize	=	0;
				static	unsigned int	sTotalCalls			=	0;
				if (not sCalledBefore) {
					// hack to assure we call main (seeded) sites at least once before we pollute the list of URLs with uninteresting ones
					sInitialURLListSize = fURLList.size ();
					sInitialURLListSize = min (sInitialURLListSize, 10);
					sCalledBefore = true;
				}

				try {
					size_t	maxIdx	=	fURLList.size ();
					if (maxIdx > 0) {
						size_t		useIdx	=	rand () % maxIdx;
						if (sTotalCalls < sInitialURLListSize) {
							useIdx = sTotalCalls % maxIdx;			//  really just sTotalCalls - but be paranoid if somehow totalCalls out
																	// of sync...
						}
						TryMSHTML (fURLList[useIdx]);
					}
				}
				catch (...) {
				}
				sTotalCalls++;
			}


	private:
		wstring					fDictionaryName;
		DictionaryIO::WordMap	fWordMap;
		DictionaryIO::URLList	fURLList;
		CMainWindow				fMainWindow;

	public:
		override	HRESULT PreMessageLoop (int nShowCmd) throw()
			{
				HRESULT	hr	=	inherited::PreMessageLoop (nShowCmd);

				LoadDictionary ();

				if (SUCCEEDED(hr)) {
					AtlAxWinInit ();
					hr = S_OK;
					RECT rc;
					rc.top = rc.left = 100;
					rc.bottom = 800;
					rc.right = 1000;
					fMainWindow.Create (NULL, rc, _T("ActiveSpelledIt! Net Word Scavanger") );
					fMainWindow.ShowWindow (nShowCmd);         
				}

				return hr;
			}

 	public:
		HRESULT PostMessageLoop ()
			{
				SaveDictionary ();
				AtlAxWinTerm ();
				return inherited::PostMessageLoop ();
			}
};

CScavangeWordsFromNetModule _AtlModule;



LRESULT CMainWindow::OnTimer (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TIMERPROC*	timerProc	=	reinterpret_cast<TIMERPROC*> (lParam);
	UINT nEventID	=	wParam;
	if (nEventID == kTimerEvID) {

		bHandled = TRUE;

		static	bool	inTimerEvt	=	false;

		Led_Assert (not inTimerEvt);
		inTimerEvt = true;
		Led_Verify (::KillTimer (m_hWnd, kTimerEvID));
		try {
			_AtlModule.DoRandomPageFetch ();
		}
		catch (...) {
			// ingore exceptions here
		}
		inTimerEvt = false;
		if (not _AtlModule.fAborted) {
			Led_Verify (::SetTimer (m_hWnd, kTimerEvID, kTimeout, NULL));
		}
	}
	return 0;
}

LRESULT CMainWindow::OnDestroy (UINT, WPARAM, LPARAM, BOOL&)
{
	(void)::KillTimer (m_hWnd, kTimerEvID);
	_AtlModule.fAborted = true;
	_pAtlModule->Unlock ();
	return 0;
}



extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	srand ( (unsigned)time( NULL ) );		//	Seed the random-number generator with current time so that
											//the numbers will be different every time we run.

	return _AtlModule.WinMain(nShowCmd);
}

