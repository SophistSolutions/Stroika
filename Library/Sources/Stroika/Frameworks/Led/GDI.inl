/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_GDI_inl_
#define _Stroika_Frameworks_Led_GDI_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Foundation/Execution/Exceptions.h"

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************************ Point_Base ********************************
     ********************************************************************************
     */
    template <typename COORD_TYPE>
    inline Point_Base<COORD_TYPE>::Point_Base ()
        : v (COORD_TYPE (0))
        , h (COORD_TYPE (0))
    {
    }
    template <typename COORD_TYPE>
    inline Point_Base<COORD_TYPE>::Point_Base (COORD_TYPE newV, COORD_TYPE newH)
        : v (newV)
        , h (newH)
    {
    }
    template <typename COORD_TYPE>
    inline Point_Base<COORD_TYPE> operator+ (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        Led_Point result = lhs;
        result.v += rhs.v;
        result.h += rhs.h;
        return result;
    }

    /*
     ********************************************************************************
     ************************************ Led_Point *********************************
     ********************************************************************************
     */
    inline Led_Point operator- (const Led_Point& lhs, const Led_Point& rhs)
    {
        return (Led_Point (lhs.v - rhs.v, lhs.h - rhs.h));
    }

    /*
     ********************************************************************************
     ************************************ Rect_Base *********************************
     ********************************************************************************
     */
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base
    @DESCRIPTION:   <p>No-argument constructor leaves object uninitialized (garbage data).</p>
    */
    inline Rect_Base<POINT_TYPE, SIZE_TYPE>::Rect_Base ()
        : top (CoordinateType (0))
        , left (CoordinateType (0))
        , bottom (CoordinateType (0))
        , right (CoordinateType (0))
    {
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::Rect_Base
    @DESCRIPTION:   <p>Initialize with argument top, left, height, width.</p>
    */
    inline Rect_Base<POINT_TYPE, SIZE_TYPE>::Rect_Base (CoordinateType newTop, CoordinateType newLeft, DistanceType newHeight, DistanceType newWidth)
        : top (newTop)
        , left (newLeft)
        , bottom (newTop + newHeight)
        , right (newLeft + newWidth)
    {
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline Rect_Base<POINT_TYPE, SIZE_TYPE>::Rect_Base (POINT_TYPE origin, SIZE_TYPE size)
        : top (origin.v)
        , left (origin.h)
        , bottom (origin.v + size.v)
        , right (origin.h + size.h)
    {
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetTop
    @DESCRIPTION:   <p>Retrieve top of rectangle.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::CoordinateType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetTop () const
    {
        return top;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetLeft
    @DESCRIPTION:   <p>Retrieve left of rectangle.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::CoordinateType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetLeft () const
    {
        return left;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetBottom
    @DESCRIPTION:   <p>Retrieve bottom of rectangle.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::CoordinateType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetBottom () const
    {
        return bottom;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetRight
    @DESCRIPTION:   <p>Retrieve right of rectangle.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::CoordinateType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetRight () const
    {
        return right;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::IsEmpty
    @DESCRIPTION:   <p>Returns true if either horizontal or vertical dimentions are less or equal to zero.</p>
    */
    inline bool
    Rect_Base<POINT_TYPE, SIZE_TYPE>::IsEmpty () const
    {
        return (right <= left or bottom <= top);
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetHeight
    @DESCRIPTION:   <p>Asserts height is non-negative, and then returns height.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::DistanceType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetHeight () const
    {
        Ensure (bottom >= top);
        return (bottom - top);
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::GetWidth
    @DESCRIPTION:   <p>Asserts width is non-negative, and then returns width.</p>
    */
    inline typename Rect_Base<POINT_TYPE, SIZE_TYPE>::DistanceType
    Rect_Base<POINT_TYPE, SIZE_TYPE>::GetWidth () const
    {
        Ensure (right >= left);
        return (right - left);
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline POINT_TYPE Rect_Base<POINT_TYPE, SIZE_TYPE>::GetTopLeft () const
    {
        return (POINT_TYPE (top, left));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline POINT_TYPE Rect_Base<POINT_TYPE, SIZE_TYPE>::GetTopRight () const
    {
        return (POINT_TYPE (top, right));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline POINT_TYPE Rect_Base<POINT_TYPE, SIZE_TYPE>::GetBotRight () const
    {
        return (POINT_TYPE (bottom, right));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline POINT_TYPE Rect_Base<POINT_TYPE, SIZE_TYPE>::GetOrigin () const
    {
        return (POINT_TYPE (top, left));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline SIZE_TYPE Rect_Base<POINT_TYPE, SIZE_TYPE>::GetSize () const
    {
        return (SIZE_TYPE (bottom - top, right - left));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline bool Rect_Base<POINT_TYPE, SIZE_TYPE>::Contains (POINT_TYPE p) const
    {
        return ((p >= GetTopLeft ()) and (p <= GetBotRight ()));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline bool Rect_Base<POINT_TYPE, SIZE_TYPE>::Contains (Rect_Base<POINT_TYPE, SIZE_TYPE> r) const
    {
        // could be more terse, but this does it...
        return (Contains (r.GetTopLeft ()) and Contains (r.GetBotRight ()));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetOrigin (POINT_TYPE p)
    {
        top  = p.v;
        left = p.h;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetTop (CoordinateType t)
    {
        this->top = t;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetLeft (CoordinateType l)
    {
        this->left = l;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetBottom (CoordinateType b)
    {
        this->bottom = b;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetRight (CoordinateType r)
    {
        this->right = r;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline const Rect_Base<POINT_TYPE, SIZE_TYPE>& Rect_Base<POINT_TYPE, SIZE_TYPE>::operator+= (const POINT_TYPE& delta)
    {
        *this = THIS_TYPE (GetTopLeft () + delta, GetSize ()); // simple definition - but could tweek!
        return *this;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline const Rect_Base<POINT_TYPE, SIZE_TYPE>& Rect_Base<POINT_TYPE, SIZE_TYPE>::operator-= (const POINT_TYPE& delta)
    {
        *this = THIS_TYPE (GetTopLeft () - delta, GetSize ()); // simple definition - but could tweek!
        return *this;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    /*
    @METHOD:        Rect_Base<POINT_TYPE,SIZE_TYPE>::operator*=
    @DESCRIPTION:   <p>Intersects this rect with the argument rectangle.</p>
    */
    inline const Rect_Base<POINT_TYPE, SIZE_TYPE>&
    Rect_Base<POINT_TYPE, SIZE_TYPE>::operator*= (const Rect_Base<POINT_TYPE, SIZE_TYPE>& intersectWith)
    {
        POINT_TYPE topLeft  = max (GetTopLeft (), intersectWith.GetTopLeft ());
        POINT_TYPE botRight = min (GetBotRight (), intersectWith.GetBotRight ());
        POINT_TYPE newSize  = botRight - topLeft;
        if (newSize >= Led_Point (0, 0)) {
            *this = THIS_TYPE (topLeft, SIZE_TYPE (newSize));
        }
        else {
            *this = THIS_TYPE (0, 0, 0, 0);
        }
        return *this;
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline bool Rect_Base<POINT_TYPE, SIZE_TYPE>::operator== (const Rect_Base<POINT_TYPE, SIZE_TYPE>& rhs) const
    {
        return ((GetLeft () == rhs.GetLeft ()) and (GetRight () == rhs.GetRight ()) and (GetTop () == rhs.GetTop ()) and (GetBottom () == rhs.GetBottom ()));
    }

    /*
     ********************************************************************************
     ************************************ TWIPS *************************************
     ********************************************************************************
     */
    constexpr inline TWIPS::TWIPS (long v)
        : fValue (v)
    {
    }
    inline TWIPS::operator long () const
    {
        return fValue;
    }
    inline TWIPS& TWIPS::operator+= (const TWIPS& rhs)
    {
        fValue += rhs.fValue;
        return *this;
    }
    inline TWIPS& TWIPS::operator-= (const TWIPS& rhs)
    {
        fValue -= rhs.fValue;
        return *this;
    }
    inline TWIPS& TWIPS::operator*= (double rhs)
    {
        fValue = static_cast<long> (fValue * rhs);
        return *this;
    }
    inline TWIPS operator+ (const TWIPS& lhs, const TWIPS& rhs)
    {
        return TWIPS (static_cast<long> (lhs) + static_cast<long> (rhs));
    }
    inline TWIPS operator- (const TWIPS& lhs, const TWIPS& rhs)
    {
        return TWIPS (static_cast<long> (lhs) - static_cast<long> (rhs));
    }

#if qPlatform_Windows
    /*
     ********************************************************************************
     ******************************** FontObject ************************************
     ********************************************************************************
     */
    inline FontObject::~FontObject ()
    {
        (void)DeleteObject ();
    }
    inline FontObject ::operator HFONT () const
    {
        return m_hObject;
    }
    inline int FontObject ::GetObject (int nCount, LPVOID lpObject) const
    {
        Assert (m_hObject != nullptr);
        return ::GetObject (m_hObject, nCount, lpObject);
    }
    inline BOOL FontObject ::DeleteObject ()
    {
        if (m_hObject == nullptr)
            return FALSE;
        HFONT h   = m_hObject;
        m_hObject = nullptr;
        return ::DeleteObject (h);
    }
    inline BOOL FontObject ::CreateFontIndirect (const LOGFONT* lpLogFont)
    {
        return Attach (::CreateFontIndirect (lpLogFont));
    }
    inline BOOL FontObject ::Attach (HFONT hObject)
    {
        Assert (m_hObject == nullptr); // only attach once, detach on destroy
        if (hObject == nullptr)
            return FALSE;
        m_hObject = hObject;
        return TRUE;
    }
#endif

#if qPlatform_Windows
    /*
     ********************************************************************************
     *********************************** Brush **************************************
     ********************************************************************************
     */
    inline Brush::Brush (COLORREF crColor)
        : m_hObject{nullptr}
    {
        if (!Attach (::CreateSolidBrush (crColor)))
            Foundation::Execution::Throw (bad_alloc{});
    }
    inline Brush::~Brush ()
    {
        (void)DeleteObject ();
    }
    inline Brush::operator HBRUSH () const
    {
        return m_hObject;
    }
    inline BOOL Brush::Attach (HBRUSH hObject)
    {
        Assert (m_hObject == nullptr); // only attach once, detach on destroy
        if (hObject == nullptr)
            return FALSE;
        m_hObject = hObject;
        return TRUE;
    }
    inline BOOL Brush::DeleteObject ()
    {
        if (m_hObject == nullptr)
            return FALSE;
        HBRUSH h  = m_hObject;
        m_hObject = nullptr;
        return ::DeleteObject (h);
    }
#endif

    /*
     ********************************************************************************
     ************************************ Region ********************************
     ********************************************************************************
     */
    inline Region::Region ()
#if qPlatform_MacOS
        : fRgn{::NewRgn ()}
        , fOwned
    {
        true
    }
#elif qPlatform_Windows
        : fRgn
    {
        ::CreateRectRgn (0, 0, 0, 0)
    }
#endif
    {
#if qPlatform_MacOS || qPlatform_Windows
        Foundation::Execution::ThrowIfNull (fRgn);
#endif
    }
    inline Region::Region (const Led_Rect& r)
#if qPlatform_MacOS
        : fRgn{::NewRgn ()}
        , fOwned (true)
#elif qPlatform_Windows
        : fRgn (::CreateRectRgn (r.GetLeft (), r.GetTop (), r.GetRight (), r.GetBottom ()))
#endif
    {
        Require (r.GetHeight () >= 0);
        Require (r.GetWidth () >= 0);
#if qPlatform_MacOS || qPlatform_Windows
        Foundation::Execution::ThrowIfNull (fRgn);
#endif
#if qPlatform_MacOS
        ::SetRectRgn (fRgn, (short)r.left, (short)r.top, (short)r.right, (short)r.bottom);
#endif
        Assert (GetBoundingRect () == r or (GetBoundingRect ().IsEmpty () and r.IsEmpty ()));
    }
    inline Region::Region (const Region& from)
#if qPlatform_MacOS
        : fRgn (::NewRgn ())
        , fOwned (true)
#elif qPlatform_Windows
        : fRgn (::CreateRectRgn (0, 0, 0, 0))
#endif
    {
#if qPlatform_MacOS || qPlatform_Windows
        Foundation::Execution::ThrowIfNull (fRgn);
#endif
#if qPlatform_MacOS
        ::CopyRgn (from.GetOSRep (), fRgn);
#elif qPlatform_Windows
        Verify (::CombineRgn (fRgn, from, from, RGN_COPY) != ERROR);
#endif
    }
    inline const Region& Region::operator= (const Region& rhs)
    {
#if qPlatform_MacOS
        if (fOwned and fRgn != nullptr) {
            ::DisposeRgn (fRgn);
        }
        fOwned = true;
        fRgn   = ::NewRgn ();
        ::CopyRgn (rhs.GetOSRep (), fRgn);
#elif qPlatform_Windows
        Verify (::CombineRgn (fRgn, rhs, rhs, RGN_COPY) != ERROR);
#endif
#if qPlatform_MacOS || qPlatform_Windows
        Foundation::Execution::ThrowIfNull (fRgn);
#endif
        return *this;
    }
    inline Region::~Region ()
    {
#if qPlatform_MacOS
        if (fOwned and fRgn != nullptr) {
            ::DisposeRgn (fRgn);
        }
#elif qPlatform_Windows
        if (fRgn != nullptr) {
            ::DeleteObject (fRgn);
        }
#endif
    }
    inline bool Region::IsEmpty () const
    {
#if qPlatform_MacOS || qPlatform_Windows
        AssertNotNull (fRgn);
#endif
        Assert (false); //NYI - not used yet - so don't worry about this right now... LGP 2002-12-03
        return false;
    }
    inline Led_Rect Region::GetBoundingRect () const
    {
#if qPlatform_MacOS || qPlatform_Windows
        AssertNotNull (fRgn);
#endif
#if qPlatform_MacOS
#if TARGET_CARBON
        Rect bnds;
        return AsLedRect (*::GetRegionBounds (fRgn, &bnds));
#else
        return AsLedRect ((**fRgn).rgnBBox);
#endif
#elif qPlatform_Windows
        RECT r;
        int  tmp = ::GetRgnBox (fRgn, &r);
        Verify (tmp != 0);
#if qDebug
        {
            if (tmp == 0) {
                Assert (AsLedRect (r) == Led_Rect (0, 0, 0, 0));
            }
        }
#endif
        return AsLedRect (r);
#else
        Assert (false);
        return Led_Rect (0, 0, 0, 0);
#endif
    }
    inline Region operator* (const Region& lhs, const Region& rhs)
    {
        Region result;
#if qPlatform_MacOS
        ::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
#elif qPlatform_Windows
        Verify (::CombineRgn (result, lhs, rhs, RGN_AND) != ERROR);
#endif
        return result;
    }
    inline Region operator+ (const Region& lhs, const Region& rhs)
    {
        Region result;
#if qPlatform_MacOS
        ::UnionRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
#elif qPlatform_Windows
        Verify (::CombineRgn (result, lhs, rhs, RGN_OR) != ERROR);
#endif
        return result;
    }
    inline Led_Rect operator* (const Led_Rect& lhs, const Led_Rect& rhs)
    {
        Led_Rect tmp = lhs;
        return tmp *= rhs;
    }
#if qPlatform_MacOS
    inline Region::Region (RgnHandle rgn)
        : fRgn (rgn)
        , fOwned (false)
    {
    }
    inline RgnHandle Region::GetOSRep () const
    {
        return fRgn;
    }
    inline RgnHandle Region::GetOSRep ()
    {
        return fRgn;
    }
#elif qPlatform_Windows
    inline Region::operator HRGN () const
    {
        return fRgn;
    }
    inline int Region::CombineRgn (Region* pRgn1, Region* pRgn2, int nCombineMode)
    {
        Require (pRgn1 != nullptr);
        Require (pRgn2 != nullptr);
        Require (fRgn != nullptr);
        return ::CombineRgn (fRgn, pRgn1->fRgn, pRgn2->fRgn, nCombineMode);
    }
    inline BOOL Region::PtInRegion (int x, int y) const
    {
        Require (fRgn != nullptr);
        return ::PtInRegion (fRgn, x, y);
    }
    inline BOOL Region::PtInRegion (POINT point) const
    {
        Require (fRgn != nullptr);
        return ::PtInRegion (fRgn, point.x, point.y);
    }

    inline BOOL Region::DeleteObject ()
    {
        if (fRgn == nullptr)
            return FALSE;
        HRGN r = fRgn;
        fRgn   = nullptr;
        return ::DeleteObject (r);
    }
#endif

    /*
     ********************************************************************************
     ************************************ TabStopList *******************************
     ********************************************************************************
     */
    inline DistanceType TabStopList::ComputeTabStopAfterPosition (Tablet* tablet, DistanceType afterPos) const
    {
        RequireNotNull (tablet);
        return tablet->CvtFromTWIPSH (ComputeTabStopAfterPosition (tablet->CvtToTWIPSH (afterPos)));
    }

#if qPlatform_Windows
    /*
     ********************************************************************************
     ************************************ Bitmap ********************************
     ********************************************************************************
     */
    inline Bitmap::~Bitmap ()
    {
        if (m_hObject != nullptr) {
            ::DeleteObject (m_hObject);
        }
    }
    inline void Bitmap::DeleteObject ()
    {
        if (m_hObject != nullptr) {
            ::DeleteObject (m_hObject);
            m_hObject = nullptr;
        }
    }
    inline Bitmap::operator HBITMAP () const
    {
        return m_hObject;
    }
    inline Led_Size Bitmap::GetImageSize () const
    {
        // only valid if m_hObject != nullptr
        RequireNotNull (m_hObject);
        return fImageSize;
    }
#endif

    /*
     ********************************************************************************
     *********************************** Tablet ********************************
     ********************************************************************************
     */
#if qPlatform_MacOS
    inline Tablet::operator GrafPtr () const
    {
        return fGrafPort;
    }
#elif qPlatform_Windows
    inline Tablet::operator HDC () const
    {
        return m_hDC;
    }
#endif
    /*
    @METHOD:        Tablet::CvtFromTWIPSV
    @DESCRIPTION:
        <p>See also @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
    */
    inline CoordinateType Tablet::CvtFromTWIPSV (TWIPS from) const
    {
#if qPlatform_Windows
        if (fLogPixelsV == 0) {
            fLogPixelsV = GetDeviceCaps (LOGPIXELSY);
        }
        POINT vpOrg;
        Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
        POINT wOrg;
        Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
        POINT x = vpOrg;
        x.y += ::MulDiv (from, fLogPixelsV, 1440);
        Verify (::DPtoLP (m_hAttribDC, &x, 1));
        x.y -= wOrg.y;
        Assert (x.x == wOrg.x);
        return x.y;
#else
        //TMPHACK - look at tablet resolution?
        return from * Globals::Get ().GetMainScreenLogPixelsV () / 1440;
//              return from / 20;   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Tablet::CvtFromTWIPSH
    @DESCRIPTION:
        <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
    */
    inline CoordinateType Tablet::CvtFromTWIPSH (TWIPS from) const
    {
#if qPlatform_Windows
        if (fLogPixelsH == 0) {
            fLogPixelsH = GetDeviceCaps (LOGPIXELSX);
        }
        POINT vpOrg;
        Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
        POINT wOrg;
        Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
        POINT x = vpOrg;
        x.x += ::MulDiv (from, fLogPixelsH, 1440);
        Verify (::DPtoLP (m_hAttribDC, &x, 1));
        x.x -= wOrg.x;
        Assert (x.y == wOrg.y);
        return x.x;
#else
        //TMPHACK - look at tablet resolution?
        return from * Globals::Get ().GetMainScreenLogPixelsH () / 1440;
//              return from / 20;   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Tablet::CvtToTWIPSV
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
        <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSH'.</p>
    */
    inline TWIPS Tablet::CvtToTWIPSV (CoordinateType from) const
    {
#if qPlatform_Windows
        if (fLogPixelsV == 0) {
            fLogPixelsV = GetDeviceCaps (LOGPIXELSY);
        }
        POINT vpOrg;
        Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
        POINT wOrg;
        Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
        POINT x = wOrg;
        x.y += from;
        Verify (::LPtoDP (m_hAttribDC, &x, 1));
        x.y -= vpOrg.y;
        Assert (x.x == wOrg.x);
        return TWIPS (::MulDiv (x.y, 1440, fLogPixelsV));
#else
        return TWIPS (from * 1440 / Globals::Get ().GetMainScreenLogPixelsV ());
//              return TWIPS (from * 20);   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Tablet::CvtToTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
        <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV'.</p>
    */
    inline TWIPS Tablet::CvtToTWIPSH (CoordinateType from) const
    {
#if qPlatform_Windows
        if (fLogPixelsH == 0) {
            fLogPixelsH = GetDeviceCaps (LOGPIXELSX);
        }
        POINT vpOrg;
        Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
        POINT wOrg;
        Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
        POINT x = wOrg;
        x.x += from;
        Verify (::LPtoDP (m_hAttribDC, &x, 1));
        x.x -= vpOrg.x;
        Assert (x.y == vpOrg.y);
        return TWIPS (::MulDiv (x.x, 1440, fLogPixelsH));
#else
        return TWIPS (from * 1440 / Globals::Get ().GetMainScreenLogPixelsH ());
//              return TWIPS (from * 20);   // assume 72dpi on mac
#endif
    }
#if qPlatform_MacOS
    inline void Tablet::SetPort ()
    {
        ::SetPort (fGrafPort);
    }
#elif qPlatform_Windows
    inline BOOL Tablet::BitBlt (int x, int y, int nWidth, int nHeight, Tablet* pSrcDC, int xSrc, int ySrc, DWORD dwRop)
    {
        AssertNotNull (m_hDC);
        return ::BitBlt (m_hDC, x, y, nWidth, nHeight, pSrcDC->m_hDC, xSrc, ySrc, dwRop);
    }
    inline BOOL Tablet::CreateCompatibleDC (Tablet* pDC)
    {
        Assert (m_hDC == nullptr);       // only attach once, detach on destroy
        Assert (m_hAttribDC == nullptr); // only attach to an empty DC

        m_hDC = ::CreateCompatibleDC (pDC == nullptr ? nullptr : pDC->m_hDC);
        if (m_hDC == nullptr)
            return FALSE;

        m_hAttribDC = m_hDC;
        fOwnsDC     = eOwnsDC;
        return TRUE;
    }
    inline COLORREF Tablet::SetTextColor (COLORREF crColor)
    {
        Assert (m_hDC != nullptr);
        COLORREF crRetVal = CLR_INVALID;

        if (m_hDC != m_hAttribDC)
            crRetVal = ::SetTextColor (m_hDC, crColor);
        if (m_hAttribDC != nullptr)
            crRetVal = ::SetTextColor (m_hAttribDC, crColor);
        return crRetVal;
    }
    inline COLORREF Tablet::SetBkColor (COLORREF crColor)
    {
        Assert (m_hDC != nullptr);
        COLORREF crRetVal = CLR_INVALID;

        if (m_hDC != m_hAttribDC)
            crRetVal = ::SetBkColor (m_hDC, crColor);
        if (m_hAttribDC != nullptr)
            crRetVal = ::SetBkColor (m_hAttribDC, crColor);
        return crRetVal;
    }
    inline HWND Tablet::GetWindow () const
    {
        Assert (m_hDC != nullptr);
        return ::WindowFromDC (m_hDC);
    }
    inline BOOL Tablet::IsPrinting () const
    {
        return m_bPrinting;
    }
    inline BOOL Tablet::RoundRect (int x1, int y1, int x2, int y2, int x3, int y3)
    {
        Assert (m_hDC != nullptr);
        return ::RoundRect (m_hDC, x1, y1, x2, y2, x3, y3);
    }
    inline BOOL Tablet::TextOut (int x, int y, LPCTSTR lpszString, int nCount)
    {
        Assert (m_hDC != nullptr);
        return ::TextOut (m_hDC, x, y, lpszString, nCount);
    }
#if 0
    inline  SIZE    Tablet::GetTextExtent (LPCTSTR lpszString, int nCount) const
    {
        Assert(m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetTextExtentPoint32 (m_hAttribDC, lpszString, nCount, &size));
        return size;
    }
#endif
    inline int Tablet::SetBkMode (int nBkMode)
    {
        Assert (m_hDC != nullptr);
        int nRetVal = 0;

        if (m_hDC != m_hAttribDC)
            nRetVal = ::SetBkMode (m_hDC, nBkMode);
        if (m_hAttribDC != nullptr)
            nRetVal = ::SetBkMode (m_hAttribDC, nBkMode);
        return nRetVal;
    }
    inline unsigned int Tablet::SetTextAlign (unsigned int nTextAlign)
    {
        Assert (m_hDC != nullptr);
        unsigned int nRetVal = 0;

        if (m_hDC != m_hAttribDC)
            nRetVal = ::SetTextAlign (m_hDC, nTextAlign);
        if (m_hAttribDC != nullptr)
            nRetVal = ::SetTextAlign (m_hAttribDC, nTextAlign);
        return nRetVal;
    }

    inline SIZE Tablet::GetWindowExt () const
    {
        Assert (m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetWindowExtEx (m_hAttribDC, &size));
        return size;
    }
    inline SIZE Tablet::GetViewportExt () const
    {
        Assert (m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetViewportExtEx (m_hAttribDC, &size));
        return size;
    }
    inline BOOL Tablet::Rectangle (int x1, int y1, int x2, int y2)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, x1, y1, x2, y2);
    }
    inline BOOL Tablet::Rectangle (const RECT& r)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, r.left, r.top, r.right, r.bottom);
    }
    inline BOOL Tablet::Rectangle (LPCRECT lpRect)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }
    inline BOOL Tablet::GetTextMetrics (LPTEXTMETRIC lpMetrics) const
    {
        Assert (m_hAttribDC != nullptr);
        return ::GetTextMetrics (m_hAttribDC, lpMetrics);
    }
    inline HBITMAP Tablet::SelectObject (HBITMAP hBitmap)
    {
        Assert (m_hDC != nullptr);
        return (HBITMAP)::SelectObject (m_hDC, hBitmap);
    }
#if defined(STRICT)
    inline HFONT Tablet::SelectObject (HFONT hFont)
    {
        Assert (m_hDC != nullptr);
        return (HFONT)::SelectObject (m_hDC, hFont);
    }
#endif
    inline POINT Tablet::SetWindowOrg (int x, int y)
    {
        Assert (m_hDC != nullptr);
        POINT point;
        memset (&point, 0, sizeof (point));
        if (m_hDC != m_hAttribDC)
            Verify (::SetWindowOrgEx (m_hDC, x, y, &point));
        if (m_hAttribDC != nullptr)
            Verify (::SetWindowOrgEx (m_hAttribDC, x, y, &point));
        return point;
    }
    inline int Tablet::GetDeviceCaps (int nIndex) const
    {
        Assert (m_hAttribDC != nullptr);
        return ::GetDeviceCaps (m_hAttribDC, nIndex);
    }
    inline BOOL Tablet::Attach (HDC hDC, Tablet::OwnDCControl ownsDC)
    {
        Assert (m_hDC == nullptr);       // only attach once, detach on destroy
        Assert (m_hAttribDC == nullptr); // only attach to an empty DC

        if (hDC == nullptr)
            return FALSE;

        m_hDC       = hDC;
        m_hAttribDC = m_hDC; // Default to same as output
        fOwnsDC     = ownsDC;
        return TRUE;
    }
    inline HDC Tablet::Detach ()
    {
        HDC hDC     = m_hDC;
        m_hAttribDC = nullptr;
        m_hDC       = nullptr;
        return hDC;
    }
#endif
    inline void Tablet::MoveTo (const Led_Point& to)
    {
#if qPlatform_MacOS
        ::MoveTo (to.h, to.v);
#elif qPlatform_Windows
        Assert (m_hDC != nullptr);
        Verify (::MoveToEx (m_hDC, to.h, to.v, nullptr));
#elif qStroika_FeatureSupported_XWindows
        fCurDrawLineLoc = to;
#endif
    }
    inline void Tablet::LineTo (const Led_Point& to)
    {
#if qPlatform_MacOS
        ::LineTo (to.h, to.v);
#elif qPlatform_Windows
        Assert (m_hDC != nullptr);
        Verify (::LineTo (m_hDC, to.h, to.v));
#elif qStroika_FeatureSupported_XWindows
        ::XDrawLine (fDisplay, fDrawable, fGC, fCurDrawLineLoc.h, fCurDrawLineLoc.v, to.h, to.v);
        fCurDrawLineLoc = to;
#endif
    }
    inline Region Tablet::GetClip () const
    {
        Region result;
#if qPlatform_MacOS
        const_cast<Tablet*> (this)->SetPort ();
        ::GetClip (result.GetOSRep ());
#elif qPlatform_Windows
        int r = ::GetClipRgn (*this, result);
        Assert (r == 0 or r == 1 or r == -1);
        if (r == 0) {
#if !qInternalErrorWithStaticRegionDeclaredInFunction
            static
#endif
                Region kWideOpened = Region (Led_Rect (-10000, -10000, 20000, 20000));
            result                 = kWideOpened;
        }
#else
        Assert (false); // NYI
#endif
        return result;
    }
    inline bool Tablet::GetClip (Region* r) const
    {
        RequireNotNull (r);
#if qPlatform_MacOS
        const_cast<Tablet*> (this)->SetPort ();
        ::GetClip (r->GetOSRep ());
        return true;
#elif qPlatform_Windows
        int res = ::GetClipRgn (*this, *r);
        Assert (res == 0 or res == 1 or res == -1);
        if (res == 0) {
            return false;
        }
        return true;
#else
        Assert (false); // NYI
        return false;
#endif
    }
    inline void Tablet::SetClip ()
    {
#if qPlatform_MacOS
        SetPort ();
        static Region kWideOpened = Region (Led_Rect (-10000, -10000, 20000, 20000));
        ::SetClip (kWideOpened.GetOSRep ());
#elif qPlatform_Windows
        Verify (::SelectClipRgn (*this, nullptr) != ERROR);
#elif qStroika_FeatureSupported_XWindows
        static Led_Rect kWideOpened = Led_Rect (-10000, -10000, 20000, 20000);
        XRectangle      xrectangle  = AsXRect (kWideOpened);
        ::XSetClipRectangles (fDisplay, fGC, 0, 0, &xrectangle, 1, Unsorted);
#else
        Assert (false); // NYI
#endif
    }
    inline void Tablet::SetClip (const Led_Rect& clipTo)
    {
#if qPlatform_MacOS
        SetPort ();
        ::SetClip (Region (clipTo).GetOSRep ());
#elif qPlatform_Windows
        Verify (::SelectClipRgn (*this, Region (clipTo)) != ERROR);
        Ensure (GetClip ().GetBoundingRect () == clipTo);
#elif qStroika_FeatureSupported_XWindows
        XRectangle xrectangle = AsXRect (clipTo);
        ::XSetClipRectangles (fDisplay, fGC, 0, 0, &xrectangle, 1, Unsorted);
#else
        Assert (false); // NYI
#endif
    }
    inline void Tablet::SetClip (const Region& clipTo)
    {
#if qPlatform_MacOS
        SetPort ();
        ::SetClip (clipTo.GetOSRep ());
#elif qPlatform_Windows
        Verify (::SelectClipRgn (*this, clipTo) != ERROR);
#else
        Assert (false); // NYI
#endif
    }

#if qPlatform_MacOS
    /*
     ********************************************************************************
     *********************************** Led_GetCurrentGDIPort *****************************
     ********************************************************************************
     */
    inline GrafPtr Led_GetCurrentGDIPort ()
    {
#if qPeekAtQuickDrawGlobals
        GrafPtr t = qd.thePort;
#else
        // This used to (and perhaps could in ifdefs) return qd.thePort -
        GrafPtr t = nullptr;
        ::GetPort (&t);
#endif
        EnsureNotNull (t);
        return t;
    }
#endif

    /*
     ********************************************************************************
     *********************************** Globals *****************************
     ********************************************************************************
     */
    inline Globals& Globals::Get ()
    {
        if (sThe == nullptr) {
            sThe = new Globals ();
        }
        return *sThe;
    }
    inline DistanceType Globals::GetMainScreenLogPixelsH () const
    {
        return fLogPixelsH;
    }
    inline DistanceType Globals::GetMainScreenLogPixelsV () const
    {
        return fLogPixelsV;
    }

    /*
     ********************************************************************************
     *********************************** Led_Rect ***********************************
     ********************************************************************************
     */
    inline Led_Rect operator- (const Led_Rect& lhs, const Led_Point& rhs)
    {
        return (Led_Rect (lhs.GetTop () - rhs.v, lhs.GetLeft () - rhs.h, lhs.GetHeight (), lhs.GetWidth ()));
    }
    inline Led_Rect operator+ (Led_Point p, Led_Rect r)
    {
        return (Led_Rect (r.GetTop () + p.v, r.GetLeft () + p.h, r.GetHeight (), r.GetWidth ()));
    }
    inline Led_Rect operator+ (Led_Rect r, Led_Point p)
    {
        return (Led_Rect (r.GetTop () + p.v, r.GetLeft () + p.h, r.GetHeight (), r.GetWidth ()));
    }

    /*
     ********************************************************************************
     *********************************** Intersect **********************************
     ********************************************************************************
     */
    inline bool Intersect (const Led_Rect& lhs, const Led_Rect& rhs)
    {
#if qPlatform_MacOS && qDebug
        bool gdiResult;
        {
            Rect mLHS;
            mLHS.top    = lhs.top;
            mLHS.left   = lhs.left;
            mLHS.bottom = lhs.bottom;
            mLHS.right  = lhs.right;
            Rect mRHS;
            mRHS.top    = rhs.top;
            mRHS.left   = rhs.left;
            mRHS.bottom = rhs.bottom;
            mRHS.right  = rhs.right;
            Rect ignored;
            gdiResult = ::SectRect (&mLHS, &mRHS, &ignored);
        }
#endif
        if (rhs.GetTop () >= lhs.GetBottom ()) {
#if qPlatform_MacOS
            Assert (not gdiResult);
#endif
            return (false);
        }
        else if (rhs.GetBottom () <= lhs.GetTop ()) {
#if qPlatform_MacOS
            Assert (not gdiResult);
#endif
            return (false);
        }
        else if (rhs.GetLeft () >= lhs.GetRight ()) {
#if qPlatform_MacOS
            Assert (not gdiResult);
#endif
            return (false);
        }
        else if (rhs.GetRight () <= lhs.GetLeft ()) {
#if qPlatform_MacOS
            Assert (not gdiResult);
#endif
            return (false);
        }

        if (rhs.GetHeight () == 0 or rhs.GetWidth () == 0 or lhs.GetHeight () == 0 or lhs.GetWidth () == 0) {
#if qPlatform_MacOS
            Assert (not gdiResult);
#endif
            return false;
        }

#if qPlatform_MacOS
        Assert (gdiResult);
#endif
        return (true);
    }
    inline bool Intersect (const Led_Rect& lhs, const Region& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        static RgnHandle lhsRgn = ::NewRgn ();
        ::SetRectRgn (lhsRgn, (short)lhs.left, (short)lhs.top, (short)lhs.right, (short)lhs.bottom);
        ::SectRgn (lhsRgn, rhs.GetOSRep (), result);
        return not ::EmptyRgn (result);
#elif qPlatform_Windows
        Region lhsRgn = lhs;
        Region result;
        return result.CombineRgn (&lhsRgn, const_cast<Region*> (&rhs), RGN_AND) != NULLREGION;
#endif
    }
    inline bool Intersect (const Region& lhs, const Led_Rect& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        static RgnHandle rhsRgn = ::NewRgn ();
        ::SetRectRgn (rhsRgn, (short)rhs.left, (short)rhs.top, (short)rhs.right, (short)rhs.bottom);
        ::SectRgn (lhs.GetOSRep (), rhsRgn, result);
        return not ::EmptyRgn (result);
#elif qPlatform_Windows
        Region rhsRgn = rhs;
        Region result;
        return result.CombineRgn (const_cast<Region*> (&lhs), &rhsRgn, RGN_AND) != NULLREGION;
#endif
    }
    inline bool Intersect (const Region& lhs, const Region& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        ::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result);
        return not ::EmptyRgn (result);
#elif qPlatform_Windows
        Region result;
        return result.CombineRgn (const_cast<Region*> (&lhs), const_cast<Region*> (&rhs), RGN_AND) != NULLREGION;
#endif
    }

    /*
     ********************************************************************************
     ******************************** Intersection **********************************
     ********************************************************************************
     */
    inline Led_Rect Intersection (const Led_Rect& lhs, const Led_Rect& rhs)
    {
        Led_Rect tmp = lhs;
        return tmp *= rhs;
    }

    /*
     ********************************************************************************
     ************************************ Led_Size **********************************
     ********************************************************************************
     */
    inline Led_Size operator+ (Led_Size lhs, Led_Size rhs)
    {
        return (Led_Size (lhs.v + rhs.v, lhs.h + rhs.h));
    }
    inline Led_Size operator* (int lhs, Led_Size rhs)
    {
        return (Led_Size (lhs * rhs.v, lhs * rhs.h));
    }

    /*
     ********************************************************************************
     *********************************** InsetRect **********************************
     ********************************************************************************
     */
    /*
    @METHOD:        InsetRect
    @DESCRIPTION:   <p>Utility routine to convert shrink (if vBy/hBy posative), or expand (if negative) the given @'Led_Rect'.
        NB: This routine pins the minimum output rect size (in each dimention) to be zero.</p>
    */
    inline Led_Rect InsetRect (const Led_Rect& r, int vBy, int hBy)
    {
        return Led_Rect (r.GetTop () + vBy, r.GetLeft () + hBy,
                         max (0L, CoordinateType (r.GetHeight ()) - 2 * vBy), max (0L, CoordinateType (r.GetWidth ()) - 2 * hBy));
    }

    /*
     ********************************************************************************
     *********************************** EnsureRectInRect ***************************
     ********************************************************************************
     */
    /*
    @METHOD:        EnsureRectInRect
    @DESCRIPTION:   <p>Utility routine to ensure the first rect is entirely enclosed in the second (enclosing) rectangle.
                Pin the edges so it fits.</p>
    */
    inline Led_Rect EnsureRectInRect (const Led_Rect& r, Led_Rect enlosingR)
    {
        DistanceType   winWidth  = min (r.GetWidth (), enlosingR.GetWidth ());
        DistanceType   winHeight = min (r.GetHeight (), enlosingR.GetHeight ());
        CoordinateType winLeft   = max (r.GetLeft (), enlosingR.GetLeft ());
        CoordinateType winTop    = max (r.GetTop (), enlosingR.GetTop ());

        DISABLE_COMPILER_MSC_WARNING_START (4018) //signed/unsigned mismatch
        if ((winLeft + winWidth) > enlosingR.GetRight ()) {
            winLeft = enlosingR.GetRight () - winWidth;
        }
        if ((winTop + winHeight) > enlosingR.GetBottom ()) {
            winTop = enlosingR.GetBottom () - winHeight;
        }
        DISABLE_COMPILER_MSC_WARNING_END (4018)
        return Led_Rect (winTop, winLeft, winHeight, winWidth);
    }

    /*
     ********************************************************************************
     ********************************* EnsureRectOnScreen ***************************
     ********************************************************************************
     */
    /*
    @METHOD:        EnsureRectOnScreen
    @DESCRIPTION:   <p>Utility routine to ensure the first rect (typically used for a window) fits on the sceen.
                Pin the edges so it fits. See also @'EnsureRectInRect'.</p>
    */
    inline Led_Rect EnsureRectOnScreen ([[maybe_unused]] Led_Rect& r)
    {
#if qPlatform_Windows
        // Get the limits of the 'workarea'
        RECT rWorkArea;
        memset (&rWorkArea, 0, sizeof (rWorkArea));
        BOOL bResult = SystemParametersInfo (SPI_GETWORKAREA, sizeof (RECT), &rWorkArea, 0);
        if (!bResult) {
            rWorkArea.left = rWorkArea.top = 0;
            rWorkArea.right                = GetSystemMetrics (SM_CXSCREEN);
            rWorkArea.bottom               = GetSystemMetrics (SM_CYSCREEN);
        }
        return EnsureRectInRect (r, AsLedRect (rWorkArea));
#else
        Assert (false); // NYI -
        return Led_Rect (0, 0, 0, 0);
#endif
    }

    /*
     ********************************************************************************
     ************************************* Led_Point ********************************
     ********************************************************************************
     */
#if qPlatform_MacOS
    inline Led_Point AsLedPoint (Point p)
    {
        return Led_Point (p.v, p.h);
    }
    inline Point AsQDPoint (Led_Point p)
    {
        Point p2;
        p2.v = p.v;
        p2.h = p.h;
        return p2;
    }
    inline Led_Rect AsLedRect (Rect r)
    {
        Led_Rect newR;
        newR.top    = r.top;
        newR.left   = r.left;
        newR.bottom = r.bottom;
        newR.right  = r.right;
        return newR;
    }
    inline Rect AsQDRect (Led_Rect r)
    {
        Rect newR; // SHOULD ASSERT NO OVERFLOWS!!!!
        newR.top    = r.top;
        newR.left   = r.left;
        newR.bottom = r.bottom;
        newR.right  = r.right;
        return (newR);
    }
    inline Led_Size AsLedSize (Point s)
    {
        return Led_Size (s.v, s.h);
    }
    inline Point GetRectOrigin (const Rect& r)
    {
        Point p;
        p.v = r.top;
        p.h = r.left;
        return p;
    }
    inline Point GetRectSize (const Rect& r)
    {
        Point p;
        p.v = r.bottom - r.top;
        p.h = r.right - r.left;
        return p;
    }
    inline short GetRectWidth (const Rect& r)
    {
        return (r.right - r.left);
    }
    inline short GetRectHeight (const Rect& r)
    {
        return (r.bottom - r.top);
    }
#elif qPlatform_Windows
    inline Led_Point AsLedPoint (POINT p)
    {
        return Led_Point (p.y, p.x);
    }
    inline POINT AsPOINT (Led_Point p)
    {
        POINT newP;
        newP.x = p.h;
        newP.y = p.v;
        return newP;
    }
    inline Led_Rect AsLedRect (RECT r)
    {
        return Led_Rect (r.top, r.left, r.bottom - r.top, r.right - r.left);
    }
    inline RECT AsRECT (Led_Rect r)
    {
        RECT newR;
        newR.top    = r.GetTop ();
        newR.left   = r.GetLeft ();
        newR.bottom = r.GetBottom ();
        newR.right  = r.GetRight ();
        return newR;
    }
    inline SIZE AsSIZE (Led_Size s)
    {
        SIZE result;
        result.cx = s.h;
        result.cy = s.v;
        return result;
    }
    inline Led_Size AsLedSize (SIZE s)
    {
        Led_Size result;
        result.h = s.cx;
        result.v = s.cy;
        return result;
    }
#elif qStroika_FeatureSupported_XWindows
    inline Led_Rect AsLedRect (const XRectangle& r)
    {
        return Led_Rect (r.y, r.x, r.height, r.width);
    }
    inline XRectangle AsXRect (const Led_Rect& r)
    {
        XRectangle newR;
        newR.x = r.GetLeft ();
        newR.y = r.GetTop ();
        newR.height = r.GetHeight ();
        newR.width = r.GetWidth ();
        return newR;
    }
#endif

    /*
    @METHOD:        Led_CvtScreenPixelsToTWIPSV
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.
        <p>See also @'Led_CvtScreenPixelsToTWIPSH'.</p>
    */
    inline TWIPS Led_CvtScreenPixelsToTWIPSV (CoordinateType from)
    {
#if qPlatform_Windows
        return TWIPS (::MulDiv (from, 1440, Globals::Get ().GetMainScreenLogPixelsV ()));
#else
        return TWIPS (from * 1440 / Globals::Get ().GetMainScreenLogPixelsV ());
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsToTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.
        <p>See also @'Led_CvtScreenPixelsToTWIPSV'.</p>
    */
    inline TWIPS Led_CvtScreenPixelsToTWIPSH (CoordinateType from)
    {
#if qPlatform_Windows
        return TWIPS (::MulDiv (from, 1440, Globals::Get ().GetMainScreenLogPixelsH ()));
#else
        return TWIPS (from * 1440 / Globals::Get ().GetMainScreenLogPixelsH ());
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsFromTWIPSV
    @DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
        <p>See also @'Led_CvtScreenPixelsFromTWIPSH'.</p>
    */
    inline CoordinateType Led_CvtScreenPixelsFromTWIPSV (TWIPS from)
    {
#if qPlatform_Windows
        return ::MulDiv (from, Globals::Get ().GetMainScreenLogPixelsV (), 1440);
#else
        return TWIPS{from * Globals::Get ().GetMainScreenLogPixelsV () / 1440};
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsFromTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
        <p>See also @'Led_CvtScreenPixelsFromTWIPSV'.</p>
    */
    inline CoordinateType Led_CvtScreenPixelsFromTWIPSH (TWIPS from)
    {
#if qPlatform_Windows
        return ::MulDiv (from, Globals::Get ().GetMainScreenLogPixelsH (), 1440);
#else
        return TWIPS{from * Globals::Get ().GetMainScreenLogPixelsH () / 1440};
#endif
    }

    /*
     ********************************************************************************
     ******************************* FontMetrics ********************************
     ********************************************************************************
     */
    inline FontMetrics::FontMetrics ()
        : fPlatformSpecific ()
    {
        (void)::memset (&fPlatformSpecific, 0, sizeof (fPlatformSpecific));
    }
#if qPlatform_MacOS
    inline FontMetrics::FontMetrics (const FontInfo& from)
        : fPlatformSpecific (from)
    {
    }
#elif qPlatform_Windows
    inline FontMetrics::FontMetrics (const TEXTMETRIC& from)
        : fPlatformSpecific (from)
    {
    }
#elif qStroika_FeatureSupported_XWindows
    inline FontMetrics::FontMetrics (const FontMetrics::PlatformSpecific& from)
        : fPlatformSpecific (from)
    {
    }
#endif
    inline FontMetrics::FontMetrics (const FontMetrics& from)
        : fPlatformSpecific (from.fPlatformSpecific)
    {
    }
    inline const FontMetrics& FontMetrics::operator= (const FontMetrics& rhs)
    {
        fPlatformSpecific = rhs.fPlatformSpecific;
        return *this;
    }
    inline DistanceType FontMetrics::GetAscent () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.ascent);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmAscent);
#elif qStroika_FeatureSupported_XWindows
        return fPlatformSpecific.fAscent;
#endif
    }
    inline DistanceType FontMetrics::GetDescent () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.descent);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmDescent);
#elif qStroika_FeatureSupported_XWindows
        return fPlatformSpecific.fDescent;
#endif
    }
    inline DistanceType FontMetrics::GetLeading () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.leading);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmExternalLeading);
#elif qStroika_FeatureSupported_XWindows
        return (fPlatformSpecific.fLeading);
#endif
    }
    inline DistanceType FontMetrics::GetHeight () const
    {
#if qPlatform_Windows
        Assert (fPlatformSpecific.tmHeight >= 0);
        Assert (GetAscent () + GetDescent () == DistanceType (fPlatformSpecific.tmHeight));
#endif
        return (GetAscent () + GetDescent ());
    }
    inline DistanceType FontMetrics::GetLineHeight () const
    {
        return (GetAscent () + GetDescent () + GetLeading ());
    }
    inline nonvirtual DistanceType FontMetrics::GetMaxCharacterWidth () const
    {
#if qPlatform_MacOS
        return fPlatformSpecific.widMax;
#elif qPlatform_Windows
        return fPlatformSpecific.tmMaxCharWidth;
#elif qStroika_FeatureSupported_XWindows
        return fPlatformSpecific.fMaxCharWidth;
#endif
    }
#if qPlatform_Windows
    inline nonvirtual DistanceType FontMetrics::GetAveCharacterWidth () const
    {
        return fPlatformSpecific.tmAveCharWidth;
    }
#endif
#if qPlatform_MacOS
    inline FontMetrics::operator const FontInfo* () const
    {
        return &fPlatformSpecific;
    }
    inline FontMetrics::operator FontInfo* ()
    {
        return (&fPlatformSpecific);
    }
#elif qPlatform_Windows
    inline FontMetrics::operator const TEXTMETRIC* () const
    {
        return &fPlatformSpecific;
    }
    inline FontMetrics::operator TEXTMETRIC* ()
    {
        return &fPlatformSpecific;
    }
#endif

    /*
     ********************************************************************************
     ************************************* Color ************************************
     ********************************************************************************
     */
    constexpr inline Color::Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue)
        : fRed{redValue}
        , fGreen{greenValue}
        , fBlue{blueValue}
    {
    }
#if qPlatform_MacOS
    inline Color::Color (const RGBColor& rgbColor)
        : fRed (rgbColor.red)
        , fGreen (rgbColor.green)
        , fBlue (rgbColor.blue)
    {
    }
#elif qPlatform_Windows
    inline Color::Color (COLORREF colorRef)
        : fRed (static_cast<ColorValue> (GetRValue (colorRef)) << 8)
        , fGreen (static_cast<ColorValue> (GetGValue (colorRef)) << 8)
        , fBlue (static_cast<ColorValue> (GetBValue (colorRef)) << 8)
    {
    }
#endif
    inline Color::ColorValue Color::GetRed () const
    {
        return fRed;
    }
    inline Color::ColorValue Color::GetGreen () const
    {
        return fGreen;
    }
    inline Color::ColorValue Color::GetBlue () const
    {
        return fBlue;
    }
#if qPlatform_MacOS
    inline RGBColor Color::GetOSRep () const
    {
        RGBColor r;
        r.red   = fRed;
        r.green = fGreen;
        r.blue  = fBlue;
        return r;
    }
#elif qPlatform_Windows
    inline COLORREF Color::GetOSRep () const
    {
        return RGB (fRed >> 8, fGreen >> 8, fBlue >> 8);
    }
#endif
    inline Color operator* (Color lhs, float factor)
    {
        using CV = Color::ColorValue;
        return Color (
            static_cast<CV> (lhs.GetRed () * factor),
            static_cast<CV> (lhs.GetGreen () * factor),
            static_cast<CV> (lhs.GetBlue () * factor));
    }
    inline Color operator/ (Color lhs, float divBy)
    {
        return Color (static_cast<Color::ColorValue> (lhs.GetRed () / divBy), static_cast<Color::ColorValue> (lhs.GetGreen () / divBy), static_cast<Color::ColorValue> (lhs.GetBlue () / divBy));
    }
    inline Color operator+ (Color lhs, Color rhs)
    {
        return Color (lhs.GetRed () + rhs.GetRed (), lhs.GetGreen () + rhs.GetGreen (), lhs.GetBlue () + rhs.GetBlue ());
    }
    inline Color operator- (Color lhs, Color rhs)
    {
        return Color (lhs.GetRed () - rhs.GetRed (), lhs.GetGreen () - rhs.GetGreen (), lhs.GetBlue () - rhs.GetBlue ());
    }
    inline unsigned int Distance_Squared (Color lhs, Color rhs)
    {
        int          rDiff = static_cast<int> (lhs.GetRed ()) - static_cast<int> (rhs.GetRed ());
        int          gDiff = static_cast<int> (lhs.GetGreen ()) - static_cast<int> (rhs.GetGreen ());
        int          bDiff = static_cast<int> (lhs.GetBlue ()) - static_cast<int> (rhs.GetBlue ());
        unsigned int sum   = rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
        return sum;
    }
    inline unsigned int Distance (Color lhs, Color rhs)
    {
        return static_cast<unsigned int> (::sqrt (static_cast<float> (Distance_Squared (lhs, rhs))));
    }
#if qPlatform_Windows
    inline unsigned int Distance_Squared (COLORREF lhs, COLORREF rhs)
    {
        int          rDiff = static_cast<int> (GetRValue (lhs)) - static_cast<int> (GetRValue (rhs));
        int          gDiff = static_cast<int> (GetGValue (lhs)) - static_cast<int> (GetGValue (rhs));
        int          bDiff = static_cast<int> (GetBValue (lhs)) - static_cast<int> (GetBValue (rhs));
        unsigned int sum   = rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
        return sum;
    }
#endif
}

namespace std {
    template <>
    struct less<Stroika::Frameworks::Led::Color> {
        bool operator() (const Stroika::Frameworks::Led::Color& _Left, const Stroika::Frameworks::Led::Color& _Right) const
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
}

namespace Stroika::Frameworks::Led {
#if qPlatform_MacOS
    inline Pen::Pen (short penStyle = srcCopy, const Pattern* penPat = &kBlackPattern, const Color& color = Color::kBlack)
        : fPenStyle (penStyle)
        , fPenPat (*penPat)
        , fPenColor (color)
    {
    }
#endif
#if qPlatform_Windows
    inline Pen::Pen (int nPenStyle, int nWidth, COLORREF crColor)
        : m_hObject{nullptr}
    {
        if (!Attach (::CreatePen (nPenStyle, nWidth, crColor)))
            Foundation::Execution::Throw (bad_alloc{});
    }
    inline Pen::~Pen ()
    {
        (void)DeleteObject ();
    }
    inline Pen::operator HPEN () const
    {
        return m_hObject;
    }
    inline BOOL Pen::Attach (HPEN hObject)
    {
        Assert (m_hObject == nullptr); // only attach once, detach on destroy
        if (hObject == nullptr)
            return FALSE;
        m_hObject = hObject;
        return TRUE;
    }
    inline BOOL Pen::DeleteObject ()
    {
        if (m_hObject == nullptr)
            return FALSE;
        HPEN h    = m_hObject;
        m_hObject = nullptr;
        return ::DeleteObject (h);
    }
}
#endif

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************************* LineSpacing ******************************
     ********************************************************************************
     */
    inline LineSpacing::LineSpacing (Rule rule)
        : fRule{rule}
    {
        Require (rule == eSingleSpace or rule == eOnePointFiveSpace or rule == eDoubleSpace);
    }
    inline LineSpacing::LineSpacing (Rule rule, TWIPS twips)
        : fRule{rule}
        , fArg{static_cast<unsigned> (twips)}
    {
        Require (rule == eAtLeastTWIPSSpacing or rule == eExactTWIPSSpacing);
    }
    inline LineSpacing::LineSpacing (Rule rule, unsigned lineCount)
        : fRule{rule}
        , fArg{lineCount}
    {
        Require (rule == eExactLinesSpacing);
        switch (lineCount) {
            case 20:
                fRule = eSingleSpace;
                break;
            case 30:
                fRule = eOnePointFiveSpace;
                break;
            case 40:
                fRule = eDoubleSpace;
                break;
        }
    }
    inline bool LineSpacing::operator== (LineSpacing rhs) const
    {
        if (fRule != rhs.fRule) {
            return false;
        }
        if (fRule == LineSpacing::eAtLeastTWIPSSpacing or fRule == LineSpacing::eExactTWIPSSpacing or fRule == LineSpacing::eExactLinesSpacing) {
            if (fArg != rhs.fArg) {
                return false;
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     ********************** FontSpecification::FontNameSpecifier ********************
     ********************************************************************************
     */
#if qPlatform_Windows
    inline FontSpecification::FontNameSpecifier::FontNameSpecifier ()
    {
        fName[0] = '\0';
    }
#endif

    /*
     ********************************************************************************
     ****************************** FontSpecification ***************************
     ********************************************************************************
     */
    inline FontSpecification::FontSpecification ()
        :
#if qPlatform_MacOS
        fFontSpecifier (0)
        , fFontSize (0)
        , fFontStyle (0)
        ,
#elif qPlatform_Windows
            fFontInfo ()
            ,
#elif qStroika_FeatureSupported_XWindows
            fFontFamily ()
            , fBold (false)
            , fItalics (false)
            , fUnderline (false)
            , fFontSize (0)
            ,
#endif
        fSubOrSuperScript (eNoSubOrSuperscript)
        , fTextColor (Color::kBlack)
    {
#if qPlatform_Windows
        (void)::memset (&fFontInfo, 0, sizeof (fFontInfo));
#endif
    }
    inline FontSpecification::FontSpecification (const IncrementalFontSpecification& from)
        :
#if qPlatform_MacOS
        fFontSpecifier (((const FontSpecification&)from).fFontSpecifier)
        , fFontSize (((const FontSpecification&)from).fFontSize)
        , fFontStyle (((const FontSpecification&)from).fFontStyle)
        ,
#elif qPlatform_Windows
            fFontInfo (((const FontSpecification&)from).fFontInfo)
            ,
#elif qStroika_FeatureSupported_XWindows
            fFontFamily (from.fFontFamily)
            , fBold (from.fBold)
            , fItalics (from.fItalics)
            , fUnderline (from.fUnderline)
            , fFontSize (from.fFontSize)
            ,
#endif
        fSubOrSuperScript (((const FontSpecification&)from).fSubOrSuperScript)
        , fTextColor (((const FontSpecification&)from).fTextColor)
    {
    }
    /*
    @METHOD:        FontSpecification::GetFontName
    @DESCRIPTION:   <p>Retrieve the 'FontName' attribute of the given font specification. This name is a string, and corresponds to the font family name.</p>
                    <p>See also @'FontSpecification::SetFontName'.</p>
    */
    inline Led_SDK_String FontSpecification::GetFontName () const
    {
#if qPlatform_MacOS
        Str255 fontName;
        ::GetFontName (fFontSpecifier, fontName);
        size_t len = fontName[0];
        return string{(const char*)&fontName[1], len};
#elif qPlatform_Windows
            return fFontInfo.lfFaceName;
#elif qStroika_FeatureSupported_XWindows
            return fFontFamily;
#endif
    }
    // FontName info
    inline FontSpecification::FontNameSpecifier FontSpecification::GetFontNameSpecifier () const
    {
#if qPlatform_MacOS
        return fFontSpecifier;
#elif qPlatform_Windows
            return fFontInfo.lfFaceName;
#elif qStroika_FeatureSupported_XWindows
            return fFontFamily;
#endif
    }
    // Style info
    /*
    @METHOD:        FontSpecification::GetStyle_Plain
    @DESCRIPTION:   <p>Checks all the various font 'style' attributes, such as 'bold', or 'italic'. Returns true iff all
                of these styles are not set.</p>
                <p>See also @'FontSpecification::SetStyle_Plain'.</p>
    */
    inline bool FontSpecification::GetStyle_Plain () const
    {
        if (fSubOrSuperScript != eNoSubOrSuperscript) {
            return false;
        }
#if qPlatform_MacOS
        return (fFontStyle == 0);
#elif qPlatform_Windows
            return (
                fFontInfo.lfItalic == false and
                fFontInfo.lfWeight <= FW_NORMAL and
                fFontInfo.lfUnderline == false and
                fFontInfo.lfStrikeOut == false);
#elif qStroika_FeatureSupported_XWindows
            return not fBold and not fItalics and not fUnderline;
#endif
    }
    /*
    @METHOD:        FontSpecification::SetStyle_Plain
    @DESCRIPTION:   <p>Clears all 'style' attributes, such as 'bold', or 'italic'.</p>
                <p>See also @'FontSpecification::GetStyle_Plain'.</p>
    */
    inline void FontSpecification::SetStyle_Plain ()
    {
        fSubOrSuperScript = eNoSubOrSuperscript;
#if qPlatform_MacOS
        fFontStyle = 0;
#elif qPlatform_Windows
            fFontInfo.lfItalic    = false;
            fFontInfo.lfWeight    = FW_NORMAL;
            fFontInfo.lfUnderline = false;
            fFontInfo.lfStrikeOut = false;
#elif qStroika_FeatureSupported_XWindows
            fBold = false;
            fItalics = false;
            fUnderline = false;
#endif
    }
    inline bool FontSpecification::GetStyle_Bold () const
    {
#if qPlatform_MacOS
        return (fFontStyle & bold);
#elif qPlatform_Windows
            return (fFontInfo.lfWeight > FW_NORMAL);
#elif qStroika_FeatureSupported_XWindows
            return fBold;
#endif
    }
    inline void FontSpecification::SetStyle_Bold (bool isBold)
    {
#if qPlatform_MacOS
        if (isBold) {
            fFontStyle |= bold;
        }
        else {
            fFontStyle &= ~bold;
        }
#elif qPlatform_Windows
            fFontInfo.lfWeight = isBold ? FW_BOLD : FW_NORMAL;
#elif qStroika_FeatureSupported_XWindows
            fBold = isBold;
#endif
    }
    inline bool FontSpecification::GetStyle_Italic () const
    {
#if qPlatform_MacOS
        return (fFontStyle & italic);
#elif qPlatform_Windows
            return !!fFontInfo.lfItalic;
#elif qStroika_FeatureSupported_XWindows
            return fItalics;
#endif
    }
    inline void FontSpecification::SetStyle_Italic (bool isItalic)
    {
#if qPlatform_MacOS
        if (isItalic) {
            fFontStyle |= italic;
        }
        else {
            fFontStyle &= ~italic;
        }
#elif qPlatform_Windows
            fFontInfo.lfItalic = isItalic;
#elif qStroika_FeatureSupported_XWindows
            fItalics = isItalic;
#endif
    }
    inline bool FontSpecification::GetStyle_Underline () const
    {
#if qPlatform_MacOS
        return (fFontStyle & underline);
#elif qPlatform_Windows
            return !!fFontInfo.lfUnderline;
#elif qStroika_FeatureSupported_XWindows
            return fUnderline;
#endif
    }
    inline void FontSpecification::SetStyle_Underline (bool isUnderline)
    {
#if qPlatform_MacOS
        if (isUnderline) {
            fFontStyle |= underline;
        }
        else {
            fFontStyle &= ~underline;
        }
#elif qPlatform_Windows
            fFontInfo.lfUnderline = isUnderline;
#elif qStroika_FeatureSupported_XWindows
            fUnderline = isUnderline;
#endif
    }
    inline FontSpecification::SubOrSuperScript FontSpecification::GetStyle_SubOrSuperScript () const
    {
        return fSubOrSuperScript;
    }
    inline void FontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
    {
        fSubOrSuperScript = subOrSuperScript;
    }
#if qPlatform_MacOS
    inline bool FontSpecification::GetStyle_Outline () const
    {
        return (fFontStyle & outline);
    }
    inline void FontSpecification::SetStyle_Outline (bool isOutline)
    {
        if (isOutline) {
            fFontStyle |= outline;
        }
        else {
            fFontStyle &= ~outline;
        }
    }
    inline bool FontSpecification::GetStyle_Shadow () const
    {
        return (fFontStyle & shadow);
    }
    inline void FontSpecification::SetStyle_Shadow (bool isShadow)
    {
        if (isShadow) {
            fFontStyle |= shadow;
        }
        else {
            fFontStyle &= ~shadow;
        }
    }
    inline bool FontSpecification::GetStyle_Condensed () const
    {
        return (fFontStyle & condense);
    }
    inline void FontSpecification::SetStyle_Condensed (bool isCondensed)
    {
        if (isCondensed) {
            fFontStyle |= condense;
        }
        else {
            fFontStyle &= ~condense;
        }
    }
    inline bool FontSpecification::GetStyle_Extended () const
    {
        return (fFontStyle & extend);
    }
    inline void FontSpecification::SetStyle_Extended (bool isExtended)
    {
        if (isExtended) {
            fFontStyle |= extend;
        }
        else {
            fFontStyle &= ~extend;
        }
    }
#elif qPlatform_Windows
        inline bool FontSpecification::GetStyle_Strikeout () const
        {
            return !!fFontInfo.lfStrikeOut;
        }
        inline void FontSpecification::SetStyle_Strikeout (bool isStrikeout)
        {
            fFontInfo.lfStrikeOut = isStrikeout;
        }
#endif
    // FontSize info
    inline FontSpecification::FontSize FontSpecification::GetPointSize () const
    {
#if qPlatform_MacOS
        return fFontSize;
#elif qPlatform_Windows
            if (fFontInfo.lfHeight >= 0) {
                // I probably should be doing some magic here with subtracing internal leading, or something like that from this value -
                // See TextImager::GetStaticDefaultFont () and Win32 SDK docs for LOGFONT
                // LGP 960222
                WindowDC   screenDC (nullptr);
                FontObject font;
                Verify (font.CreateFontIndirect (&fFontInfo));
                HFONT      oldFont = screenDC.SelectObject (font);
                TEXTMETRIC tms;
                screenDC.GetTextMetrics (&tms);
                screenDC.SelectObject (oldFont);
                return (unsigned short)::MulDiv (tms.tmHeight, 72, Globals::Get ().GetMainScreenLogPixelsV ());
            }
            else {
                return static_cast<unsigned short> (::MulDiv (-fFontInfo.lfHeight, 72, Globals::Get ().GetMainScreenLogPixelsV ()));
            }
#elif qStroika_FeatureSupported_XWindows
            return fFontSize;
#endif
    }
    inline void FontSpecification::SetPointSize (FontSize pointSize)
    {
#if qPlatform_MacOS
        fFontSize = pointSize;
#elif qPlatform_Windows
            fFontInfo.lfHeight = ::MulDiv (-long (pointSize), Globals::Get ().GetMainScreenLogPixelsV (), 72);
#elif qStroika_FeatureSupported_XWindows
            fFontSize = pointSize;
#endif
    }
#if qPlatform_Windows
    inline long FontSpecification::PeekAtTMHeight () const
    {
        return fFontInfo.lfHeight;
    }
    inline void FontSpecification::PokeAtTMHeight (long tmHeight)
    {
        fFontInfo.lfHeight = tmHeight;
    }
#endif
    inline Color FontSpecification::GetTextColor () const
    {
        return fTextColor;
    }
    inline void FontSpecification::SetTextColor (const Color& textColor)
    {
        fTextColor = textColor;
    }
#if qPlatform_MacOS
    inline void FontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
    {
        if (fontID != nullptr) {
            *fontID = fFontSpecifier;
        }
        if (fontSize != nullptr) {
            *fontSize = fFontSize;
        }
        if (fontStyle != nullptr) {
            *fontStyle = fFontStyle;
        }
    }
    inline void FontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
    {
        fFontSpecifier = fontID;
        fFontSize      = fontSize;
        fFontStyle     = fontStyle;
    }
#elif qPlatform_Windows
        inline LOGFONT FontSpecification::GetOSRep () const
        {
            return fFontInfo;
        }
        inline void FontSpecification::GetOSRep (LOGFONT* logFont) const
        {
            RequireNotNull (logFont);
            *logFont = fFontInfo;
        }
        inline void FontSpecification::SetOSRep (LOGFONT logFont)
        {
            fFontInfo = logFont;
        }
        inline void FontSpecification::LightSetOSRep (LOGFONT logFont)
        {
            fFontInfo                  = logFont;
            fFontInfo.lfWidth          = 0;
            fFontInfo.lfEscapement     = 0;
            fFontInfo.lfOrientation    = 0;
            fFontInfo.lfCharSet        = DEFAULT_CHARSET;
            fFontInfo.lfOutPrecision   = 0;
            fFontInfo.lfClipPrecision  = 0;
            fFontInfo.lfQuality        = 0;
            fFontInfo.lfPitchAndFamily = 0;
        }
#endif
#if qPlatform_Windows
    inline FontSpecification::FontSpecification (const LOGFONT& logFont)
        : fFontInfo ()
        , fSubOrSuperScript (eNoSubOrSuperscript)
        , fTextColor (Color::kBlack)
    {
        LightSetOSRep (logFont);
    }
#endif
    inline bool FontSpecification::operator== (const FontSpecification& rhs) const
    {
        const FontSpecification& lhs = *this;
        // FontName Info
        if (lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ()) {
            return false;
        }

// Style Info
#if qPlatform_MacOS
        Style lhsStyle;
        Style rhsStyle;
        lhs.GetOSRep (nullptr, nullptr, &lhsStyle);
        rhs.GetOSRep (nullptr, nullptr, &rhsStyle);
        if (lhsStyle != rhsStyle) {
            return false;
        }
#elif qPlatform_Windows
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
#elif qStroika_FeatureSupported_XWindows
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
#if qPlatform_Windows
        // Speed tweek to avoid divide and getdevicecaps crap...
        if (lhs.PeekAtTMHeight () == rhs.PeekAtTMHeight ()) {
            return true;
        }
        else if ((lhs.PeekAtTMHeight () > 0) == (rhs.PeekAtTMHeight () > 0)) {
            return false; // if same sign, we can just compare for equality, and since they
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
    inline void FontSpecification::MergeIn (const IncrementalFontSpecification& addInTheseAttributes)
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
#if qPlatform_MacOS
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
#elif qPlatform_Windows
            if (addInTheseAttributes.GetStyle_Strikeout_Valid ()) {
                SetStyle_Strikeout (addInTheseAttributes.GetStyle_Strikeout ());
            }
#endif

        // Font Size
        if (addInTheseAttributes.GetPointSize_Valid ()) {
#if qPlatform_Windows
            // speed tweek - avoid costly conversion to 'points'. All we want todo is copy the tmHeight field!
            PokeAtTMHeight (addInTheseAttributes.PeekAtTMHeight ());
#else
                SetPointSize (addInTheseAttributes.GetPointSize ());
#endif
        }
        if (addInTheseAttributes.GetPointSizeIncrement_Valid ()) {
            short pointSize = GetPointSize ();
            pointSize += addInTheseAttributes.GetPointSizeIncrement ();
            if (pointSize <= 0) { // never let point size get any smaller...
                pointSize = 1;
            }
            SetPointSize (pointSize);
        }

        // Text Color
        if (addInTheseAttributes.GetTextColor_Valid ()) {
            SetTextColor (addInTheseAttributes.GetTextColor ());
        }

#if qPlatform_Windows
        // could have done somewhat earlier, but if so, must be more careful about what else gets changed... (like textcolor not part of this guy)
        if (addInTheseAttributes.GetDidSetOSRepCallFlag ()) {
            LOGFONT lf;
            addInTheseAttributes.GetOSRep (&lf);
            SetOSRep (lf);
        }
#endif
    }

    /*
     ********************************************************************************
     ********************** IncrementalFontSpecification ************************
     ********************************************************************************
     */
    inline IncrementalFontSpecification::IncrementalFontSpecification ()
        : fFontSpecifierValid (false)
        , fStyleValid_Bold (false)
        , fStyleValid_Italic (false)
        , fStyleValid_Underline (false)
        , fStyleValid_SubOrSuperScript (false)
        ,
#if qPlatform_MacOS
        fStyleValid_Outline (false)
        , fStyleValid_Shadow (false)
        , fStyleValid_Condensed (false)
        , fStyleValid_Extended (false)
        ,
#elif qPlatform_Windows
            fStyleValid_Strikeout (false)
            , fDidSetOSRepCallFlag (false)
            ,
#endif
        fFontSizeValid (false)
        , fFontSizeIncrementValid (false)
        , fTextColorValid (false)
    {
    }
    inline IncrementalFontSpecification::IncrementalFontSpecification (const FontSpecification& fontSpec)
        : FontSpecification{fontSpec}
        , fFontSpecifierValid (true)
        , fStyleValid_Bold (true)
        , fStyleValid_Italic (true)
        , fStyleValid_Underline (true)
        , fStyleValid_SubOrSuperScript (true)
        ,
#if qPlatform_MacOS
        fStyleValid_Outline (true)
        , fStyleValid_Shadow (true)
        , fStyleValid_Condensed (true)
        , fStyleValid_Extended (true)
        ,
#elif qPlatform_Windows
            fStyleValid_Strikeout (true)
            , fDidSetOSRepCallFlag (true)
            ,
#endif
        fFontSizeValid (true)
        , fFontSizeIncrementValid (false)
        , fTextColorValid (true)
    {
    }
    inline FontSpecification::FontNameSpecifier IncrementalFontSpecification::GetFontNameSpecifier () const
    {
        Require (fFontSpecifierValid);
        return inherited::GetFontNameSpecifier ();
    }
    inline bool IncrementalFontSpecification::GetFontNameSpecifier_Valid () const
    {
        return (fFontSpecifierValid);
    }
    inline void IncrementalFontSpecification::InvalidateFontNameSpecifier ()
    {
        fFontSpecifierValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
    {
        fFontSpecifierValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetFontNameSpecifier (fontNameSpecifier);
    }
    inline void IncrementalFontSpecification::SetFontName (const Led_SDK_String& fontName)
    {
        fFontSpecifierValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetFontName (fontName);
    }
    inline bool IncrementalFontSpecification::GetStyle_Plain () const
    {
        Require (fStyleValid_Bold);
        Require (fStyleValid_Italic);
        Require (fStyleValid_Underline);
        Require (fStyleValid_SubOrSuperScript);
#if qPlatform_MacOS
        Require (fStyleValid_Outline);
        Require (fStyleValid_Shadow);
        Require (fStyleValid_Condensed);
        Require (fStyleValid_Extended);
#elif qPlatform_Windows
            Require (fStyleValid_Strikeout);
#endif
        return inherited::GetStyle_Plain ();
    }
    inline bool IncrementalFontSpecification::GetStyle_Plain_Valid () const
    {
        bool isValid = fStyleValid_Bold and fStyleValid_Italic and fStyleValid_Underline and fStyleValid_SubOrSuperScript;
#if qPlatform_MacOS
        isValid = isValid and fStyleValid_Outline and fStyleValid_Shadow and fStyleValid_Condensed and fStyleValid_Extended;
#elif qPlatform_Windows
            isValid = isValid and fStyleValid_Strikeout;
#endif
        return isValid;
    }
    inline void IncrementalFontSpecification::InvalidateStyles ()
    {
        fStyleValid_Bold             = false;
        fStyleValid_Italic           = false;
        fStyleValid_Underline        = false;
        fStyleValid_SubOrSuperScript = false;
#if qPlatform_MacOS
        fStyleValid_Outline   = false;
        fStyleValid_Shadow    = false;
        fStyleValid_Condensed = false;
        fStyleValid_Extended  = false;
#elif qPlatform_Windows
            fStyleValid_Strikeout = false;
            fDidSetOSRepCallFlag  = false;
#endif
    }
    inline void IncrementalFontSpecification::SetStyle_Plain ()
    {
        fStyleValid_Bold             = true;
        fStyleValid_Italic           = true;
        fStyleValid_Underline        = true;
        fStyleValid_SubOrSuperScript = true;
#if qPlatform_MacOS
        fStyleValid_Outline   = true;
        fStyleValid_Shadow    = true;
        fStyleValid_Condensed = true;
        fStyleValid_Extended  = true;
#elif qPlatform_Windows
            fStyleValid_Strikeout = true;
            fDidSetOSRepCallFlag  = false;
#endif
        inherited::SetStyle_Plain ();
    }
    inline bool IncrementalFontSpecification::GetStyle_Bold () const
    {
        Require (fStyleValid_Bold);
        return inherited::GetStyle_Bold ();
    }
    inline bool IncrementalFontSpecification::GetStyle_Bold_Valid () const
    {
        return (fStyleValid_Bold);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Bold ()
    {
        fStyleValid_Bold = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetStyle_Bold (bool isBold)
    {
        fStyleValid_Bold = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Bold (isBold);
    }
    inline bool IncrementalFontSpecification::GetStyle_Italic () const
    {
        Require (fStyleValid_Italic);
        return inherited::GetStyle_Italic ();
    }
    inline bool IncrementalFontSpecification::GetStyle_Italic_Valid () const
    {
        return (fStyleValid_Italic);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Italic ()
    {
        fStyleValid_Italic = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetStyle_Italic (bool isItalic)
    {
        fStyleValid_Italic = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Italic (isItalic);
    }
    inline bool IncrementalFontSpecification::GetStyle_Underline () const
    {
        Require (fStyleValid_Underline);
        return inherited::GetStyle_Underline ();
    }
    inline bool IncrementalFontSpecification::GetStyle_Underline_Valid () const
    {
        return (fStyleValid_Underline);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Underline ()
    {
        fStyleValid_Underline = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetStyle_Underline (bool isUnderline)
    {
        fStyleValid_Underline = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Underline (isUnderline);
    }
    inline FontSpecification::SubOrSuperScript IncrementalFontSpecification::GetStyle_SubOrSuperScript () const
    {
        Require (fStyleValid_SubOrSuperScript);
        return inherited::GetStyle_SubOrSuperScript ();
    }
    inline bool IncrementalFontSpecification::GetStyle_SubOrSuperScript_Valid () const
    {
        return (fStyleValid_SubOrSuperScript);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_SubOrSuperScript ()
    {
        fStyleValid_SubOrSuperScript = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
    {
        fStyleValid_SubOrSuperScript = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_SubOrSuperScript (subOrSuperScript);
    }
#if qPlatform_MacOS
    inline bool IncrementalFontSpecification::GetStyle_Outline () const
    {
        Require (fStyleValid_Outline);
        return (inherited::GetStyle_Outline ());
    }
    inline bool IncrementalFontSpecification::GetStyle_Outline_Valid () const
    {
        return (fStyleValid_Outline);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Outline ()
    {
        fStyleValid_Outline = false;
    }
    inline void IncrementalFontSpecification::SetStyle_Outline (bool isOutline)
    {
        fStyleValid_Outline = true;
        inherited::SetStyle_Outline (isOutline);
    }
    inline bool IncrementalFontSpecification::GetStyle_Shadow () const
    {
        Require (fStyleValid_Shadow);
        return (inherited::GetStyle_Shadow ());
    }
    inline bool IncrementalFontSpecification::GetStyle_Shadow_Valid () const
    {
        return (fStyleValid_Shadow);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Shadow ()
    {
        fStyleValid_Shadow = false;
    }
    inline void IncrementalFontSpecification::SetStyle_Shadow (bool isShadow)
    {
        fStyleValid_Shadow = true;
        inherited::SetStyle_Shadow (isShadow);
    }
    inline bool IncrementalFontSpecification::GetStyle_Condensed () const
    {
        Require (fStyleValid_Condensed);
        return (inherited::GetStyle_Condensed ());
    }
    inline bool IncrementalFontSpecification::GetStyle_Condensed_Valid () const
    {
        return (fStyleValid_Condensed);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Condensed ()
    {
        fStyleValid_Condensed = false;
    }
    inline void IncrementalFontSpecification::SetStyle_Condensed (bool isCondensed)
    {
        fStyleValid_Condensed = true;
        inherited::SetStyle_Condensed (isCondensed);
    }
    inline bool IncrementalFontSpecification::GetStyle_Extended () const
    {
        Require (fStyleValid_Extended);
        return (inherited::GetStyle_Extended ());
    }
    inline bool IncrementalFontSpecification::GetStyle_Extended_Valid () const
    {
        return (fStyleValid_Extended);
    }
    inline void IncrementalFontSpecification::InvalidateStyle_Extended ()
    {
        fStyleValid_Extended = false;
    }
    inline void IncrementalFontSpecification::SetStyle_Extended (bool isExtended)
    {
        fStyleValid_Extended = true;
        inherited::SetStyle_Extended (isExtended);
    }
#elif qPlatform_Windows
        inline bool IncrementalFontSpecification::GetStyle_Strikeout () const
        {
            Require (fStyleValid_Strikeout);
            return (inherited::GetStyle_Strikeout ());
        }
        inline bool IncrementalFontSpecification::GetStyle_Strikeout_Valid () const
        {
            return (fStyleValid_Strikeout);
        }
        inline void IncrementalFontSpecification::InvalidateStyle_Strikeout ()
        {
            fStyleValid_Strikeout = false;
#if qPlatform_Windows
            fDidSetOSRepCallFlag = false;
#endif
        }
        inline void IncrementalFontSpecification::SetStyle_Strikeout (bool isStrikeout)
        {
            fStyleValid_Strikeout = true;
#if qPlatform_Windows
            fDidSetOSRepCallFlag = false;
#endif
            inherited::SetStyle_Strikeout (isStrikeout);
        }
#endif
    // FontSize info
    inline unsigned short IncrementalFontSpecification::GetPointSize () const
    {
        Require (fFontSizeValid);
        Require (not fFontSizeIncrementValid);
        return inherited::GetPointSize ();
    }
    inline bool IncrementalFontSpecification::GetPointSize_Valid () const
    {
        return (fFontSizeValid);
    }
    inline void IncrementalFontSpecification::InvalidatePointSize ()
    {
        fFontSizeValid          = false;
        fFontSizeIncrementValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetPointSize (FontSize pointSize)
    {
        fFontSizeValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetPointSize (pointSize);
        fFontSizeIncrementValid = false;
    }
#if qPlatform_Windows
    inline void IncrementalFontSpecification::PokeAtTMHeight (long tmHeight)
    {
        fFontSizeValid          = true;
        fDidSetOSRepCallFlag    = false;
        fFontSizeIncrementValid = false;
        inherited::PokeAtTMHeight (tmHeight);
    }
#endif
    inline short IncrementalFontSpecification::GetPointSizeIncrement () const
    {
        Require (not fFontSizeValid);
        Require (fFontSizeIncrementValid);
        return (short)inherited::GetPointSize ();
    }
    inline bool IncrementalFontSpecification::GetPointSizeIncrement_Valid () const
    {
        return (fFontSizeIncrementValid);
    }
    inline void IncrementalFontSpecification::InvalidatePointSizeIncrement ()
    {
        fFontSizeValid          = false;
        fFontSizeIncrementValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void IncrementalFontSpecification::SetPointSizeIncrement (short pointSizeIncrement)
    {
        fFontSizeValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetPointSize ((unsigned short)pointSizeIncrement);
        fFontSizeIncrementValid = true;
    }
    // FontFaceColor info
    inline Color IncrementalFontSpecification::GetTextColor () const
    {
        Require (fTextColorValid);
        return inherited::GetTextColor ();
    }
    inline bool IncrementalFontSpecification::GetTextColor_Valid () const
    {
        return (fTextColorValid);
    }
    inline void IncrementalFontSpecification::InvalidateTextColor ()
    {
        fTextColorValid = false;
    }
    inline void IncrementalFontSpecification::SetTextColor (const Color& textColor)
    {
        fTextColorValid = true;
        inherited::SetTextColor (textColor);
    }
#if qPlatform_MacOS
    inline void IncrementalFontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
    {
        Require (fFontSpecifierValid and
                 fStyleValid_Bold and
                 fStyleValid_Italic and
                 fStyleValid_Underline and
                 fFontSizeValid);
        Require (fStyleValid_Outline and
                 fStyleValid_Shadow and
                 fStyleValid_Condensed and
                 fStyleValid_Extended);
        inherited::GetOSRep (fontID, fontSize, fontStyle);
    }
    inline void IncrementalFontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
    {
        fFontSpecifierValid     = true;
        fStyleValid_Bold        = true;
        fStyleValid_Italic      = true;
        fStyleValid_Underline   = true;
        fStyleValid_Outline     = true;
        fStyleValid_Shadow      = true;
        fStyleValid_Condensed   = true;
        fStyleValid_Extended    = true;
        fFontSizeValid          = true;
        fFontSizeIncrementValid = false;
        inherited::SetOSRep (fontID, fontSize, fontStyle);
    }
#elif qPlatform_Windows
        inline LOGFONT IncrementalFontSpecification::GetOSRep () const
        {
            Require (fFontSpecifierValid and
                     fStyleValid_Bold and
                     fStyleValid_Italic and
                     fStyleValid_Underline and
                     fFontSizeValid);
            Require (fStyleValid_Strikeout);
            return inherited::GetOSRep ();
        }
        inline void IncrementalFontSpecification::GetOSRep (LOGFONT* logFont) const
        {
            RequireNotNull (logFont);
            Require (fFontSpecifierValid and
                     fStyleValid_Bold and
                     fStyleValid_Italic and
                     fStyleValid_Underline and
                     fFontSizeValid);
            Require (fStyleValid_Strikeout);
            inherited::GetOSRep (logFont);
        }
        inline void IncrementalFontSpecification::SetOSRep (LOGFONT logFont)
        {
            fFontSpecifierValid     = true;
            fStyleValid_Bold        = true;
            fStyleValid_Italic      = true;
            fStyleValid_Underline   = true;
            fStyleValid_Strikeout   = true;
            fFontSizeValid          = true;
            fFontSizeIncrementValid = false;
            fDidSetOSRepCallFlag    = true;
            inherited::SetOSRep (logFont);
        }
        inline void IncrementalFontSpecification::LightSetOSRep (LOGFONT logFont)
        {
            fFontSpecifierValid     = true;
            fStyleValid_Bold        = true;
            fStyleValid_Italic      = true;
            fStyleValid_Underline   = true;
            fStyleValid_Strikeout   = true;
            fFontSizeValid          = true;
            fFontSizeIncrementValid = false;
            fDidSetOSRepCallFlag    = true;
            inherited::LightSetOSRep (logFont);
        }
        inline bool IncrementalFontSpecification::GetDidSetOSRepCallFlag () const
        {
            return fDidSetOSRepCallFlag;
        }
#endif
    inline void IncrementalFontSpecification::MergeIn (const IncrementalFontSpecification& addInTheseAttributes)
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
#if qPlatform_MacOS
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
#elif qPlatform_Windows
            if (addInTheseAttributes.GetStyle_Strikeout_Valid ()) {
                SetStyle_Strikeout (addInTheseAttributes.GetStyle_Strikeout ());
            }
#endif

        // Font Size
        if (addInTheseAttributes.GetPointSize_Valid ()) {
#if qPlatform_Windows
            // speed tweek - avoid costly conversion to 'points'. All we want todo is copy the tmHeight field!
            PokeAtTMHeight (addInTheseAttributes.PeekAtTMHeight ());
#else
                SetPointSize (addInTheseAttributes.GetPointSize ());
#endif
        }
        if (addInTheseAttributes.GetPointSizeIncrement_Valid ()) {
            short pointSize = GetPointSize ();
            pointSize += addInTheseAttributes.GetPointSizeIncrement ();
            if (pointSize <= 0) { // never let point size get any smaller...
                pointSize = 1;
            }
            SetPointSize (pointSize);
        }

        // Text Color
        if (addInTheseAttributes.GetTextColor_Valid ()) {
            SetTextColor (addInTheseAttributes.GetTextColor ());
        }

#if qPlatform_Windows
        fDidSetOSRepCallFlag = addInTheseAttributes.GetDidSetOSRepCallFlag ();
#endif
    }
    inline bool IncrementalFontSpecification::operator== (const IncrementalFontSpecification& rhs) const
    {
        // Either make this non-portable, or somehow do some hack to make this test FASTER than it looks like
        // it may be currently - profile??? - LGP 960517
        //
        // FontName Info
        {
            if (GetFontNameSpecifier_Valid () != rhs.GetFontNameSpecifier_Valid ()) {
                return false;
            }
            if (GetFontNameSpecifier_Valid () and (GetFontNameSpecifier () != rhs.GetFontNameSpecifier ())) {
                return false;
            }
        }

        // Style Info
        {
            if (GetStyle_Bold_Valid () != rhs.GetStyle_Bold_Valid ()) {
                return false;
            }
            if (GetStyle_Bold_Valid () and (GetStyle_Bold () != rhs.GetStyle_Bold ())) {
                return false;
            }
        }
        {
            if (GetStyle_Italic_Valid () != rhs.GetStyle_Italic_Valid ()) {
                return false;
            }
            if (GetStyle_Italic_Valid () and (GetStyle_Italic () != rhs.GetStyle_Italic ())) {
                return false;
            }
        }
        {
            if (GetStyle_Underline_Valid () != rhs.GetStyle_Underline_Valid ()) {
                return false;
            }
            if (GetStyle_Underline_Valid () and (GetStyle_Underline () != rhs.GetStyle_Underline ())) {
                return false;
            }
        }
        {
            if (GetStyle_SubOrSuperScript_Valid () != rhs.GetStyle_SubOrSuperScript_Valid ()) {
                return false;
            }
            if (GetStyle_SubOrSuperScript_Valid () and (GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ())) {
                return false;
            }
        }
#if qPlatform_MacOS
        {
            if (GetStyle_Outline_Valid () != rhs.GetStyle_Outline_Valid ()) {
                return false;
            }
            if (GetStyle_Outline_Valid () and (GetStyle_Outline () != rhs.GetStyle_Outline ())) {
                return false;
            }
        }
        {
            if (GetStyle_Shadow_Valid () != rhs.GetStyle_Shadow_Valid ()) {
                return false;
            }
            if (GetStyle_Shadow_Valid () and (GetStyle_Shadow () != rhs.GetStyle_Shadow ())) {
                return false;
            }
        }
        {
            if (GetStyle_Condensed_Valid () != rhs.GetStyle_Condensed_Valid ()) {
                return false;
            }
            if (GetStyle_Condensed_Valid () and (GetStyle_Condensed () != rhs.GetStyle_Condensed ())) {
                return false;
            }
        }
        {
            if (GetStyle_Extended_Valid () != rhs.GetStyle_Extended_Valid ()) {
                return false;
            }
            if (GetStyle_Extended_Valid () and (GetStyle_Extended () != rhs.GetStyle_Extended ())) {
                return false;
            }
        }
#elif qPlatform_Windows
            {
                if (GetStyle_Strikeout_Valid () != rhs.GetStyle_Strikeout_Valid ()) {
                    return false;
                }
                if (GetStyle_Strikeout_Valid () and (GetStyle_Strikeout () != rhs.GetStyle_Strikeout ())) {
                    return false;
                }
            }
#endif

        // Font Color Info
        {
            if (GetTextColor_Valid () != rhs.GetTextColor_Valid ()) {
                return false;
            }
            if (GetTextColor_Valid () and (GetTextColor () != rhs.GetTextColor ())) {
                return false;
            }
        }

        // Size Info
        {
            if (GetPointSizeIncrement_Valid () != rhs.GetPointSizeIncrement_Valid ()) {
                return false;
            }
            if (GetPointSizeIncrement_Valid () and (GetPointSizeIncrement () != rhs.GetPointSizeIncrement ())) {
                return false;
            }
        }
        {
            if (GetPointSize_Valid () != rhs.GetPointSize_Valid ()) {
                return false;
            }
            if (GetPointSize_Valid ()) {
#if qPlatform_Windows
                // Speed tweek to avoid divide and getdevicecaps crap...
                if (PeekAtTMHeight () == rhs.PeekAtTMHeight ()) {
                    return true;
                }
                else if ((PeekAtTMHeight () > 0) == (rhs.PeekAtTMHeight () > 0)) {
                    return false; // if same sign, we can just compare for equality, and since they
                    // ABOVE didn't compare equal, they must be different point sizes
                    // (or at least very close depending a little on resoution...)
                    // If their signs DIFFER, we must fall through into the scaling crap (GetPointSize).
                }
#endif
                if (GetPointSize () != rhs.GetPointSize ()) {
                    return false;
                }
            }
        }

        return true;
    }

    //  class   InstalledFonts
    inline const vector<Led_SDK_String>& InstalledFonts::GetUsableFontNames () const
    {
        return fFontNames;
    }

    /*
     ********************************************************************************
     ******************************** Led_GetTextColor ******************************
     ********************************************************************************
     */
    inline Color Led_GetTextColor ()
    {
#if qPlatform_MacOS
        return (Color::kBlack);
#elif qPlatform_Windows
            return Color (::GetSysColor (COLOR_WINDOWTEXT));
#elif qStroika_FeatureSupported_XWindows
            return (Color::kBlack);
#endif
    }

    /*
     ********************************************************************************
     ************************** Led_GetTextBackgroundColor **************************
     ********************************************************************************
     */
    inline Color Led_GetTextBackgroundColor ()
    {
#if qPlatform_MacOS
        return (Color::kWhite);
#elif qPlatform_Windows
            return Color (::GetSysColor (COLOR_WINDOW));
#elif qStroika_FeatureSupported_XWindows
            return (Color::kWhite);
#endif
    }

    /*
     ********************************************************************************
     **************************** Led_GetSelectedTextColor **************************
     ********************************************************************************
     */
    inline Color Led_GetSelectedTextColor ()
    {
#if qPlatform_MacOS
        RGBColor hiliteRGBValue;
        LMGetHiliteRGB (&hiliteRGBValue);
        /*
            *  This is based on empirical testing with the behavior of the TE in the Color desk accessory
            *  that comes with system 7.5. I REALLY should probably do something with color intensity
            *  matching -- LGP 950531
            */
        if (Color (hiliteRGBValue) == Color::kBlack) {
            return (Color::kWhite);
        }
        else {
            return (Color::kBlack);
        }
#elif qPlatform_Windows
            //          return Color (::GetSysColor (COLOR_CAPTIONTEXT));
            return Color (::GetSysColor (COLOR_HIGHLIGHTTEXT));
#elif qStroika_FeatureSupported_XWindows
            return (Color::kWhite);
#endif
    }
    inline Color Led_GetSelectedTextBackgroundColor ()
    {
#if qPlatform_MacOS
        RGBColor hiliteRGBValue;
        LMGetHiliteRGB (&hiliteRGBValue);
        return Color (hiliteRGBValue);
#elif qPlatform_Windows
            //return Color (::GetSysColor (COLOR_ACTIVECAPTION));
            return Color (::GetSysColor (COLOR_HIGHLIGHT));
#elif qStroika_FeatureSupported_XWindows
            return (Color::kBlack);
#endif
    }

    /*
     ********************************************************************************
     ************************ Tablet::ClipNarrowAndRestore *********************
     ********************************************************************************
     */
    inline Tablet::ClipNarrowAndRestore::ClipNarrowAndRestore (Tablet* tablet)
        : fTablet (tablet)
        , fHasOldClip (false)
        , fOldClip ()
    {
        RequireNotNull (tablet);
#if qPlatform_Windows
        if (::GetDeviceCaps (fTablet->m_hDC, TECHNOLOGY) == DT_METAFILE) {
            return;
        }
#endif
        fHasOldClip = tablet->GetClip (&fOldClip);
    }
    inline Tablet::ClipNarrowAndRestore::ClipNarrowAndRestore (Tablet* tablet, const Led_Rect& clipFurtherTo)
        : fTablet (tablet)
        , fHasOldClip (false)
        , fOldClip ()
    {
        RequireNotNull (tablet);
        fHasOldClip = tablet->GetClip (&fOldClip);
#if qPlatform_MacOS
        Assert (fHasOldClip);
        tablet->SetClip (fOldClip * clipFurtherTo);
#elif qPlatform_Windows
            /*
            *  NB: We must use IntersectClipRect instead of the above SetClip () call because the CLIP on Win32 is in
            *  device rather than logical coordinates.
            */
            if (::GetDeviceCaps (tablet->m_hDC, TECHNOLOGY) == DT_METAFILE) {
                return;
            }
            Verify (::IntersectClipRect (*tablet, clipFurtherTo.GetLeft (), clipFurtherTo.GetTop (), clipFurtherTo.GetRight (), clipFurtherTo.GetBottom ()) != ERROR);
#endif
    }
    inline Tablet::ClipNarrowAndRestore::ClipNarrowAndRestore (Tablet* tablet, [[maybe_unused]] const Region& clipFurtherTo)
        : fTablet (tablet)
        , fHasOldClip (false)
        , fOldClip ()
    {
        RequireNotNull (tablet);
        fHasOldClip = tablet->GetClip (&fOldClip);
#if qPlatform_MacOS
        Assert (fHasOldClip);
        tablet->SetClip (fOldClip * clipFurtherTo);
#elif qPlatform_Windows
            Assert (false); // NYI - see SPR#????
#else
            Assert (false); // NYI
#endif
    }
    inline Tablet::ClipNarrowAndRestore::~ClipNarrowAndRestore ()
    {
        AssertNotNull (fTablet);
#if qPlatform_Windows
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

#if qPlatform_Windows
    /*
     ********************************************************************************
     ********************************** WindowDC ************************************
     ********************************************************************************
     */
    inline WindowDC::WindowDC (HWND hWnd)
        : fHWnd_{hWnd}
    {
        Require (fHWnd_ == nullptr or ::IsWindow (fHWnd_));
        if (!Attach (::GetWindowDC (fHWnd_))) {
            Foundation::Execution::Throw (bad_alloc{});
        }
    }
    inline WindowDC::~WindowDC ()
    {
        AssertNotNull (m_hDC);
        ::ReleaseDC (fHWnd_, Detach ());
    }

#endif

    /*
     ********************************************************************************
     ************************* MacPortAndClipRegionEtcSaver *********************
     ********************************************************************************
     */
#if qPlatform_MacOS
    inline MacPortAndClipRegionEtcSaver::MacPortAndClipRegionEtcSaver ()
        : fSavedPort (Led_GetCurrentGDIPort ())
        ,
#if !TARGET_CARBON
        fOldLeft (Led_GetCurrentGDIPort ()->portRect.left)
        , fOldTop (Led_GetCurrentGDIPort ()->portRect.top)
        ,
#endif
        //fRGBFgColor (Led_GetCurrentGDIPort ()->rgbFgColor),
        //fRGBBkColor (Led_GetCurrentGDIPort ()->rgbBkColor)
        fOldClip (nullptr)
    {
#if TARGET_CARBON
        Rect portRect;
        ::GetPortBounds (Led_GetCurrentGDIPort (), &portRect);
        fOldLeft = portRect.left;
        fOldTop  = portRect.top;
#endif
        fOldClip = ::NewRgn ();
        if (fOldClip != nullptr) {
            ::GetClip (fOldClip);
        }
        ::GetForeColor (&fRGBFgColor);
        ::GetBackColor (&fRGBBkColor);
    }
    inline MacPortAndClipRegionEtcSaver::~MacPortAndClipRegionEtcSaver ()
    {
        ::SetPort (fSavedPort);
        ::SetOrigin (fOldLeft, fOldTop);
        ::RGBForeColor (&fRGBFgColor);
        ::RGBBackColor (&fRGBBkColor);
        if (fOldClip != nullptr) {
            ::SetClip (fOldClip);
            ::DisposeRgn (fOldClip);
        }
    }
#endif

    /*
     ********************************************************************************
     ******************************* GDI_Obj_Selector ***************************
     ********************************************************************************
     */
#if qPlatform_Windows
    inline GDI_Obj_Selector::GDI_Obj_Selector (Tablet* tablet, HGDIOBJ objToSelect)
        : fTablet (tablet)
        , fRestoreObject (nullptr)
        , fRestoreAttribObject (nullptr)
    {
        AssertNotNull (tablet);
        AssertNotNull (objToSelect);
        // See CDC::SelectObject for the logic..., but we do better than thiers and restore
        // right object to right DC!!!! - LGP 950525
        if (tablet->m_hDC != tablet->m_hAttribDC) {
            fRestoreObject = ::SelectObject (tablet->m_hDC, objToSelect);
        }
        if (tablet->m_hAttribDC != nullptr) {
            fRestoreAttribObject = ::SelectObject (tablet->m_hAttribDC, objToSelect);
        }
    }
#elif qPlatform_MacOS
        inline GDI_Obj_Selector::GDI_Obj_Selector (Tablet* tablet, const Pen& pen)
            : fTablet (tablet)
            ,
#if TARGET_CARBON
            fRestorePen (Pen (::GetPortPenMode (Led_GetCurrentGDIPort ()), &Pen::kBlackPattern, Color (GDI_GetForeColor ())))
#else
            fRestorePen (Pen (Led_GetCurrentGDIPort ()->pnMode, &Led_GetCurrentGDIPort ()->pnPat, Color (GDI_GetForeColor ())))
#endif
        {
#if qDebug
            Assert (Led_GetCurrentGDIPort () == *tablet);
#endif
            GDI_RGBForeColor (pen.fPenColor.GetOSRep ());
            ::PenMode (pen.fPenStyle);
            ::PenPat (&pen.fPenPat);
        }
#elif qStroika_FeatureSupported_XWindows
        inline GDI_Obj_Selector::GDI_Obj_Selector (Tablet* tablet, const Pen& pen)
        {
        }
#endif
    inline GDI_Obj_Selector::~GDI_Obj_Selector ()
    {
#if qPlatform_Windows
        //NB: These restore objects CAN be nullptr, if no font (or whatever) selected into DC before we do... (aside from error cases)
        if (fRestoreObject != nullptr) {
            Verify (::SelectObject (fTablet->m_hDC, fRestoreObject));
        }
        if (fRestoreAttribObject != nullptr) {
            Verify (::SelectObject (fTablet->m_hAttribDC, fRestoreAttribObject));
        }
#elif qPlatform_MacOS
            GDI_RGBForeColor (fRestorePen.fPenColor.GetOSRep ());
            ::PenMode (fRestorePen.fPenStyle);
            ::PenPat (&fRestorePen.fPenPat);
#endif
    }

#if qPlatform_MacOS
    inline void GDI_RGBForeColor (const RGBColor& color)
    {
        ::RGBForeColor (&color);
    }
    inline void GDI_RGBBackColor (const RGBColor& color)
    {
        ::RGBBackColor (&color);
    }
    inline RGBColor GDI_GetForeColor ()
    {
        RGBColor color;
        ::GetForeColor (&color);
        return (color);
    }
    inline RGBColor GDI_GetBackColor ()
    {
        RGBColor color;
        ::GetBackColor (&color);
        return (color);
    }
#endif

    inline short Led_GetMacPictTop (const Led_Picture* picture)
    {
        RequireNotNull (picture);
#if qPlatform_MacOS
        return picture->picFrame.top;
#elif qPlatform_Windows
            return Led_ByteSwapFromMac (picture->picFrameTop);
#endif
    }
    inline short Led_GetMacPictLeft (const Led_Picture* picture)
    {
        RequireNotNull (picture);
#if qPlatform_MacOS
        return picture->picFrame.left;
#elif qPlatform_Windows
            return Led_ByteSwapFromMac (picture->picFrameLeft);
#endif
    }
    inline short Led_GetMacPictBottom (const Led_Picture* picture)
    {
        RequireNotNull (picture);
#if qPlatform_MacOS
        return picture->picFrame.bottom;
#elif qPlatform_Windows
            return Led_ByteSwapFromMac (picture->picFrameBottom);
#endif
    }
    inline short Led_GetMacPictRight (const Led_Picture* picture)
    {
        RequireNotNull (picture);
#if qPlatform_MacOS
        return picture->picFrame.right;
#elif qPlatform_Windows
            return Led_ByteSwapFromMac (picture->picFrameRight);
#endif
    }
    inline short Led_GetMacPictWidth (const Led_Picture* picture)
    {
        return (Led_GetMacPictRight (picture) - Led_GetMacPictLeft (picture));
    }
    inline short Led_GetMacPictHeight (const Led_Picture* picture)
    {
        return (Led_GetMacPictBottom (picture) - Led_GetMacPictTop (picture));
    }
    inline Led_Size Led_GetMacPictSize (const Led_Picture* picture)
    {
        return Led_Size (Led_GetMacPictHeight (picture), Led_GetMacPictWidth (picture));
    }
#if qPlatform_MacOS
    inline short Led_GetMacPictTop (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictTop (*picture);
    }
    inline short Led_GetMacPictLeft (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictLeft (*picture);
    }
    inline short Led_GetMacPictBottom (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictBottom (*picture);
    }
    inline short Led_GetMacPictRight (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictRight (*picture);
    }
    inline short Led_GetMacPictWidth (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictWidth (*picture);
    }
    inline short Led_GetMacPictHeight (const Led_Picture* const* picture)
    {
        RequireNotNull (picture);
        return Led_GetMacPictHeight (*picture);
    }
    inline Led_Size Led_GetMacPictSize (const Led_Picture* const* picture)
    {
        return Led_Size (Led_GetMacPictHeight (picture), Led_GetMacPictWidth (picture));
    }
#endif

#if qProvideIMESupport
    /*
     ********************************************************************************
     *********************************** IME ************************************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (6011)
    inline IME& IME::Get ()
    {
        if (sThe == nullptr) {
            new IME ();
        }
        AssertNotNull (sThe);
        return *sThe;
    }
    DISABLE_COMPILER_MSC_WARNING_END (6011)
    inline void IME::Enable ()
    {
        if (fIMEEnableProc != nullptr) {
            fIMEEnableProc (nullptr, true);
        }
    }
    inline void IME::Disable ()
    {
        if (fIMEEnableProc != nullptr) {
            fIMEEnableProc (nullptr, false);
        }
    }
    inline bool IME::Available () const
    {
        return fWinNlsAvailable;
    }
    inline void IME::ForgetPosition ()
    {
        fLastX = -1;
        fLastY = -1;
    }
#endif

}

#endif /*_Stroika_Frameworks_Led_GDI_inl_*/
