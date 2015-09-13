/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedIt_ScanContext_h__
#define	__ActiveLedIt_ScanContext_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Headers/ActiveSpelledIt_ScanContext.h,v 1.5 2003/12/17 01:19:19 lewis Exp $
 */

/*
 * Changes:
 *	$Log: ActiveSpelledIt_ScanContext.h,v $
 *	Revision 1.5  2003/12/17 01:19:19  lewis
 *	SPR#1585: Added several new UD/SD loading APIs to ActiveSpelledIt. Minimal testing of the new APIs - but at least saving to UDs works (AddWordToUD UI/API).
 *	
 *	Revision 1.4  2003/06/12 17:33:55  lewis
 *	SPR#1425: fix last checkin message: really did a few small cheanges like passing in
 *	CURSOR arg to CreateScanContext API (with IDL defaultValue(0)) and added LookupWord ()
 *	method
 *	
 *	Revision 1.3  2003/06/12 17:30:39  lewis
 *	SPR#1526: significantly embellished test suiteTests/ActiveSpelledItCtl.htm
 *	
 *	Revision 1.2  2003/06/10 15:14:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2003/06/10 02:06:15  lewis
 *	first cut at ActiveSpelledIt control
 *	
 *
 *
 *
 */

#include	"ActiveSpelledItConfig.h"

#include	"LedSupport.h"

#include	"resource.h"       // main symbols

#include	"ActiveSpelledIt_IDL.h"


class	ActiveSpelledItCtl;


// ActiveSpelledIt_ScanContext
class ATL_NO_VTABLE ActiveSpelledIt_ScanContext : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveSpelledIt_ScanContext>,
	public IDispatchImpl<IActiveSpelledIt_ScanContext, &IID_IActiveSpelledIt_ScanContext, &LIBID_ActiveSpelledItLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	public:
		struct	CTOR_ARGS {
			CTOR_ARGS (ActiveSpelledItCtl* owner, BSTR text, UINT cursor);

			ActiveSpelledItCtl*	fOwner;
			BSTR				fText;
			UINT				fOrigCursor;
		};
		ActiveSpelledIt_ScanContext (const CTOR_ARGS& args);
		virtual ~ActiveSpelledIt_ScanContext ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		BEGIN_COM_MAP(ActiveSpelledIt_ScanContext)
			COM_INTERFACE_ENTRY(IActiveSpelledIt_ScanContext)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	public:
		STDMETHOD(get_WordFound)(VARIANT_BOOL* pVal);
		STDMETHOD(get_WordStart)(ULONG* pVal);
		STDMETHOD(get_WordEnd)(ULONG* pVal);
		STDMETHOD(get_Word)(BSTR* pVal);
		STDMETHOD(Next)(void);
		STDMETHOD(get_Suggestions)(VARIANT* pVal);
		STDMETHOD(get_Cursor)(ULONG* pVal);

	private:
		ActiveSpelledItCtl*		fOwner;
		CComBSTR				fText;
		const Led_tChar*		fTextStart;
		const Led_tChar*		fTextEnd;
		const Led_tChar*		fCursor;
		bool					fLastFound;
		const Led_tChar*		fWordStart;
		const Led_tChar*		fWordEnd;
};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	ActiveSpelledIt_ScanContext::CTOR_ARGS
	inline	ActiveSpelledIt_ScanContext::CTOR_ARGS::CTOR_ARGS (ActiveSpelledItCtl* owner, BSTR text, UINT cursor):
		fOwner (owner),
		fText (text),
		fOrigCursor (cursor)
		{
		}


#endif	/*__ActiveLedIt_ScanContext_h__*/
