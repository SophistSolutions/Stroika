/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_ATL_h__
#define	__Led_ATL_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_ATL.h,v 2.3 2004/01/23 03:01:17 lewis Exp $
 */


/*
@MODULE:	Led_ATL
@DESCRIPTION:
		<p>Led MFC specific wrappers.</p>
		<p>REALLY not yet implemented (and won't be for 3.1). But I do have some components using
	ATL, and this is a temporary repository for those utilities.</p>
 */



/*
 * Changes:
 *	$Log: Led_ATL.h,v $
 *	Revision 2.3  2004/01/23 03:01:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2003/06/12 15:18:13  lewis
 *	moved CreateSafeArrayOfBSTR () from Led_ATL to LedSupport
 *	
 *	Revision 2.1  2003/06/10 14:27:49  lewis
 *	SPR#1526: as part of adding ATL-based ActiveSpelledIt - created helper file for Led/ATL
 *	
 *
 *
 *
 */

#include	<atlbase.h>
#include	<atlcom.h>

#include	"LedConfig.h"
#include	"LedSupport.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			CComObjectWithARGS<BASE,CTOR_ARGS>
@DESCRIPTION:	<p>Virtually identical to the ATL class @'CComObject<Base>', except that this is used
			to allow an object to be directly constructed from C++ (instead of through CoCreateInstance), and
			allowing constructor arguments to be passed along. It seems CRAZY that this isn't made
			straight-forward in ATL. Perhaps it is - and I just haven't figured it out.
				</p>
				<p>This is functionally similar to the hack I used to do (e.g. at LEC) by patching
			(replacing/cloning) the END_COM_MAP macro (to do the AddRef/etc method defs you see
			here). I'm not sure which approach is better - but this isn't too bad. NB: this code is
			based on the @'CComObject<Base>' code from _ATL_VER==0x0710 (from MSVC.Net 2003).
			</p>
*/
template	<typename BASE, typename CTOR_ARGS>
	class CComObjectWithARGS : public BASE {
	private:
		typedef BASE inherited;
	
	public:
		CComObjectWithARGS (const CTOR_ARGS& args);
		virtual ~CComObjectWithARGS() throw ();

	public:
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();
		STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw();
	};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	CComObjectWithARGS<BASE,CTOR_ARGS>
	template	<typename BASE, typename CTOR_ARGS>
		inline	CComObjectWithARGS<BASE,CTOR_ARGS>::CComObjectWithARGS (const CTOR_ARGS& args):
			inherited (args)
		{
			_pAtlModule->Lock();
		}
	template	<typename BASE, typename CTOR_ARGS>
		CComObjectWithARGS<BASE,CTOR_ARGS>::~CComObjectWithARGS() throw ()
			{
				m_dwRef = -(LONG_MAX/2);
				FinalRelease();
				#ifdef _ATL_DEBUG_INTERFACES
					_AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
				#endif
				_pAtlModule->Unlock();
			}
	template	<typename BASE, typename CTOR_ARGS>
		ULONG	STDMETHODCALLTYPE CComObjectWithARGS<BASE,CTOR_ARGS>::AddRef ()
			{
				return InternalAddRef ();
			}
	template	<typename BASE, typename CTOR_ARGS>
		ULONG	STDMETHODCALLTYPE CComObjectWithARGS<BASE,CTOR_ARGS>::Release ()
			{
				ULONG l = InternalRelease();
				if (l == 0) {
					delete this;
				}
				return l;
			}
	template	<typename BASE, typename CTOR_ARGS>
		HRESULT	STDMETHODCALLTYPE CComObjectWithARGS<BASE,CTOR_ARGS>::QueryInterface (REFIID iid, void ** ppvObject) throw()
			{
				//if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
				return _InternalQueryInterface(iid, ppvObject);
			}

#if		qLedUsesNamespaces
}
#endif



#endif	/*__Led_ATL_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
