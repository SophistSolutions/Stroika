/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedIt_WordInfo_h__
#define	__ActiveLedIt_WordInfo_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Headers/ActiveSpelledIt_WordInfo.h,v 1.1 2004/01/01 23:24:47 lewis Exp $
 */

/*
 * Changes:
 *	$Log: ActiveSpelledIt_WordInfo.h,v $
 *	Revision 1.1  2004/01/01 23:24:47  lewis
 *	forgot to checkin earlier
 *	
 *	
 *
 *
 *
 */

#include	"ActiveSpelledItConfig.h"

#include	"LedSupport.h"

#include	"resource.h"       // main symbols

#include	"ActiveSpelledIt_IDL.h"



// ActiveSpelledIt_WordInfo
class ATL_NO_VTABLE ActiveSpelledIt_WordInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveSpelledIt_WordInfo>,
	public IDispatchImpl<IActiveSpelledIt_WordInfo, &IID_IActiveSpelledIt_WordInfo, &LIBID_ActiveSpelledItLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	public:
		struct	CTOR_ARGS {
			CTOR_ARGS (UINT start, UINT end, bool wordReal);

			UINT	fWordStart;
			UINT	fWordEnd;
			bool	fWordReal;
		};
		ActiveSpelledIt_WordInfo (const CTOR_ARGS& args);
		virtual ~ActiveSpelledIt_WordInfo ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		BEGIN_COM_MAP(ActiveSpelledIt_WordInfo)
			COM_INTERFACE_ENTRY(IActiveSpelledIt_WordInfo)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	public:
		STDMETHOD(get_WordStart)(UINT* pVal);
		STDMETHOD(get_WordEnd)(UINT* pVal);
		STDMETHOD(get_WordReal)(VARIANT_BOOL* pVal);

	private:
		UINT	fWordStart;
		UINT	fWordEnd;
		bool	fWordReal;
};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	ActiveSpelledIt_WordInfo::CTOR_ARGS
	inline	ActiveSpelledIt_WordInfo::CTOR_ARGS::CTOR_ARGS (UINT start, UINT end, bool wordReal):
		fWordStart (start),
		fWordEnd (end),
		fWordReal (wordReal)
		{
		}


#endif	/*__ActiveLedIt_WordInfo_h__*/
