/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_ATL_h_
#define _Stroika_Frameworks_Led_Platform_ATL_h_ 1

/*
@MODULE:    Led_ATL
@DESCRIPTION:
        <p>Led MFC specific wrappers.</p>
        <p>REALLY not yet implemented (and won't be for 3.1). But I do have some components using
    ATL, and this is a temporary repository for those utilities.</p>
 */

#include "../../../Foundation/StroikaPreComp.h"

static_assert (qHasFeature_ATLMFC, "Error: Stroika::Framework::Led::Platform ATL code requires the ATLMFC feature to be set true");

#include <atlbase.h>
#include <atlcom.h>

#include "../Config.h"

#include "../Support.h"

namespace Stroika::Frameworks::Led::Platform {

    /*
    @CLASS:         CComObjectWithARGS<BASE,CTOR_ARGS>
    @DESCRIPTION:   <p>Virtually identical to the ATL class @'CComObject<Base>', except that this is used
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
    template <typename BASE, typename CTOR_ARGS>
    class CComObjectWithARGS : public BASE {
    private:
        using inherited = BASE;

    public:
        CComObjectWithARGS (const CTOR_ARGS& args);
        virtual ~CComObjectWithARGS () noexcept;

    public:
        STDMETHOD_ (ULONG, AddRef)
        ();
        STDMETHOD_ (ULONG, Release)
        ();
        STDMETHOD (QueryInterface)
        (REFIID iid, void** ppvObject) noexcept;
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    //  class   CComObjectWithARGS<BASE,CTOR_ARGS>
    template <typename BASE, typename CTOR_ARGS>
    inline CComObjectWithARGS<BASE, CTOR_ARGS>::CComObjectWithARGS (const CTOR_ARGS& args)
        : inherited (args)
    {
        _pAtlModule->Lock ();
    }
    template <typename BASE, typename CTOR_ARGS>
    CComObjectWithARGS<BASE, CTOR_ARGS>::~CComObjectWithARGS () noexcept
    {
        m_dwRef = -(LONG_MAX / 2);
        FinalRelease ();
#ifdef _ATL_DEBUG_INTERFACES
        _AtlDebugInterfacesModule.DeleteNonAddRefThunk (_GetRawUnknown ());
#endif
        _pAtlModule->Unlock ();
    }
    template <typename BASE, typename CTOR_ARGS>
    ULONG STDMETHODCALLTYPE CComObjectWithARGS<BASE, CTOR_ARGS>::AddRef ()
    {
        return InternalAddRef ();
    }
    template <typename BASE, typename CTOR_ARGS>
    ULONG STDMETHODCALLTYPE CComObjectWithARGS<BASE, CTOR_ARGS>::Release ()
    {
        ULONG l = InternalRelease ();
        if (l == 0) {
            delete this;
        }
        return l;
    }
    template <typename BASE, typename CTOR_ARGS>
    HRESULT STDMETHODCALLTYPE CComObjectWithARGS<BASE, CTOR_ARGS>::QueryInterface (REFIID iid, void** ppvObject) noexcept
    {
        //if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
        return _InternalQueryInterface (iid, ppvObject);
    }

}

#endif /*_Stroika_Frameworks_Led_Platform_ATL_h_*/
