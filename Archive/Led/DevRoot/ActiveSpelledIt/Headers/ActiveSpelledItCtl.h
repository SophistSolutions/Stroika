/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedItCtl_h__
#define	__ActiveLedItCtl_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Headers/ActiveSpelledItCtl.h,v 1.10 2003/12/17 21:01:31 lewis Exp $
 */

/*
 * Changes:
 *	$Log: ActiveSpelledItCtl.h,v $
 *	Revision 1.10  2003/12/17 21:01:31  lewis
 *	SPR#1600: Change FindWordBreaks method to return an Object (_WordInfo) - and created and supported that new object.
 *	
 *	Revision 1.9  2003/12/17 01:59:51  lewis
 *	SPR#1585: also added propget AddWordToUserDictionarySupported
 *	
 *	Revision 1.8  2003/12/17 01:19:18  lewis
 *	SPR#1585: Added several new UD/SD loading APIs to ActiveSpelledIt. Minimal testing of the new
 *	APIs - but at least saving to UDs works (AddWordToUD UI/API).
 *	
 *	Revision 1.7  2003/06/12 17:33:55  lewis
 *	SPR#1425: fix last checkin message: really did a few small cheanges like passing in CURSOR
 *	arg to CreateScanContext API (with IDL defaultValue(0)) and added LookupWord () method
 *	
 *	Revision 1.6  2003/06/12 17:30:39  lewis
 *	SPR#1526: significantly embellished test suiteTests/ActiveSpelledItCtl.htm
 *	
 *	Revision 1.5  2003/06/10 16:30:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.4  2003/06/10 15:14:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.3  2003/06/10 14:59:30  lewis
 *	SPR#1526: Added IObjectSafetyImpl<> support
 *	
 *	Revision 1.2  2003/06/10 14:30:03  lewis
 *	SPR#1526: more work - improved test suite - added basic DEMO-MODE support,
 *	added version# OLE AUTOMATION API, exception safety in OLEAUT code, and
 *	maybe a bit more...
 *	
 *	Revision 1.1  2003/06/10 02:06:15  lewis
 *	first cut at ActiveSpelledIt control
 *	
 *
 *
 *
 */

#include	"ActiveSpelledItConfig.h"

#include	<atlctl.h>

#include	"SpellCheckEngine_Basic.h"

#include	"resource.h"       // main symbols

#include	"ActiveSpelledIt_IDL.h"


class	ActiveSpelledIt_ScanContext;


// ActiveSpelledItCtl
class ATL_NO_VTABLE ActiveSpelledItCtl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IActiveSpelledItCtl, &IID_IActiveSpelledItCtl, &LIBID_ActiveSpelledItLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<ActiveSpelledItCtl>,
	public IOleControlImpl<ActiveSpelledItCtl>,
	public IOleObjectImpl<ActiveSpelledItCtl>,
	public IOleInPlaceActiveObjectImpl<ActiveSpelledItCtl>,
	public IViewObjectExImpl<ActiveSpelledItCtl>,
	public IOleInPlaceObjectWindowlessImpl<ActiveSpelledItCtl>,
	public IObjectSafetyImpl<ActiveSpelledItCtl, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<ActiveSpelledItCtl>,
	public ISpecifyPropertyPagesImpl<ActiveSpelledItCtl>,
	public IDataObjectImpl<ActiveSpelledItCtl>,
	public IProvideClassInfo2Impl<&CLSID_ActiveSpelledItCtl, NULL, &LIBID_ActiveSpelledItLib>,
	public CComCoClass<ActiveSpelledItCtl, &CLSID_ActiveSpelledItCtl>,
	public CComControl<ActiveSpelledItCtl>
{
	private:
		Led::SpellCheckEngine_Basic_Simple	fSpellCheckEngine;

	public:
		ActiveSpelledItCtl ();
		~ActiveSpelledItCtl ();

	public:
		DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
			OLEMISC_INVISIBLEATRUNTIME |
			OLEMISC_CANTLINKINSIDE | 
			OLEMISC_INSIDEOUT | 
			OLEMISC_ACTIVATEWHENVISIBLE | 
			OLEMISC_SETCLIENTSITEFIRST
		)

	public:
		DECLARE_REGISTRY_RESOURCEID(IDR_ACTIVESPELLEDITCTL)

	public:
		BEGIN_COM_MAP(ActiveSpelledItCtl)
			COM_INTERFACE_ENTRY(IActiveSpelledItCtl)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(IViewObjectEx)
			COM_INTERFACE_ENTRY(IViewObject2)
			COM_INTERFACE_ENTRY(IViewObject)
			COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
			COM_INTERFACE_ENTRY(IOleInPlaceObject)
			COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
			COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
			COM_INTERFACE_ENTRY(IOleControl)
			COM_INTERFACE_ENTRY(IOleObject)
			COM_INTERFACE_ENTRY(IPersistStreamInit)
			COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
			COM_INTERFACE_ENTRY(IObjectSafety)
			COM_INTERFACE_ENTRY(ISupportErrorInfo)
			COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
			COM_INTERFACE_ENTRY(IPersistStorage)
			COM_INTERFACE_ENTRY(IDataObject)
			COM_INTERFACE_ENTRY(IProvideClassInfo)
			COM_INTERFACE_ENTRY(IProvideClassInfo2)
		END_COM_MAP()

	public:
		BEGIN_PROP_MAP(ActiveSpelledItCtl)
			PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
			PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
			// Example entries
			// PROP_ENTRY("Property Description", dispid, clsid)
			// PROP_PAGE(CLSID_StockColorPage)
		END_PROP_MAP()

	public:
		BEGIN_MSG_MAP(ActiveSpelledItCtl)
			CHAIN_MSG_MAP(CComControl<ActiveSpelledItCtl>)
			DEFAULT_REFLECTION_HANDLER()
		END_MSG_MAP()

	public:
		// Handler prototypes:
		//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	// ISupportsErrorInfo
	public:
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	// IViewObjectEx
	public:
		DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IActiveSpelledItCtl
	public:
		HRESULT OnDraw (ATL_DRAWINFO& di);

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	public:
		STDMETHOD(GenerateSuggestions)(BSTR missingWord, VARIANT* results);
		STDMETHOD(CreateScanContext)(BSTR text, UINT cursor, IDispatch** scanContext);
		STDMETHOD(get_VersionNumber)(LONG* pVal);
		STDMETHOD(get_ShortVersionString)(BSTR* pVal);
		STDMETHOD(LookupWord)(BSTR word, BSTR* matchedWord, VARIANT_BOOL* found);
		STDMETHOD(FindWordBreaks)(BSTR srcText, UINT textOffsetToStartLookingForWord, IDispatch** wordInfo);
		STDMETHOD(get_AvailableSystemDictionaries)(VARIANT* pVal);
		STDMETHOD(get_SystemDictionaryList)(VARIANT* pVal);
		STDMETHOD(put_SystemDictionaryList)(VARIANT val);
		STDMETHOD(get_DictionaryList)(VARIANT* pVal);
		STDMETHOD(get_UserDictionary)(BSTR* pUDName);
		STDMETHOD(put_UserDictionary)(BSTR UDName);
		STDMETHOD(get_DefaultUserDictionaryName)(BSTR* pUDName);
		STDMETHOD(AddWordToUserDictionary)(BSTR word);
		STDMETHOD(get_AddWordToUserDictionarySupported)(VARIANT_BOOL* supported);

	private:
		friend	class	ActiveSpelledIt_ScanContext;
};

OBJECT_ENTRY_AUTO(__uuidof(ActiveSpelledItCtl), ActiveSpelledItCtl)

#endif	/*__ActiveLedItCtl_h__*/
