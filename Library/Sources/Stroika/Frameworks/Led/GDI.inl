/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_GDI_inl_
#define _Stroika_Frameworks_Led_GDI_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    //struct    Point_Base
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
    inline bool operator== (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return (lhs.v == rhs.v and lhs.h == rhs.h);
    }
    template <typename COORD_TYPE>
    inline bool operator!= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return (lhs.v != rhs.v or lhs.h != rhs.h);
    }
    template <typename COORD_TYPE>
    inline bool operator< (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return ((lhs.v <= rhs.v) and (lhs.h <= rhs.h) and (lhs != rhs));
    }
    template <typename COORD_TYPE>
    inline bool operator<= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return ((lhs.v <= rhs.v) and (lhs.h <= rhs.h));
    }
    template <typename COORD_TYPE>
    inline bool operator> (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return ((lhs.v >= rhs.v) and (lhs.h >= rhs.h) and lhs != rhs);
    }
    template <typename COORD_TYPE>
    inline bool operator>= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        return ((lhs.v >= rhs.v) and (lhs.h >= rhs.h));
    }
    template <typename COORD_TYPE>
    inline Point_Base<COORD_TYPE> operator+ (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs)
    {
        Led_Point result = lhs;
        result.v += rhs.v;
        result.h += rhs.h;
        return result;
    }

    //struct    Led_Point
    inline Led_Point operator- (const Led_Point& lhs, const Led_Point& rhs)
    {
        return (Led_Point (lhs.v - rhs.v, lhs.h - rhs.h));
    }

    //struct    Rect_Base<POINT_TYPE,SIZE_TYPE>
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
    inline bool operator== (const Rect_Base<POINT_TYPE, SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE, SIZE_TYPE>& rhs)
    {
        return ((lhs.GetLeft () == rhs.GetLeft ()) and (lhs.GetRight () == rhs.GetRight ()) and (lhs.GetTop () == rhs.GetTop ()) and (lhs.GetBottom () == rhs.GetBottom ()));
    }
    template <typename POINT_TYPE, typename SIZE_TYPE>
    inline bool operator!= (const Rect_Base<POINT_TYPE, SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE, SIZE_TYPE>& rhs)
    {
        return ((lhs.GetLeft () != rhs.GetLeft ()) or (lhs.GetRight () != rhs.GetRight ()) or (lhs.GetTop () != rhs.GetTop ()) or (lhs.GetBottom () != rhs.GetBottom ()));
    }

    //class Led_TWIPS
    inline Led_TWIPS::Led_TWIPS (long v)
        : fValue (v)
    {
    }
    inline Led_TWIPS::operator long () const
    {
        return fValue;
    }
    inline Led_TWIPS& Led_TWIPS::operator+= (const Led_TWIPS& rhs)
    {
        fValue += rhs.fValue;
        return *this;
    }
    inline Led_TWIPS& Led_TWIPS::operator-= (const Led_TWIPS& rhs)
    {
        fValue -= rhs.fValue;
        return *this;
    }
    inline Led_TWIPS& Led_TWIPS::operator*= (double rhs)
    {
        fValue = static_cast<long> (fValue * rhs);
        return *this;
    }
    inline Led_TWIPS operator+ (const Led_TWIPS& lhs, const Led_TWIPS& rhs)
    {
        return Led_TWIPS (static_cast<long> (lhs) + static_cast<long> (rhs));
    }
    inline Led_TWIPS operator- (const Led_TWIPS& lhs, const Led_TWIPS& rhs)
    {
        return Led_TWIPS (static_cast<long> (lhs) - static_cast<long> (rhs));
    }

#if qPlatform_Windows
    //class auto_gdi_ptr
    inline auto_gdi_ptr::auto_gdi_ptr (HGDIOBJ gdiObj)
        : fGDIObj (gdiObj)
    {
    }
    inline auto_gdi_ptr::~auto_gdi_ptr ()
    {
        Verify (::DeleteObject (fGDIObj));
    }
#endif

    //class Led_Region
    inline Led_Region::Led_Region ()
#if qPlatform_MacOS
        : fRgn (::NewRgn ())
        , fOwned (true)
#elif qPlatform_Windows
        : fRgn (::CreateRectRgn (0, 0, 0, 0))
#endif
    {
#if qPlatform_MacOS || qPlatform_Windows
        Led_ThrowIfNull (fRgn);
#endif
    }
    inline Led_Region::Led_Region (const Led_Rect& r)
#if qPlatform_MacOS
        : fRgn (::NewRgn ())
        , fOwned (true)
#elif qPlatform_Windows
        : fRgn (::CreateRectRgn (r.GetLeft (), r.GetTop (), r.GetRight (), r.GetBottom ()))
#endif
    {
        Require (r.GetHeight () >= 0);
        Require (r.GetWidth () >= 0);
#if qPlatform_MacOS || qPlatform_Windows
        Led_ThrowIfNull (fRgn);
#endif
#if qPlatform_MacOS
        ::SetRectRgn (fRgn, (short)r.left, (short)r.top, (short)r.right, (short)r.bottom);
#endif
        Assert (GetBoundingRect () == r or (GetBoundingRect ().IsEmpty () and r.IsEmpty ()));
    }
    inline Led_Region::Led_Region (const Led_Region& from)
#if qPlatform_MacOS
        : fRgn (::NewRgn ())
        , fOwned (true)
#elif qPlatform_Windows
        : fRgn (::CreateRectRgn (0, 0, 0, 0))
#endif
    {
#if qPlatform_MacOS || qPlatform_Windows
        Led_ThrowIfNull (fRgn);
#endif
#if qPlatform_MacOS
        ::CopyRgn (from.GetOSRep (), fRgn);
#elif qPlatform_Windows
        Verify (::CombineRgn (fRgn, from, from, RGN_COPY) != ERROR);
#endif
    }
    inline const Led_Region& Led_Region::operator= (const Led_Region& rhs)
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
        Led_ThrowIfNull (fRgn);
#endif
        return *this;
    }
    inline Led_Region::~Led_Region ()
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
    inline bool Led_Region::IsEmpty () const
    {
#if qPlatform_MacOS || qPlatform_Windows
        AssertNotNull (fRgn);
#endif
        Assert (false); //NYI - not used yet - so don't worry about this right now... LGP 2002-12-03
        return false;
    }
    inline Led_Rect Led_Region::GetBoundingRect () const
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
    inline Led_Region operator* (const Led_Region& lhs, const Led_Region& rhs)
    {
        Led_Region result;
#if qPlatform_MacOS
        ::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
#elif qPlatform_Windows
        Verify (::CombineRgn (result, lhs, rhs, RGN_AND) != ERROR);
#endif
        return result;
    }
    inline Led_Region operator+ (const Led_Region& lhs, const Led_Region& rhs)
    {
        Led_Region result;
#if qPlatform_MacOS
        ::UnionRgn (lhs.GetOSRep (), rhs.GetOSRep (), result.GetOSRep ());
#elif qPlatform_Windows
        Verify (::CombineRgn (result, lhs, rhs, RGN_OR) != ERROR);
#endif
        return result;
    }

    // class Led_TabStopList::Led_TabStopList
    inline Led_TabStopList::Led_TabStopList ()
    {
    }
    inline Led_TabStopList::~Led_TabStopList ()
    {
    }
    inline Led_Distance Led_TabStopList::ComputeTabStopAfterPosition (Led_Tablet tablet, Led_Distance afterPos) const
    {
        RequireNotNull (tablet);
        return tablet->CvtFromTWIPSH (ComputeTabStopAfterPosition (tablet->CvtToTWIPSH (afterPos)));
    }

#if qPlatform_Windows
    // class Led_Bitmap
    inline Led_Bitmap::Led_Bitmap ()
        : m_hObject (nullptr)
        , fImageSize (Led_Size (0, 0))
    {
    }
    inline Led_Bitmap::~Led_Bitmap ()
    {
        if (m_hObject != nullptr) {
            ::DeleteObject (m_hObject);
        }
    }
    inline void Led_Bitmap::DeleteObject ()
    {
        if (m_hObject != nullptr) {
            ::DeleteObject (m_hObject);
            m_hObject = nullptr;
        }
    }
    inline Led_Bitmap::operator HBITMAP () const
    {
        return m_hObject;
    }
    inline Led_Size Led_Bitmap::GetImageSize () const
    {
        // only valid if m_hObject != nullptr
        RequireNotNull (m_hObject);
        return fImageSize;
    }
#endif

//class Led_Tablet_
#if qPlatform_MacOS
    inline Led_Tablet_::operator GrafPtr () const
    {
        return fGrafPort;
    }
#elif qPlatform_Windows
    inline Led_Tablet_::operator HDC () const
    {
        return m_hDC;
    }
#endif
    /*
    @METHOD:        Led_Tablet_::CvtFromTWIPSV
    @DESCRIPTION:
        <p>See also @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
    */
    inline Led_Coordinate Led_Tablet_::CvtFromTWIPSV (Led_TWIPS from) const
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
        return from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsV () / 1440;
//              return from / 20;   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Led_Tablet_::CvtFromTWIPSH
    @DESCRIPTION:
        <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
    */
    inline Led_Coordinate Led_Tablet_::CvtFromTWIPSH (Led_TWIPS from) const
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
        return from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsH () / 1440;
//              return from / 20;   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Led_Tablet_::CvtToTWIPSV
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
        <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSH'.</p>
    */
    inline Led_TWIPS Led_Tablet_::CvtToTWIPSV (Led_Coordinate from) const
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
        return Led_TWIPS (::MulDiv (x.y, 1440, fLogPixelsV));
#else
        return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
//              return Led_TWIPS (from * 20);   // assume 72dpi on mac
#endif
    }
    /*
    @METHOD:        Led_Tablet_::CvtToTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
        <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV'.</p>
    */
    inline Led_TWIPS Led_Tablet_::CvtToTWIPSH (Led_Coordinate from) const
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
        return Led_TWIPS (::MulDiv (x.x, 1440, fLogPixelsH));
#else
        return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
//              return Led_TWIPS (from * 20);   // assume 72dpi on mac
#endif
    }
#if qPlatform_MacOS
    inline void Led_Tablet_::SetPort ()
    {
        ::SetPort (fGrafPort);
    }
#elif qPlatform_Windows
    inline BOOL Led_Tablet_::BitBlt (int x, int y, int nWidth, int nHeight, Led_Tablet_* pSrcDC, int xSrc, int ySrc, DWORD dwRop)
    {
        AssertNotNull (m_hDC);
        return ::BitBlt (m_hDC, x, y, nWidth, nHeight, pSrcDC->m_hDC, xSrc, ySrc, dwRop);
    }
    inline BOOL Led_Tablet_::CreateCompatibleDC (Led_Tablet_* pDC)
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
    inline COLORREF Led_Tablet_::SetTextColor (COLORREF crColor)
    {
        Assert (m_hDC != nullptr);
        COLORREF crRetVal = CLR_INVALID;

        if (m_hDC != m_hAttribDC)
            crRetVal = ::SetTextColor (m_hDC, crColor);
        if (m_hAttribDC != nullptr)
            crRetVal = ::SetTextColor (m_hAttribDC, crColor);
        return crRetVal;
    }
    inline COLORREF Led_Tablet_::SetBkColor (COLORREF crColor)
    {
        Assert (m_hDC != nullptr);
        COLORREF crRetVal = CLR_INVALID;

        if (m_hDC != m_hAttribDC)
            crRetVal = ::SetBkColor (m_hDC, crColor);
        if (m_hAttribDC != nullptr)
            crRetVal = ::SetBkColor (m_hAttribDC, crColor);
        return crRetVal;
    }
    inline HWND Led_Tablet_::GetWindow () const
    {
        Assert (m_hDC != nullptr);
        return ::WindowFromDC (m_hDC);
    }
    inline BOOL Led_Tablet_::IsPrinting () const
    {
        return m_bPrinting;
    }
    inline BOOL Led_Tablet_::RoundRect (int x1, int y1, int x2, int y2, int x3, int y3)
    {
        Assert (m_hDC != nullptr);
        return ::RoundRect (m_hDC, x1, y1, x2, y2, x3, y3);
    }
    inline BOOL Led_Tablet_::TextOut (int x, int y, LPCTSTR lpszString, int nCount)
    {
        Assert (m_hDC != nullptr);
        return ::TextOut (m_hDC, x, y, lpszString, nCount);
    }
#if 0
    inline  SIZE    Led_Tablet_::GetTextExtent (LPCTSTR lpszString, int nCount) const
    {
        Assert(m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetTextExtentPoint32 (m_hAttribDC, lpszString, nCount, &size));
        return size;
    }
#endif
    inline int Led_Tablet_::SetBkMode (int nBkMode)
    {
        Assert (m_hDC != nullptr);
        int nRetVal = 0;

        if (m_hDC != m_hAttribDC)
            nRetVal = ::SetBkMode (m_hDC, nBkMode);
        if (m_hAttribDC != nullptr)
            nRetVal = ::SetBkMode (m_hAttribDC, nBkMode);
        return nRetVal;
    }
    inline unsigned int Led_Tablet_::SetTextAlign (unsigned int nTextAlign)
    {
        Assert (m_hDC != nullptr);
        unsigned int nRetVal = 0;

        if (m_hDC != m_hAttribDC)
            nRetVal = ::SetTextAlign (m_hDC, nTextAlign);
        if (m_hAttribDC != nullptr)
            nRetVal = ::SetTextAlign (m_hAttribDC, nTextAlign);
        return nRetVal;
    }

    inline SIZE Led_Tablet_::GetWindowExt () const
    {
        Assert (m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetWindowExtEx (m_hAttribDC, &size));
        return size;
    }
    inline SIZE Led_Tablet_::GetViewportExt () const
    {
        Assert (m_hAttribDC != nullptr);
        SIZE size;
        Verify (::GetViewportExtEx (m_hAttribDC, &size));
        return size;
    }
    inline BOOL Led_Tablet_::Rectangle (int x1, int y1, int x2, int y2)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, x1, y1, x2, y2);
    }
    inline BOOL Led_Tablet_::Rectangle (const RECT& r)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, r.left, r.top, r.right, r.bottom);
    }
    inline BOOL Led_Tablet_::Rectangle (LPCRECT lpRect)
    {
        Assert (m_hDC != nullptr);
        return ::Rectangle (m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }
    inline BOOL Led_Tablet_::GetTextMetrics (LPTEXTMETRIC lpMetrics) const
    {
        Assert (m_hAttribDC != nullptr);
        return ::GetTextMetrics (m_hAttribDC, lpMetrics);
    }
    inline HBITMAP Led_Tablet_::SelectObject (HBITMAP hBitmap)
    {
        Assert (m_hDC != nullptr);
        return (HBITMAP)::SelectObject (m_hDC, hBitmap);
    }
#if defined(STRICT)
    inline HFONT Led_Tablet_::SelectObject (HFONT hFont)
    {
        Assert (m_hDC != nullptr);
        return (HFONT)::SelectObject (m_hDC, hFont);
    }
#endif
    inline POINT Led_Tablet_::SetWindowOrg (int x, int y)
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
    inline int Led_Tablet_::GetDeviceCaps (int nIndex) const
    {
        Assert (m_hAttribDC != nullptr);
        return ::GetDeviceCaps (m_hAttribDC, nIndex);
    }
    inline BOOL Led_Tablet_::Attach (HDC hDC, Led_Tablet_::OwnDCControl ownsDC)
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
    inline HDC Led_Tablet_::Detach ()
    {
        HDC hDC     = m_hDC;
        m_hAttribDC = nullptr;
        m_hDC       = nullptr;
        return hDC;
    }
#endif
    inline void Led_Tablet_::MoveTo (const Led_Point& to)
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
    inline void Led_Tablet_::LineTo (const Led_Point& to)
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
    inline Led_Region Led_Tablet_::GetClip () const
    {
        Led_Region result;
#if qPlatform_MacOS
        const_cast<Led_Tablet_*> (this)->SetPort ();
        ::GetClip (result.GetOSRep ());
#elif qPlatform_Windows
        int r = ::GetClipRgn (*this, result);
        Assert (r == 0 or r == 1 or r == -1);
        if (r == 0) {
#if !qInternalErrorWithStaticRegionDeclaredInFunction
            static
#endif
                Led_Region kWideOpened = Led_Region (Led_Rect (-10000, -10000, 20000, 20000));
            result                     = kWideOpened;
        }
#else
        Assert (false); // NYI
#endif
        return result;
    }
    inline bool Led_Tablet_::GetClip (Led_Region* r) const
    {
        RequireNotNull (r);
#if qPlatform_MacOS
        const_cast<Led_Tablet_*> (this)->SetPort ();
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
    inline void Led_Tablet_::SetClip ()
    {
#if qPlatform_MacOS
        SetPort ();
        static Led_Region kWideOpened = Led_Region (Led_Rect (-10000, -10000, 20000, 20000));
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
    inline void Led_Tablet_::SetClip (const Led_Rect& clipTo)
    {
#if qPlatform_MacOS
        SetPort ();
        ::SetClip (Led_Region (clipTo).GetOSRep ());
#elif qPlatform_Windows
        Verify (::SelectClipRgn (*this, Led_Region (clipTo)) != ERROR);
        Ensure (GetClip ().GetBoundingRect () == clipTo);
#elif qStroika_FeatureSupported_XWindows
        XRectangle xrectangle = AsXRect (clipTo);
        ::XSetClipRectangles (fDisplay, fGC, 0, 0, &xrectangle, 1, Unsorted);
#else
        Assert (false); // NYI
#endif
    }
    inline void Led_Tablet_::SetClip (const Led_Region& clipTo)
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

    //class Led_GDIGlobals
    inline Led_GDIGlobals& Led_GDIGlobals::Get ()
    {
        if (sThe == nullptr) {
            sThe = new Led_GDIGlobals ();
        }
        return *sThe;
    }
    inline Led_Distance Led_GDIGlobals::GetMainScreenLogPixelsH () const
    {
        return fLogPixelsH;
    }
    inline Led_Distance Led_GDIGlobals::GetMainScreenLogPixelsV () const
    {
        return fLogPixelsV;
    }

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
    inline bool Intersect (const Led_Rect& lhs, const Led_Region& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        static RgnHandle lhsRgn = ::NewRgn ();
        ::SetRectRgn (lhsRgn, (short)lhs.left, (short)lhs.top, (short)lhs.right, (short)lhs.bottom);
        ::SectRgn (lhsRgn, rhs.GetOSRep (), result);
        return not::EmptyRgn (result);
#elif qPlatform_Windows
        Led_Region lhsRgn = lhs;
        Led_Region result;
        return result.CombineRgn (&lhsRgn, const_cast<Led_Region*> (&rhs), RGN_AND) != NULLREGION;
#endif
    }
    inline bool Intersect (const Led_Region& lhs, const Led_Rect& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        static RgnHandle rhsRgn = ::NewRgn ();
        ::SetRectRgn (rhsRgn, (short)rhs.left, (short)rhs.top, (short)rhs.right, (short)rhs.bottom);
        ::SectRgn (lhs.GetOSRep (), rhsRgn, result);
        return not::EmptyRgn (result);
#elif qPlatform_Windows
        Led_Region rhsRgn = rhs;
        Led_Region result;
        return result.CombineRgn (const_cast<Led_Region*> (&lhs), &rhsRgn, RGN_AND) != NULLREGION;
#endif
    }
    inline bool Intersect (const Led_Region& lhs, const Led_Region& rhs)
    {
#if qPlatform_MacOS
        static RgnHandle result = ::NewRgn ();
        ::SectRgn (lhs.GetOSRep (), rhs.GetOSRep (), result);
        return not::EmptyRgn (result);
#elif qPlatform_Windows
        Led_Region result;
        return result.CombineRgn (const_cast<Led_Region*> (&lhs), const_cast<Led_Region*> (&rhs), RGN_AND) != NULLREGION;
#endif
    }

    inline Led_Rect Intersection (const Led_Rect& lhs, const Led_Rect& rhs)
    {
        Led_Rect tmp = lhs;
        return tmp *= rhs;
    }
    inline Led_Rect operator* (const Led_Rect& lhs, const Led_Rect& rhs)
    {
        Led_Rect tmp = lhs;
        return tmp *= rhs;
    }
    inline Led_Size operator+ (Led_Size lhs, Led_Size rhs)
    {
        return (Led_Size (lhs.v + rhs.v, lhs.h + rhs.h));
    }
    inline Led_Size operator* (int lhs, Led_Size rhs)
    {
        return (Led_Size (lhs * rhs.v, lhs * rhs.h));
    }
    /*
    @METHOD:        InsetRect
    @DESCRIPTION:   <p>Utility routine to convert shrink (if vBy/hBy posative), or expand (if negative) the given @'Led_Rect'.
        NB: This routine pins the minimum output rect size (in each dimention) to be zero.</p>
    */
    inline Led_Rect InsetRect (const Led_Rect& r, int vBy, int hBy)
    {
        return Led_Rect (r.GetTop () + vBy, r.GetLeft () + hBy,
                         max (0L, Led_Coordinate (r.GetHeight ()) - 2 * vBy), max (0L, Led_Coordinate (r.GetWidth ()) - 2 * hBy));
    }
    /*
    @METHOD:        EnsureRectInRect
    @DESCRIPTION:   <p>Utility routine to ensure the first rect is entirely enclosed in the second (enclosing) rectangle.
                Pin the edges so it fits.</p>
    */
    inline Led_Rect EnsureRectInRect (const Led_Rect& r, Led_Rect enlosingR)
    {
        Led_Distance   winWidth  = min (r.GetWidth (), enlosingR.GetWidth ());
        Led_Distance   winHeight = min (r.GetHeight (), enlosingR.GetHeight ());
        Led_Coordinate winLeft   = max (r.GetLeft (), enlosingR.GetLeft ());
        Led_Coordinate winTop    = max (r.GetTop (), enlosingR.GetTop ());

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
    inline Led_TWIPS Led_CvtScreenPixelsToTWIPSV (Led_Coordinate from)
    {
#if qPlatform_Windows
        return Led_TWIPS (::MulDiv (from, 1440, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ()));
#else
        return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsToTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.
        <p>See also @'Led_CvtScreenPixelsToTWIPSV'.</p>
    */
    inline Led_TWIPS Led_CvtScreenPixelsToTWIPSH (Led_Coordinate from)
    {
#if qPlatform_Windows
        return Led_TWIPS (::MulDiv (from, 1440, Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ()));
#else
        return Led_TWIPS (from * 1440 / Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsFromTWIPSV
    @DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
        <p>See also @'Led_CvtScreenPixelsFromTWIPSH'.</p>
    */
    inline Led_Coordinate Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS from)
    {
#if qPlatform_Windows
        return ::MulDiv (from, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV (), 1440);
#else
        return Led_TWIPS (from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsV () / 1440);
#endif
    }
    /*
    @METHOD:        Led_CvtScreenPixelsFromTWIPSH
    @DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).
        <p>See also @'Led_CvtScreenPixelsFromTWIPSV'.</p>
    */
    inline Led_Coordinate Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS from)
    {
#if qPlatform_Windows
        return ::MulDiv (from, Led_GDIGlobals::Get ().GetMainScreenLogPixelsH (), 1440);
#else
        return Led_TWIPS (from * Led_GDIGlobals::Get ().GetMainScreenLogPixelsH () / 1440);
#endif
    }

    //  class   Led_FontMetrics
    inline Led_FontMetrics::Led_FontMetrics ()
        : fPlatformSpecific ()
    {
        (void)::memset (&fPlatformSpecific, 0, sizeof (fPlatformSpecific));
    }
#if qPlatform_MacOS
    inline Led_FontMetrics::Led_FontMetrics (const FontInfo& from)
        : fPlatformSpecific (from)
    {
    }
#elif qPlatform_Windows
    inline Led_FontMetrics::Led_FontMetrics (const TEXTMETRIC& from)
        : fPlatformSpecific (from)
    {
    }
#elif qStroika_FeatureSupported_XWindows
    inline Led_FontMetrics::Led_FontMetrics (const Led_FontMetrics::PlatformSpecific& from)
        : fPlatformSpecific (from)
    {
    }
#endif
    inline Led_FontMetrics::Led_FontMetrics (const Led_FontMetrics& from)
        : fPlatformSpecific (from.fPlatformSpecific)
    {
    }
    inline const Led_FontMetrics& Led_FontMetrics::operator= (const Led_FontMetrics& rhs)
    {
        fPlatformSpecific = rhs.fPlatformSpecific;
        return *this;
    }
    inline Led_Distance Led_FontMetrics::GetAscent () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.ascent);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmAscent);
#elif qStroika_FeatureSupported_XWindows
        return fPlatformSpecific.fAscent;
#endif
    }
    inline Led_Distance Led_FontMetrics::GetDescent () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.descent);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmDescent);
#elif qStroika_FeatureSupported_XWindows
        return fPlatformSpecific.fDescent;
#endif
    }
    inline Led_Distance Led_FontMetrics::GetLeading () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.leading);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmExternalLeading);
#elif qStroika_FeatureSupported_XWindows
        return (fPlatformSpecific.fLeading);
#endif
    }
    inline Led_Distance Led_FontMetrics::GetHeight () const
    {
#if qPlatform_Windows
        Assert (fPlatformSpecific.tmHeight >= 0);
        Assert (GetAscent () + GetDescent () == Led_Distance (fPlatformSpecific.tmHeight));
#endif
        return (GetAscent () + GetDescent ());
    }
    inline Led_Distance Led_FontMetrics::GetLineHeight () const
    {
        return (GetAscent () + GetDescent () + GetLeading ());
    }
    inline nonvirtual Led_Distance Led_FontMetrics::GetMaxCharacterWidth () const
    {
#if qPlatform_MacOS
        return (fPlatformSpecific.widMax);
#elif qPlatform_Windows
        return (fPlatformSpecific.tmMaxCharWidth);
#elif qStroika_FeatureSupported_XWindows
        return (fPlatformSpecific.fMaxCharWidth);
#endif
    }
#if qPlatform_Windows
    inline nonvirtual Led_Distance Led_FontMetrics::GetAveCharacterWidth () const
    {
        return (fPlatformSpecific.tmAveCharWidth);
    }
#endif
#if qPlatform_MacOS
    inline Led_FontMetrics::operator const FontInfo* () const
    {
        return (&fPlatformSpecific);
    }
    inline Led_FontMetrics::operator FontInfo* ()
    {
        return (&fPlatformSpecific);
    }
#elif qPlatform_Windows
    inline Led_FontMetrics::operator const TEXTMETRIC* () const
    {
        return (&fPlatformSpecific);
    }
    inline Led_FontMetrics::operator TEXTMETRIC* ()
    {
        return (&fPlatformSpecific);
    }
#endif

    //  class   Led_Color
    inline Led_Color::Led_Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue)
        : fRed (redValue)
        , fGreen (greenValue)
        , fBlue (blueValue)
    {
    }
#if qPlatform_MacOS
    inline Led_Color::Led_Color (const RGBColor& rgbColor)
        : fRed (rgbColor.red)
        , fGreen (rgbColor.green)
        , fBlue (rgbColor.blue)
    {
    }
#elif qPlatform_Windows
    inline Led_Color::Led_Color (COLORREF colorRef)
        : fRed (static_cast<ColorValue> (GetRValue (colorRef)) << 8)
        , fGreen (static_cast<ColorValue> (GetGValue (colorRef)) << 8)
        , fBlue (static_cast<ColorValue> (GetBValue (colorRef)) << 8)
    {
    }
#endif
    inline Led_Color::ColorValue Led_Color::GetRed () const
    {
        return fRed;
    }
    inline Led_Color::ColorValue Led_Color::GetGreen () const
    {
        return fGreen;
    }
    inline Led_Color::ColorValue Led_Color::GetBlue () const
    {
        return fBlue;
    }
#if qPlatform_MacOS
    inline RGBColor Led_Color::GetOSRep () const
    {
        RGBColor r;
        r.red   = fRed;
        r.green = fGreen;
        r.blue  = fBlue;
        return r;
    }
#elif qPlatform_Windows
    inline COLORREF Led_Color::GetOSRep () const
    {
        return RGB (fRed >> 8, fGreen >> 8, fBlue >> 8);
    }
#endif
    inline bool operator== (Led_Color lhs, Led_Color rhs)
    {
        return (lhs.GetRed () == rhs.GetRed () and lhs.GetGreen () == rhs.GetGreen () and lhs.GetBlue () == rhs.GetBlue ());
    }
    inline bool operator!= (Led_Color lhs, Led_Color rhs)
    {
        return not(lhs == rhs);
    }
    inline Led_Color operator* (Led_Color lhs, float factor)
    {
        using CV = Led_Color::ColorValue;
        return Led_Color (
            static_cast<CV> (lhs.GetRed () * factor),
            static_cast<CV> (lhs.GetGreen () * factor),
            static_cast<CV> (lhs.GetBlue () * factor));
    }
    inline Led_Color operator/ (Led_Color lhs, float divBy)
    {
        return Led_Color (static_cast<Led_Color::ColorValue> (lhs.GetRed () / divBy), static_cast<Led_Color::ColorValue> (lhs.GetGreen () / divBy), static_cast<Led_Color::ColorValue> (lhs.GetBlue () / divBy));
    }
    inline Led_Color operator+ (Led_Color lhs, Led_Color rhs)
    {
        return Led_Color (lhs.GetRed () + rhs.GetRed (), lhs.GetGreen () + rhs.GetGreen (), lhs.GetBlue () + rhs.GetBlue ());
    }
    inline Led_Color operator- (Led_Color lhs, Led_Color rhs)
    {
        return Led_Color (lhs.GetRed () - rhs.GetRed (), lhs.GetGreen () - rhs.GetGreen (), lhs.GetBlue () - rhs.GetBlue ());
    }
    inline unsigned int Distance_Squared (Led_Color lhs, Led_Color rhs)
    {
        int          rDiff = static_cast<int> (lhs.GetRed ()) - static_cast<int> (rhs.GetRed ());
        int          gDiff = static_cast<int> (lhs.GetGreen ()) - static_cast<int> (rhs.GetGreen ());
        int          bDiff = static_cast<int> (lhs.GetBlue ()) - static_cast<int> (rhs.GetBlue ());
        unsigned int sum   = rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
        return sum;
    }
    inline unsigned int Distance (Led_Color lhs, Led_Color rhs)
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
    struct less<Stroika::Frameworks::Led::Led_Color> {
        bool operator() (const Stroika::Frameworks::Led::Led_Color& _Left, const Stroika::Frameworks::Led::Led_Color& _Right) const
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

//  class   Led_FontSpecification
#if qPlatform_Windows
    inline Led_FontSpecification::FontNameSpecifier::FontNameSpecifier ()
    {
        fName[0] = '\0';
    }
    inline bool operator== (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs)
    {
        return (::_tcscmp (lhs.fName, rhs.fName) == 0);
    }
    inline bool operator!= (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs)
    {
        return (::_tcscmp (lhs.fName, rhs.fName) != 0);
    }
#endif
    inline Led_FontSpecification::Led_FontSpecification ()
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
        , fTextColor (Led_Color::kBlack)
    {
#if qPlatform_Windows
        (void)::memset (&fFontInfo, 0, sizeof (fFontInfo));
#endif
    }
    inline Led_FontSpecification::Led_FontSpecification (const Led_IncrementalFontSpecification& from)
        :
#if qPlatform_MacOS
        fFontSpecifier (((const Led_FontSpecification&)from).fFontSpecifier)
        , fFontSize (((const Led_FontSpecification&)from).fFontSize)
        , fFontStyle (((const Led_FontSpecification&)from).fFontStyle)
        ,
#elif qPlatform_Windows
        fFontInfo (((const Led_FontSpecification&)from).fFontInfo)
        ,
#elif qStroika_FeatureSupported_XWindows
        fFontFamily (from.fFontFamily)
        , fBold (from.fBold)
        , fItalics (from.fItalics)
        , fUnderline (from.fUnderline)
        , fFontSize (from.fFontSize)
        ,
#endif
        fSubOrSuperScript (((const Led_FontSpecification&)from).fSubOrSuperScript)
        , fTextColor (((const Led_FontSpecification&)from).fTextColor)
    {
    }
    /*
    @METHOD:        Led_FontSpecification::GetFontName
    @DESCRIPTION:   <p>Retrieve the 'FontName' attribute of the given font specification. This name is a string, and corresponds to the font family name.</p>
                    <p>See also @'Led_FontSpecification::SetFontName'.</p>
    */
    inline Led_SDK_String Led_FontSpecification::GetFontName () const
    {
#if qPlatform_MacOS
        Str255 fontName;
        ::GetFontName (fFontSpecifier, fontName);
        size_t len = fontName[0];
        return string ((const char*)&fontName[1], len);
#elif qPlatform_Windows
        return fFontInfo.lfFaceName;
#elif qStroika_FeatureSupported_XWindows
        return fFontFamily;
#endif
    }
    // FontName info
    inline Led_FontSpecification::FontNameSpecifier Led_FontSpecification::GetFontNameSpecifier () const
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
    @METHOD:        Led_FontSpecification::GetStyle_Plain
    @DESCRIPTION:   <p>Checks all the various font 'style' attributes, such as 'bold', or 'italic'. Returns true iff all
                of these styles are not set.</p>
                <p>See also @'Led_FontSpecification::SetStyle_Plain'.</p>
    */
    inline bool Led_FontSpecification::GetStyle_Plain () const
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
    @METHOD:        Led_FontSpecification::SetStyle_Plain
    @DESCRIPTION:   <p>Clears all 'style' attributes, such as 'bold', or 'italic'.</p>
                <p>See also @'Led_FontSpecification::GetStyle_Plain'.</p>
    */
    inline void Led_FontSpecification::SetStyle_Plain ()
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
    inline bool Led_FontSpecification::GetStyle_Bold () const
    {
#if qPlatform_MacOS
        return (fFontStyle & bold);
#elif qPlatform_Windows
        return (fFontInfo.lfWeight > FW_NORMAL);
#elif qStroika_FeatureSupported_XWindows
        return fBold;
#endif
    }
    inline void Led_FontSpecification::SetStyle_Bold (bool isBold)
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
    inline bool Led_FontSpecification::GetStyle_Italic () const
    {
#if qPlatform_MacOS
        return (fFontStyle & italic);
#elif qPlatform_Windows
        return !!fFontInfo.lfItalic;
#elif qStroika_FeatureSupported_XWindows
        return fItalics;
#endif
    }
    inline void Led_FontSpecification::SetStyle_Italic (bool isItalic)
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
    inline bool Led_FontSpecification::GetStyle_Underline () const
    {
#if qPlatform_MacOS
        return (fFontStyle & underline);
#elif qPlatform_Windows
        return !!fFontInfo.lfUnderline;
#elif qStroika_FeatureSupported_XWindows
        return fUnderline;
#endif
    }
    inline void Led_FontSpecification::SetStyle_Underline (bool isUnderline)
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
    inline Led_FontSpecification::SubOrSuperScript Led_FontSpecification::GetStyle_SubOrSuperScript () const
    {
        return fSubOrSuperScript;
    }
    inline void Led_FontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
    {
        fSubOrSuperScript = subOrSuperScript;
    }
#if qPlatform_MacOS
    inline bool Led_FontSpecification::GetStyle_Outline () const
    {
        return (fFontStyle & outline);
    }
    inline void Led_FontSpecification::SetStyle_Outline (bool isOutline)
    {
        if (isOutline) {
            fFontStyle |= outline;
        }
        else {
            fFontStyle &= ~outline;
        }
    }
    inline bool Led_FontSpecification::GetStyle_Shadow () const
    {
        return (fFontStyle & shadow);
    }
    inline void Led_FontSpecification::SetStyle_Shadow (bool isShadow)
    {
        if (isShadow) {
            fFontStyle |= shadow;
        }
        else {
            fFontStyle &= ~shadow;
        }
    }
    inline bool Led_FontSpecification::GetStyle_Condensed () const
    {
        return (fFontStyle & condense);
    }
    inline void Led_FontSpecification::SetStyle_Condensed (bool isCondensed)
    {
        if (isCondensed) {
            fFontStyle |= condense;
        }
        else {
            fFontStyle &= ~condense;
        }
    }
    inline bool Led_FontSpecification::GetStyle_Extended () const
    {
        return (fFontStyle & extend);
    }
    inline void Led_FontSpecification::SetStyle_Extended (bool isExtended)
    {
        if (isExtended) {
            fFontStyle |= extend;
        }
        else {
            fFontStyle &= ~extend;
        }
    }
#elif qPlatform_Windows
    inline bool Led_FontSpecification::GetStyle_Strikeout () const
    {
        return !!fFontInfo.lfStrikeOut;
    }
    inline void Led_FontSpecification::SetStyle_Strikeout (bool isStrikeout)
    {
        fFontInfo.lfStrikeOut = isStrikeout;
    }
#endif
    // FontSize info
    inline Led_FontSpecification::FontSize Led_FontSpecification::GetPointSize () const
    {
#if qPlatform_MacOS
        return fFontSize;
#elif qPlatform_Windows
        if (fFontInfo.lfHeight >= 0) {
            // I probably should be doing some magic here with subtracing internal leading, or something like that from this value -
            // See TextImager::GetStaticDefaultFont () and Win32 SDK docs for LOGFONT
            // LGP 960222
            Led_WindowDC   screenDC (nullptr);
            Led_FontObject font;
            Verify (font.CreateFontIndirect (&fFontInfo));
            HFONT      oldFont = screenDC.SelectObject (font);
            TEXTMETRIC tms;
            screenDC.GetTextMetrics (&tms);
            screenDC.SelectObject (oldFont);
            return (unsigned short)::MulDiv (tms.tmHeight, 72, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
        }
        else {
            return static_cast<unsigned short> (::MulDiv (-fFontInfo.lfHeight, 72, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ()));
        }
#elif qStroika_FeatureSupported_XWindows
        return fFontSize;
#endif
    }
    inline void Led_FontSpecification::SetPointSize (FontSize pointSize)
    {
#if qPlatform_MacOS
        fFontSize = pointSize;
#elif qPlatform_Windows
        fFontInfo.lfHeight = ::MulDiv (-long (pointSize), Led_GDIGlobals::Get ().GetMainScreenLogPixelsV (), 72);
#elif qStroika_FeatureSupported_XWindows
        fFontSize = pointSize;
#endif
    }
#if qPlatform_Windows
    inline long Led_FontSpecification::PeekAtTMHeight () const
    {
        return fFontInfo.lfHeight;
    }
    inline void Led_FontSpecification::PokeAtTMHeight (long tmHeight)
    {
        fFontInfo.lfHeight = tmHeight;
    }
#endif
    inline Led_Color Led_FontSpecification::GetTextColor () const
    {
        return fTextColor;
    }
    inline void Led_FontSpecification::SetTextColor (const Led_Color& textColor)
    {
        fTextColor = textColor;
    }
#if qPlatform_MacOS
    inline void Led_FontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
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
    inline void Led_FontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
    {
        fFontSpecifier = fontID;
        fFontSize      = fontSize;
        fFontStyle     = fontStyle;
    }
#elif qPlatform_Windows
    inline LOGFONT Led_FontSpecification::GetOSRep () const
    {
        return fFontInfo;
    }
    inline void Led_FontSpecification::GetOSRep (LOGFONT* logFont) const
    {
        RequireNotNull (logFont);
        *logFont = fFontInfo;
    }
    inline void Led_FontSpecification::SetOSRep (LOGFONT logFont)
    {
        fFontInfo = logFont;
    }
    inline void Led_FontSpecification::LightSetOSRep (LOGFONT logFont)
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
    inline Led_FontSpecification::Led_FontSpecification (const LOGFONT& logFont)
        : fFontInfo ()
        , fSubOrSuperScript (eNoSubOrSuperscript)
        , fTextColor (Led_Color::kBlack)
    {
        LightSetOSRep (logFont);
    }
#endif
    inline bool operator== (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
    {
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
    inline bool operator!= (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
    {
        return not(lhs == rhs);
    }

    //  class   Led_IncrementalFontSpecification
    inline Led_IncrementalFontSpecification::Led_IncrementalFontSpecification ()
        : Led_FontSpecification ()
        , fFontSpecifierValid (false)
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
    inline Led_IncrementalFontSpecification::Led_IncrementalFontSpecification (const Led_FontSpecification& fontSpec)
        : Led_FontSpecification (fontSpec)
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
    // FontName info
    inline Led_FontSpecification::FontNameSpecifier Led_IncrementalFontSpecification::GetFontNameSpecifier () const
    {
        Require (fFontSpecifierValid);
        return inherited::GetFontNameSpecifier ();
    }
    inline bool Led_IncrementalFontSpecification::GetFontNameSpecifier_Valid () const
    {
        return (fFontSpecifierValid);
    }
    inline void Led_IncrementalFontSpecification::InvalidateFontNameSpecifier ()
    {
        fFontSpecifierValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
    {
        fFontSpecifierValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetFontNameSpecifier (fontNameSpecifier);
    }
    inline void Led_IncrementalFontSpecification::SetFontName (const Led_SDK_String& fontName)
    {
        fFontSpecifierValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetFontName (fontName);
    }
    // Style info
    inline bool Led_IncrementalFontSpecification::GetStyle_Plain () const
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
    inline bool Led_IncrementalFontSpecification::GetStyle_Plain_Valid () const
    {
        bool isValid = fStyleValid_Bold and fStyleValid_Italic and fStyleValid_Underline and fStyleValid_SubOrSuperScript;
#if qPlatform_MacOS
        isValid = isValid and fStyleValid_Outline and fStyleValid_Shadow and fStyleValid_Condensed and fStyleValid_Extended;
#elif qPlatform_Windows
        isValid               = isValid and fStyleValid_Strikeout;
#endif
        return isValid;
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyles ()
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
    inline void Led_IncrementalFontSpecification::SetStyle_Plain ()
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
    inline bool Led_IncrementalFontSpecification::GetStyle_Bold () const
    {
        Require (fStyleValid_Bold);
        return inherited::GetStyle_Bold ();
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Bold_Valid () const
    {
        return (fStyleValid_Bold);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Bold ()
    {
        fStyleValid_Bold = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Bold (bool isBold)
    {
        fStyleValid_Bold = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Bold (isBold);
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Italic () const
    {
        Require (fStyleValid_Italic);
        return inherited::GetStyle_Italic ();
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Italic_Valid () const
    {
        return (fStyleValid_Italic);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Italic ()
    {
        fStyleValid_Italic = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Italic (bool isItalic)
    {
        fStyleValid_Italic = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Italic (isItalic);
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Underline () const
    {
        Require (fStyleValid_Underline);
        return inherited::GetStyle_Underline ();
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Underline_Valid () const
    {
        return (fStyleValid_Underline);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Underline ()
    {
        fStyleValid_Underline = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Underline (bool isUnderline)
    {
        fStyleValid_Underline = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_Underline (isUnderline);
    }
    inline Led_FontSpecification::SubOrSuperScript Led_IncrementalFontSpecification::GetStyle_SubOrSuperScript () const
    {
        Require (fStyleValid_SubOrSuperScript);
        return inherited::GetStyle_SubOrSuperScript ();
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_SubOrSuperScript_Valid () const
    {
        return (fStyleValid_SubOrSuperScript);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_SubOrSuperScript ()
    {
        fStyleValid_SubOrSuperScript = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript)
    {
        fStyleValid_SubOrSuperScript = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetStyle_SubOrSuperScript (subOrSuperScript);
    }
#if qPlatform_MacOS
    inline bool Led_IncrementalFontSpecification::GetStyle_Outline () const
    {
        Require (fStyleValid_Outline);
        return (inherited::GetStyle_Outline ());
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Outline_Valid () const
    {
        return (fStyleValid_Outline);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Outline ()
    {
        fStyleValid_Outline = false;
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Outline (bool isOutline)
    {
        fStyleValid_Outline = true;
        inherited::SetStyle_Outline (isOutline);
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Shadow () const
    {
        Require (fStyleValid_Shadow);
        return (inherited::GetStyle_Shadow ());
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Shadow_Valid () const
    {
        return (fStyleValid_Shadow);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Shadow ()
    {
        fStyleValid_Shadow = false;
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Shadow (bool isShadow)
    {
        fStyleValid_Shadow = true;
        inherited::SetStyle_Shadow (isShadow);
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Condensed () const
    {
        Require (fStyleValid_Condensed);
        return (inherited::GetStyle_Condensed ());
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Condensed_Valid () const
    {
        return (fStyleValid_Condensed);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Condensed ()
    {
        fStyleValid_Condensed = false;
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Condensed (bool isCondensed)
    {
        fStyleValid_Condensed = true;
        inherited::SetStyle_Condensed (isCondensed);
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Extended () const
    {
        Require (fStyleValid_Extended);
        return (inherited::GetStyle_Extended ());
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Extended_Valid () const
    {
        return (fStyleValid_Extended);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Extended ()
    {
        fStyleValid_Extended = false;
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Extended (bool isExtended)
    {
        fStyleValid_Extended = true;
        inherited::SetStyle_Extended (isExtended);
    }
#elif qPlatform_Windows
    inline bool Led_IncrementalFontSpecification::GetStyle_Strikeout () const
    {
        Require (fStyleValid_Strikeout);
        return (inherited::GetStyle_Strikeout ());
    }
    inline bool Led_IncrementalFontSpecification::GetStyle_Strikeout_Valid () const
    {
        return (fStyleValid_Strikeout);
    }
    inline void Led_IncrementalFontSpecification::InvalidateStyle_Strikeout ()
    {
        fStyleValid_Strikeout = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag  = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetStyle_Strikeout (bool isStrikeout)
    {
        fStyleValid_Strikeout = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag  = false;
#endif
        inherited::SetStyle_Strikeout (isStrikeout);
    }
#endif
    // FontSize info
    inline unsigned short Led_IncrementalFontSpecification::GetPointSize () const
    {
        Require (fFontSizeValid);
        Require (not fFontSizeIncrementValid);
        return inherited::GetPointSize ();
    }
    inline bool Led_IncrementalFontSpecification::GetPointSize_Valid () const
    {
        return (fFontSizeValid);
    }
    inline void Led_IncrementalFontSpecification::InvalidatePointSize ()
    {
        fFontSizeValid          = false;
        fFontSizeIncrementValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetPointSize (FontSize pointSize)
    {
        fFontSizeValid = true;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetPointSize (pointSize);
        fFontSizeIncrementValid = false;
    }
#if qPlatform_Windows
    inline void Led_IncrementalFontSpecification::PokeAtTMHeight (long tmHeight)
    {
        fFontSizeValid          = true;
        fDidSetOSRepCallFlag    = false;
        fFontSizeIncrementValid = false;
        inherited::PokeAtTMHeight (tmHeight);
    }
#endif
    inline short Led_IncrementalFontSpecification::GetPointSizeIncrement () const
    {
        Require (not fFontSizeValid);
        Require (fFontSizeIncrementValid);
        return (short)inherited::GetPointSize ();
    }
    inline bool Led_IncrementalFontSpecification::GetPointSizeIncrement_Valid () const
    {
        return (fFontSizeIncrementValid);
    }
    inline void Led_IncrementalFontSpecification::InvalidatePointSizeIncrement ()
    {
        fFontSizeValid          = false;
        fFontSizeIncrementValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
    }
    inline void Led_IncrementalFontSpecification::SetPointSizeIncrement (short pointSizeIncrement)
    {
        fFontSizeValid = false;
#if qPlatform_Windows
        fDidSetOSRepCallFlag = false;
#endif
        inherited::SetPointSize ((unsigned short)pointSizeIncrement);
        fFontSizeIncrementValid = true;
    }
    // FontFaceColor info
    inline Led_Color Led_IncrementalFontSpecification::GetTextColor () const
    {
        Require (fTextColorValid);
        return inherited::GetTextColor ();
    }
    inline bool Led_IncrementalFontSpecification::GetTextColor_Valid () const
    {
        return (fTextColorValid);
    }
    inline void Led_IncrementalFontSpecification::InvalidateTextColor ()
    {
        fTextColorValid = false;
    }
    inline void Led_IncrementalFontSpecification::SetTextColor (const Led_Color& textColor)
    {
        fTextColorValid = true;
        inherited::SetTextColor (textColor);
    }
#if qPlatform_MacOS
    inline void Led_IncrementalFontSpecification::GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const
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
    inline void Led_IncrementalFontSpecification::SetOSRep (short fontID, short fontSize, Style fontStyle)
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
    inline LOGFONT Led_IncrementalFontSpecification::GetOSRep () const
    {
        Require (fFontSpecifierValid and
                 fStyleValid_Bold and
                 fStyleValid_Italic and
                 fStyleValid_Underline and
                 fFontSizeValid);
        Require (fStyleValid_Strikeout);
        return inherited::GetOSRep ();
    }
    inline void Led_IncrementalFontSpecification::GetOSRep (LOGFONT* logFont) const
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
    inline void Led_IncrementalFontSpecification::SetOSRep (LOGFONT logFont)
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
    inline void Led_IncrementalFontSpecification::LightSetOSRep (LOGFONT logFont)
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
    inline bool Led_IncrementalFontSpecification::GetDidSetOSRepCallFlag () const
    {
        return fDidSetOSRepCallFlag;
    }
#endif

    inline void Led_FontSpecification::MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes)
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

    inline void Led_IncrementalFontSpecification::MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes)
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

    inline bool operator== (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
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
#if qPlatform_MacOS
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
#elif qPlatform_Windows
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
            }
        }

        return true;
    }
    inline bool operator!= (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
    {
        return not(lhs == rhs);
    }

    //  class   Led_InstalledFonts
    inline const vector<Led_SDK_String>& Led_InstalledFonts::GetUsableFontNames () const
    {
        return fFontNames;
    }

    inline Led_Color Led_GetTextColor ()
    {
#if qPlatform_MacOS
        return (Led_Color::kBlack);
#elif qPlatform_Windows
        return Led_Color (::GetSysColor (COLOR_WINDOWTEXT));
#elif qStroika_FeatureSupported_XWindows
        return (Led_Color::kBlack);
#endif
    }
    inline Led_Color Led_GetTextBackgroundColor ()
    {
#if qPlatform_MacOS
        return (Led_Color::kWhite);
#elif qPlatform_Windows
        return Led_Color (::GetSysColor (COLOR_WINDOW));
#elif qStroika_FeatureSupported_XWindows
        return (Led_Color::kWhite);
#endif
    }
    inline Led_Color Led_GetSelectedTextColor ()
    {
#if qPlatform_MacOS
        RGBColor hiliteRGBValue;
        LMGetHiliteRGB (&hiliteRGBValue);
        /*
            *  This is based on empirical testing with the behavior of the TE in the Color desk accessory
            *  that comes with system 7.5. I REALLY should probably do something with color intensity
            *  matching -- LGP 950531
            */
        if (Led_Color (hiliteRGBValue) == Led_Color::kBlack) {
            return (Led_Color::kWhite);
        }
        else {
            return (Led_Color::kBlack);
        }
#elif qPlatform_Windows
        //          return Led_Color (::GetSysColor (COLOR_CAPTIONTEXT));
        return Led_Color (::GetSysColor (COLOR_HIGHLIGHTTEXT));
#elif qStroika_FeatureSupported_XWindows
        return (Led_Color::kWhite);
#endif
    }
    inline Led_Color Led_GetSelectedTextBackgroundColor ()
    {
#if qPlatform_MacOS
        RGBColor hiliteRGBValue;
        LMGetHiliteRGB (&hiliteRGBValue);
        return Led_Color (hiliteRGBValue);
#elif qPlatform_Windows
        //return Led_Color (::GetSysColor (COLOR_ACTIVECAPTION));
        return Led_Color (::GetSysColor (COLOR_HIGHLIGHT));
#elif qStroika_FeatureSupported_XWindows
        return (Led_Color::kBlack);
#endif
    }

    //  class   Led_Tablet_::ClipNarrowAndRestore
    inline Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet)
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
    inline Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Rect& clipFurtherTo)
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
    inline Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet, [[maybe_unused]] const Led_Region& clipFurtherTo)
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
    inline Led_Tablet_::ClipNarrowAndRestore::~ClipNarrowAndRestore ()
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

#if qPlatform_MacOS
    inline Led_MacPortAndClipRegionEtcSaver::Led_MacPortAndClipRegionEtcSaver ()
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
    inline Led_MacPortAndClipRegionEtcSaver::~Led_MacPortAndClipRegionEtcSaver ()
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

//  class   Led_GDI_Obj_Selector
#if qPlatform_Windows
    inline Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, HGDIOBJ objToSelect)
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
    inline Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen)
        : fTablet (tablet)
        ,
#if TARGET_CARBON
        fRestorePen (Led_Pen (::GetPortPenMode (Led_GetCurrentGDIPort ()), &Led_Pen::kBlackPattern, Led_Color (GDI_GetForeColor ())))
#else
        fRestorePen (Led_Pen (Led_GetCurrentGDIPort ()->pnMode, &Led_GetCurrentGDIPort ()->pnPat, Led_Color (GDI_GetForeColor ())))
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
    inline Led_GDI_Obj_Selector::Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen)
    {
    }
#endif
    inline Led_GDI_Obj_Selector::~Led_GDI_Obj_Selector ()
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
    DISABLE_COMPILER_MSC_WARNING_START (6011)
    inline Led_IME& Led_IME::Get ()
    {
        if (sThe == nullptr) {
            new Led_IME ();
        }
        AssertNotNull (sThe);
        return *sThe;
    }
    DISABLE_COMPILER_MSC_WARNING_END (6011)
    inline void Led_IME::Enable ()
    {
        if (fIMEEnableProc != nullptr) {
            fIMEEnableProc (nullptr, true);
        }
    }
    inline void Led_IME::Disable ()
    {
        if (fIMEEnableProc != nullptr) {
            fIMEEnableProc (nullptr, false);
        }
    }
    inline bool Led_IME::Available () const
    {
        return fWinNlsAvailable;
    }
    inline void Led_IME::ForgetPosition ()
    {
        fLastX = -1;
        fLastY = -1;
    }
#endif

}

#endif /*_Stroika_Frameworks_Led_GDI_inl_*/
