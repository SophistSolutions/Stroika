/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_GDI_h_
#define _Stroika_Frameworks_Led_GDI_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    LedGDI
@DESCRIPTION:
        <p>Graphix Library support. This code encapsulates access to underlying GDI for modularity sake.
    For historical reasons - as of Led 3.0 - there is still some direct access to underlying GDI in parts of Led
    but that will gradually (hopefully right after 3.0) be eliminated.</p>
        <p>Also - no class library dependencies (ie no MFC dependencies). So we can build MFC-Free Win32 apps more
    easily.</p>
 */

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "Support.h"

// WHY DON'T WE NEED TO INCLUDE <Windows.h> here??? - LGP 991213

#if qStroika_FeatureSupported_XWindows
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         Led_Coordinate
    @DESCRIPTION:   <p><code>Led_Coordinate</code> is the <code>signed</code> analog of @'Led_Distance'.
        @'Led_Point' is a tuple of <code>Led_Coordinates</code>s
        (vertical and horizontal).</p>
    */
    using Led_Coordinate = long;

    /*
    @CLASS:         Led_Distance
    @DESCRIPTION:   <p><code>Led_Distance</code> is an unsigned type, specifying the distance (always non-negative)
        between two graphics locations. @'Led_Size' is a tuple of <code>Led_Distance</code>s
        (vertical and horizontal).</p>
    */
    using Led_Distance = unsigned long;

    /*
    @CLASS:         Led_TWIPS
    @DESCRIPTION:   <p>Many distances are specified in Led in TWIPS - 1/20 of a printers point.
        This means - 1/1440 of an inch.</p>
            <p>This size refers to the size when printed on a page. So it can be scaled - depnding on screen resolution.
        Led will often save this internally - and scale it at the last minute to the resolution of the @'Led_Tablet' being printed on.</p>
            <p>NB: This marks a change from Led 2.3 and earlier - where most distances were stored in pixels (still many are).</p>
            <p>NB: declard as a class instead of a typedef so we get better type checking. Shouldn't affect sizes or code
        generation - I would hope!</p>
    */
    class Led_TWIPS {
    public:
        explicit Led_TWIPS (long v);
        operator long () const;

    private:
        long fValue;

    public:
        nonvirtual Led_TWIPS& operator+= (const Led_TWIPS& rhs);
        nonvirtual Led_TWIPS& operator-= (const Led_TWIPS& rhs);
        nonvirtual Led_TWIPS& operator*= (double rhs);

    public:
        static const Led_TWIPS kPoint;
        static const Led_TWIPS kInch;
        static const Led_TWIPS kOneInch;
    };

    Led_TWIPS operator+ (const Led_TWIPS& lhs, const Led_TWIPS& rhs);
    Led_TWIPS operator- (const Led_TWIPS& lhs, const Led_TWIPS& rhs);

    class Led_Tablet_;

    /*
    @CLASS:         Led_TabStopList
    @DESCRIPTION:   <p>Even though WinSDK supports GetTabbedTextExtent/TabbedTextOut () - they do a bad job.
        In particular, they offer no (obvious) way to specify the tab origin for
        GetTabbedTextExtent(). This makes proper text display nearly impossible.</p>
            <p>Also, the Mac provides NO tab support at all. Because of the Mac - we NEED
        to implement our own tab support. Given we are doing that anyhow, may as
        well use our portable support on both platforms.</p>
            <p>For both calculating widths, and doing imaging, we follow essentially
        the same algorithm.
        <ul>
            <li>    Walk the text looking for a tab or EOL.
            <li>    Take the tab-free initial segment and image(calculate) it.
            <li>    if now on a tab, image (calculate) that.
            <li>    Advance indexes over what we've imaged (calculated) and repeat.
        </ul>
        </p>
    */
    class Led_TabStopList {
    protected:
        Led_TabStopList ();

    public:
        virtual ~Led_TabStopList ();

    public:
        /*
        @METHOD:        TextImager::TabStopList::ComputeIthTab
        @DESCRIPTION:   <p>Compute where the 'ith' tabstop lies (i >= 0, and can be arbitrarily high).
            Note that though tabstop <code>i=0</code> is the first tabstop (which by convention is usually NOT
            at the left margin - but is really the logically 'first' one).</p>
                <p>Note - this used to return Led_Distance - but in Led 3.0 - it changed to returning TWIPS (see SPR#0767);</p>
        */
        virtual Led_TWIPS ComputeIthTab (size_t i) const = 0;
        /*
        @METHOD:        TextImager::TabStopList::ComputeTabStopAfterPosition
        @DESCRIPTION:   <p>Given a distance from the left margin, find the immediately following tabstops position.
            This is what is actaully used in the imagers to compute where to draw text. It can logically
            be implemented by walking the tabstoplist and calling @'TextImager::TabStopList::ComputeIthTab', and
            returning the next one after the one past(or equal to) <code>afterPos</code>.</p>
                <p>Note - this used to take/return Led_Distance - but in Led 3.0 - it changed to take/return TWIPS (see SPR#0767);</p>
                <p>Now there is an overloaded version using Led_Distance that takes a Led_Tablet as argument.</p>
        */
        virtual Led_TWIPS ComputeTabStopAfterPosition (Led_TWIPS afterPos) const = 0;
        nonvirtual Led_Distance ComputeTabStopAfterPosition (Led_Tablet_* tablet, Led_Distance afterPos) const;
    };

#if qPlatform_MacOS

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

#elif qPlatform_Windows

    class Led_FontObject {
    public:
        Led_FontObject ()
            : m_hObject (nullptr)
        {
        }
        ~Led_FontObject ()
        {
            (void)DeleteObject ();
        }
        nonvirtual operator HFONT () const
        {
            return m_hObject;
        }
        nonvirtual int GetObject (int nCount, LPVOID lpObject) const
        {
            Assert (m_hObject != nullptr);
            return ::GetObject (m_hObject, nCount, lpObject);
        }
        nonvirtual BOOL DeleteObject ()
        {
            if (m_hObject == nullptr)
                return FALSE;
            HFONT h   = m_hObject;
            m_hObject = nullptr;
            return ::DeleteObject (h);
        }
        nonvirtual BOOL CreateFontIndirect (const LOGFONT* lpLogFont)
        {
            return Attach (::CreateFontIndirect (lpLogFont));
        }
        nonvirtual BOOL Attach (HFONT hObject)
        {
            Assert (m_hObject == nullptr); // only attach once, detach on destroy
            if (hObject == nullptr)
                return FALSE;
            m_hObject = hObject;
            return TRUE;
        }

    public:
        HFONT m_hObject;
    };

    class Led_Brush {
    public:
        Led_Brush (COLORREF crColor)
            : m_hObject (nullptr)
        {
            if (!Attach (::CreateSolidBrush (crColor)))
                Led_ThrowOutOfMemoryException ();
        }
        ~Led_Brush ()
        {
            (void)DeleteObject ();
        }
        nonvirtual operator HBRUSH () const
        {
            return m_hObject;
        }
        nonvirtual BOOL Attach (HBRUSH hObject)
        {
            Assert (m_hObject == nullptr); // only attach once, detach on destroy
            if (hObject == nullptr)
                return FALSE;
            m_hObject = hObject;
            return TRUE;
        }
        nonvirtual BOOL DeleteObject ()
        {
            if (m_hObject == nullptr)
                return FALSE;
            HBRUSH h  = m_hObject;
            m_hObject = nullptr;
            return ::DeleteObject (h);
        }

    private:
        HBRUSH m_hObject;
    };

#endif

    /*
    @CLASS:         Point_Base<COORD_TYPE>
    @DESCRIPTION:
    */
    template <typename COORD_TYPE>
    struct Point_Base {
    public:
        using CoordinateType = COORD_TYPE;

    public:
        Point_Base ();
        Point_Base (COORD_TYPE newV, COORD_TYPE newH);
#if !qNestedTemplateCTORInTemplateBug
        template <typename OTHER_POINT_BASE_TYPE>
        explicit Point_Base (OTHER_POINT_BASE_TYPE o)
            : v (COORD_TYPE (o.v))
            , h (COORD_TYPE (o.h))
        {
        }
#endif

    public:
        COORD_TYPE v;
        COORD_TYPE h;
    };
    template <typename COORD_TYPE>
    bool operator== (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    bool operator!= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    bool operator< (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    bool operator<= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    bool operator> (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    bool operator>= (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);
    template <typename COORD_TYPE>
    Point_Base<COORD_TYPE> operator+ (const Point_Base<COORD_TYPE>& lhs, const Point_Base<COORD_TYPE>& rhs);

    /*
    @CLASS:         Rect_Base
    @DESCRIPTION:
    */
    template <typename POINT_TYPE, typename SIZE_TYPE>
    struct Rect_Base {
    public:
        using CoordinateType = typename POINT_TYPE::CoordinateType;
        using DistanceType   = typename SIZE_TYPE::CoordinateType;
        using THIS_TYPE      = Rect_Base<POINT_TYPE, SIZE_TYPE>;

    public:
        Rect_Base ();
        Rect_Base (CoordinateType newTop, CoordinateType newLeft, DistanceType newHeight, DistanceType newWidth);
        Rect_Base (POINT_TYPE origin, SIZE_TYPE size);

    public:
        nonvirtual CoordinateType GetTop () const;
        nonvirtual CoordinateType GetLeft () const;
        nonvirtual CoordinateType GetBottom () const;
        nonvirtual CoordinateType GetRight () const;
        nonvirtual bool           IsEmpty () const; /* contains no bits - ie one side <= zero length */
        nonvirtual DistanceType GetHeight () const;
        nonvirtual DistanceType GetWidth () const;
        nonvirtual POINT_TYPE GetTopLeft () const;
        nonvirtual POINT_TYPE GetTopRight () const;
        nonvirtual POINT_TYPE GetBotRight () const;
        nonvirtual POINT_TYPE GetOrigin () const;
        nonvirtual SIZE_TYPE GetSize () const;
        nonvirtual bool      Contains (POINT_TYPE p) const;
        nonvirtual bool      Contains (THIS_TYPE r) const;

        nonvirtual void SetOrigin (POINT_TYPE p);
        nonvirtual void SetTop (CoordinateType top);
        nonvirtual void SetLeft (CoordinateType left);
        nonvirtual void SetBottom (CoordinateType bottom);
        nonvirtual void SetRight (CoordinateType right);

    public:
        CoordinateType top;
        CoordinateType left;
        CoordinateType bottom;
        CoordinateType right;

    public:
        nonvirtual const Rect_Base<POINT_TYPE, SIZE_TYPE>& operator+= (const POINT_TYPE& delta);
        nonvirtual const Rect_Base<POINT_TYPE, SIZE_TYPE>& operator-= (const POINT_TYPE& delta);
        nonvirtual const Rect_Base<POINT_TYPE, SIZE_TYPE>& operator*= (const THIS_TYPE& intersectWith);
    };

    template <typename POINT_TYPE, typename SIZE_TYPE>
    bool operator== (const Rect_Base<POINT_TYPE, SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE, SIZE_TYPE>& rhs);
    template <typename POINT_TYPE, typename SIZE_TYPE>
    bool operator!= (const Rect_Base<POINT_TYPE, SIZE_TYPE>& lhs, const Rect_Base<POINT_TYPE, SIZE_TYPE>& rhs);

#if qNestedTemplateCTORInTemplateBug
    struct Led_Point;
    struct Led_Size : public Point_Base<Led_Distance> {
        using inherited = Point_Base<Led_Distance>;
        inline Led_Size ()
            : inherited ()
        {
        }
        inline Led_Size (inherited i)
            : inherited (i)
        {
        }
        inline Led_Size (Led_Distance newV, Led_Distance newH)
            : inherited (newV, newH)
        {
        }
        Led_Size (Led_Point p);
    };
#else
    /*
    @CLASS:         Led_Size
    @DESCRIPTION:   <p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_Distance'.</p>
    */
    using Led_Size  = Point_Base<Led_Distance>;
#endif

/*
    @CLASS:         Led_Point
    @DESCRIPTION:   <p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_Coordinate'.</p>
    */
#if qNestedTemplateCTORInTemplateBug
    struct Led_Point : public Point_Base<Led_Coordinate> {
        using inherited = Point_Base<Led_Coordinate>;
        inline Led_Point ()
            : inherited ()
        {
        }
        inline Led_Point (inherited i)
            : inherited (i)
        {
        }
        inline Led_Point (Led_Coordinate newV, Led_Coordinate newH)
            : inherited (newV, newH)
        {
        }
        Led_Point (Led_Size p);
    };
    inline Led_Size::Led_Size (Led_Point p)
        : inherited (p.v, p.h)
    {
    }
    inline Led_Point::Led_Point (Led_Size p)
        : inherited (p.v, p.h)
    {
    }
#else
    using Led_Point = Point_Base<Led_Coordinate>;
#endif
    Led_Point operator- (const Led_Point& lhs, const Led_Point& rhs);

    /*
    @CLASS:         Led_TWIPS_Point
    @DESCRIPTION:   <p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'Led_TWIPS'.</p>
    */
    using Led_TWIPS_Point = Point_Base<Led_TWIPS>;

    /*
    @CLASS:         Led_Rect
    @DESCRIPTION:   <p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'Led_Point' and @'Led_Size'.</p>
    */
    using Led_Rect = Rect_Base<Led_Point, Led_Size>;

    /*
    @CLASS:         Led_TWIPS_Rect
    @DESCRIPTION:   <p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'Led_TWIPS_Point'.</p>
    */
    using Led_TWIPS_Rect = Rect_Base<Led_TWIPS_Point, Led_TWIPS_Point>;

#if qPlatform_Windows
    /*
    @CLASS:         auto_gdi_ptr
    @DESCRIPTION:       <p>Like auto_ptr<> - except to just assure a Win32 GDI object is deleted. NB: We cannot
                    use a template specialization of auto_ptr<> because HGDIOBJ is really just a simple typedef
                    (void*) - and so we would be specializing the deletion of any void* typedef (more than
                    really desired and less than safe).</p>
    */
    class auto_gdi_ptr {
    public:
        auto_gdi_ptr (HGDIOBJ gdiObj);
        ~auto_gdi_ptr ();

    private:
        HGDIOBJ fGDIObj;
    };
#endif

    /*
    @CLASS:         Led_Region
    @DESCRIPTION:   <p>Portable GDI abstraction for 'Region' object.</p>
    */
    class Led_Region {
    public:
        Led_Region ();
        Led_Region (const Led_Rect& r);
        Led_Region (const Led_Region& from);
        virtual ~Led_Region ();
        nonvirtual const Led_Region& operator= (const Led_Region& rhs);

    public:
        nonvirtual bool IsEmpty () const;
        nonvirtual Led_Rect GetBoundingRect () const;

    public:
#if qPlatform_MacOS
        Led_Region (RgnHandle rgn)
            : fRgn (rgn)
            , fOwned (false)
        {
        }
        RgnHandle GetOSRep () const
        {
            return fRgn;
        }
        RgnHandle GetOSRep ()
        {
            return fRgn;
        }

    private:
        bool fOwned;
#elif qPlatform_Windows
        operator HRGN () const
        {
            return fRgn;
        }
        int CombineRgn (Led_Region* pRgn1, Led_Region* pRgn2, int nCombineMode)
        {
            Require (pRgn1 != nullptr);
            Require (pRgn2 != nullptr);
            Require (fRgn != nullptr);
            return ::CombineRgn (fRgn, pRgn1->fRgn, pRgn2->fRgn, nCombineMode);
        }
        BOOL PtInRegion (int x, int y) const
        {
            Require (fRgn != nullptr);
            return ::PtInRegion (fRgn, x, y);
        }
        BOOL PtInRegion (POINT point) const
        {
            Require (fRgn != nullptr);
            return ::PtInRegion (fRgn, point.x, point.y);
        }

    private:
        BOOL DeleteObject ()
        {
            if (fRgn == nullptr)
                return FALSE;
            HRGN r = fRgn;
            fRgn   = nullptr;
            return ::DeleteObject (r);
        }
#endif
    private:
#if qPlatform_MacOS
        RgnHandle fRgn;
#elif qPlatform_Windows
        HRGN fRgn;
#endif
    };

    Led_Region operator* (const Led_Region& lhs, const Led_Region& rhs);
    Led_Region operator+ (const Led_Region& lhs, const Led_Region& rhs);

    /*
    @CLASS:         Led_Color
    @DESCRIPTION:   <p>This class is a portable representation of a color. It can be constructed either
                from its basic RGB componets, or from the native color representations on a particular platform.</p>
    */
    class Led_Color {
    public:
        // regardless of Mac or Windows, we use the same size ColorValue so we can write
        // portable code more easily...
        using ColorValue = unsigned short;
        enum { kColorValueMin = 0,
               kColorValueMax = USHRT_MAX };

    public:
        explicit Led_Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue);
#if qPlatform_MacOS
        explicit Led_Color (const RGBColor& rgbColor);
#elif qPlatform_Windows
        explicit Led_Color (COLORREF colorRef);
#endif

    public:
        nonvirtual ColorValue GetRed () const;
        nonvirtual ColorValue GetGreen () const;
        nonvirtual ColorValue GetBlue () const;

        /*
            *  Some useful predefined values.
            */
    public:
        static const Led_Color kBlack;
        static const Led_Color kWhite;
        static const Led_Color kRed;
        static const Led_Color kGreen;
        static const Led_Color kBlue;
        static const Led_Color kCyan;
        static const Led_Color kMagenta;
        static const Led_Color kYellow;
        static const Led_Color kMaroon;
        static const Led_Color kOlive;
        static const Led_Color kNavyBlue;
        static const Led_Color kPurple;
        static const Led_Color kTeal;
        static const Led_Color kGray;
        static const Led_Color kSilver; // aka 'light gray'
        static const Led_Color kDarkGreen;
        static const Led_Color kLimeGreen;
        static const Led_Color kFuchsia;
        static const Led_Color kAqua;

    public:
#if qPlatform_MacOS
        nonvirtual RGBColor GetOSRep () const;
#elif qPlatform_Windows
        nonvirtual COLORREF GetOSRep () const;
#endif
    private:
        ColorValue fRed;
        ColorValue fGreen;
        ColorValue fBlue;
    };
    bool operator== (Led_Color lhs, Led_Color rhs);
    bool operator!= (Led_Color lhs, Led_Color rhs);

    Led_Color operator* (Led_Color lhs, float factor);
    Led_Color operator/ (Led_Color lhs, float divBy);
    Led_Color operator+ (Led_Color lhs, Led_Color rhs);
    Led_Color operator- (Led_Color lhs, Led_Color rhs);

    unsigned int Distance (Led_Color lhs, Led_Color rhs);
    unsigned int Distance_Squared (Led_Color lhs, Led_Color rhs);
#if qPlatform_Windows
    unsigned int Distance_Squared (COLORREF lhs, COLORREF rhs);
#endif

    /*
    @CLASS:         Led_Pen
    @DESCRIPTION:   <p>Helper class to keep track of GDI information used for drawing.
                Very different implementations befween Mac and Windows.</p>
                    <p>Note - this class is used in conjunction with @'Led_GDI_Obj_Selector'.</p>
    */
    class Led_Pen {
#if qPlatform_Windows
    public:
        Led_Pen (int nPenStyle, int nWidth, COLORREF crColor)
            : m_hObject (nullptr)
        {
            if (!Attach (::CreatePen (nPenStyle, nWidth, crColor)))
                Led_ThrowOutOfMemoryException ();
        }
        ~Led_Pen ()
        {
            (void)DeleteObject ();
        }
        nonvirtual operator HPEN () const
        {
            return m_hObject;
        }
        nonvirtual BOOL Attach (HPEN hObject)
        {
            Assert (m_hObject == nullptr); // only attach once, detach on destroy
            if (hObject == nullptr)
                return FALSE;
            m_hObject = hObject;
            return TRUE;
        }
        nonvirtual BOOL DeleteObject ()
        {
            if (m_hObject == nullptr)
                return FALSE;
            HPEN h    = m_hObject;
            m_hObject = nullptr;
            return ::DeleteObject (h);
        }

    private:
        HPEN m_hObject;
#endif
#if qPlatform_MacOS
    public:
        static const Pattern kWhitePattern;
        static const Pattern kLightGrayPattern;
        static const Pattern kGrayPattern;
        static const Pattern kDarkGrayPattern;
        static const Pattern kBlackPattern;

    public:
        Led_Pen (short penStyle = srcCopy, const Pattern* penPat = &kBlackPattern, const Led_Color& color = Led_Color::kBlack)
            : fPenStyle (penStyle)
            , fPenPat (*penPat)
            , fPenColor (color)
        {
        }

    public:
        short     fPenStyle;
        Pattern   fPenPat;
        Led_Color fPenColor;
#endif
#if qStroika_FeatureSupported_XWindows
    public:
        Led_Pen () {}
#endif
    };

    /*
    @CLASS:         Led_Justification
    @DESCRIPTION:   <p>Led defines several kinds of justification, but doesn't implement all of them.
        <ul>
            <li><em>eLeftJustify</em></li>
            <li><em>eRightJustify</em></li>
            <li><em>eCenterJustify</em></li>
            <li><em>eFullyJustify</em>- not implemented in Led 3.0 - treated as eLeftJustify</li>
            <li><em>eDefaultForScriptJustify</em>-not implemented in Led 3.0 - treated as eLeftJustify</li>
        </ul>
        </p>
            <p>These extra unimplemented enums are provided so you can more easily write code and read/write files
        etc which keep track of this information, and even show the styles in the UI. They just aren't reflected
        in how the text is drawn yet. That should come in the next major Led release.</p>
    */
    enum Led_Justification { eLeftJustify,
                             eRightJustify,
                             eCenterJustify,
                             eFullyJustify,
                             eDefaultForScriptJustify };

    /*
    @CLASS:         TextDirection
    @DESCRIPTION:   <p>There are two defined text directions:
        <ul>
            <li><em>eLeftToRight</em></li>
            <li><em>eRightToLeft</em></li>
        </ul>
        </p>
    */
    enum TextDirection { eLeftToRight,
                         eRightToLeft };

    /*
    @CLASS:         ListStyle
    @DESCRIPTION:   <p>Different styles of bullet / list markers for list items in the WordProcessor.</p>

            <p>Numeric values come from RTF 1.5 Spec \levelnfc (except for eListStyle_None which is special)</p>
    */
    enum ListStyle {
        eListStyle_None   = 9999,
        eListStyle_Bullet = 23
    };

    /*
    @CLASS:         Led_LineSpacing
    @DESCRIPTION:   <p>Support at least all the crazy formats/options in the Win32 PARAFORMAT2 structure, and
        the ill-documented RTF 1.5 SPEC \sl options.
            <ul>
                    <li>eSingleSpace-   fArg IGNORED
                    <li>eOnePointFiveSpace- fArg IGNORED
                    <li>eDoubleSpace-   fArg IGNORED
                    <li>eAtLeastTWIPSSpacing-<br>
                        The fArg member specifies the spacing from one line to the next, in twips.
                        However, if fArg specifies a value that is less than single line spacing, Led displays single-spaced text.
                    <li>eExactTWIPSSpacing-<br>
                        The fArg member specifies the spacing from one row to the next, in twips.
                        Led uses the exact spacing specified, even if fArg specifies a value that is less than single spacing.
                    <li>eExactLinesSpacing-<br>
                        The value of fArg / 20 is the spacing, in lines, from one line to the next.
                        Thus, setting fArg to 20 produces single-spaced text, 40 is double spaced, 60 is triple spaced, and so on.
            </ul>
            </p>
    */
    class Led_LineSpacing {
    public:
        enum Rule {
            eSingleSpace,
            eOnePointFiveSpace,
            eDoubleSpace,
            eAtLeastTWIPSSpacing,
            eExactTWIPSSpacing,
            eExactLinesSpacing
        };
        Rule     fRule;
        unsigned fArg;

    public:
        Led_LineSpacing ()
            : fRule (eSingleSpace)
            , fArg (0)
        {
        }
        Led_LineSpacing (Rule rule)
            : fRule (rule)
            , fArg (0)
        {
            Require (rule == eSingleSpace or rule == eOnePointFiveSpace or rule == eDoubleSpace);
        }
        Led_LineSpacing (Rule rule, Led_TWIPS twips)
            : fRule (rule)
            , fArg (twips)
        {
            Require (rule == eAtLeastTWIPSSpacing or rule == eExactTWIPSSpacing);
        }
        Led_LineSpacing (Rule rule, unsigned lineCount)
            : fRule (rule)
            , fArg (lineCount)
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
    };
    inline bool operator== (Led_LineSpacing lhs, Led_LineSpacing rhs)
    {
        if (lhs.fRule != rhs.fRule) {
            return false;
        }
        if (lhs.fRule == Led_LineSpacing::eAtLeastTWIPSSpacing or lhs.fRule == Led_LineSpacing::eExactTWIPSSpacing or lhs.fRule == Led_LineSpacing::eExactLinesSpacing) {
            if (lhs.fArg != rhs.fArg) {
                return false;
            }
        }
        return true;
    }
    inline bool operator!= (Led_LineSpacing lhs, Led_LineSpacing rhs)
    {
        return not(lhs == rhs);
    }

    class Led_IncrementalFontSpecification;
    /*
    @CLASS:         Led_FontSpecification
    @DESCRIPTION:   <p><code>Led_FontSpecification</code> is a utility class which portably represents
        a user font choice. This largely corresponds to the MS-Windows <code>LOGFONT</code> structure
        or the Macintosh <code>txFace, txSize, txStyle</code>.</p>
            <p>In addition to being a portable represenation of this information, it
        also contains handy wrapper accessors, and extra information like subscript,
        superscript, and font color.</p>
            <p>See also, @'Led_IncrementalFontSpecification'</p>
    */
    class Led_FontSpecification {
    public:
        Led_FontSpecification ();
#if qPlatform_Windows
        explicit Led_FontSpecification (const LOGFONT& logFont);
#endif

        // Force users to be EXPLICIT about this object-slicing, since many of the fields
        // maybe invalid... Will the compiler REALLY do this check???? We'll see - LGP 970314
    public:
        explicit Led_FontSpecification (const Led_IncrementalFontSpecification& from);

    public:
#if qPlatform_MacOS
        using FontNameSpecifier = short;
#elif qPlatform_Windows
        struct FontNameSpecifier { // So struct copies etc will work and so we can define op==
            FontNameSpecifier ();
            FontNameSpecifier (const Led_SDK_Char* from);
            Led_SDK_Char fName[LF_FACESIZE];
        };
#elif qStroika_FeatureSupported_XWindows
        using FontNameSpecifier = Led_SDK_String;
#endif

    public:
        // string/name wrapper. Trivial for PC, and for Mac - converts between name under NUMBER ID
        nonvirtual Led_SDK_String GetFontName () const;
        nonvirtual void           SetFontName (const Led_SDK_String& fontName);

        // FontName info
        nonvirtual FontNameSpecifier GetFontNameSpecifier () const;
        nonvirtual void              SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier);

        // Style info
        nonvirtual bool GetStyle_Plain () const; // special composite of all other styles...
        nonvirtual void SetStyle_Plain ();

        nonvirtual bool GetStyle_Bold () const;
        nonvirtual void SetStyle_Bold (bool isBold);

        nonvirtual bool GetStyle_Italic () const;
        nonvirtual void SetStyle_Italic (bool isItalic);

        nonvirtual bool GetStyle_Underline () const;
        nonvirtual void SetStyle_Underline (bool isUnderline);

        enum SubOrSuperScript { eSubscript,
                                eSuperscript,
                                eNoSubOrSuperscript };
        nonvirtual SubOrSuperScript GetStyle_SubOrSuperScript () const;
        nonvirtual void             SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript);

#if qPlatform_MacOS
        nonvirtual bool GetStyle_Outline () const;
        nonvirtual void SetStyle_Outline (bool isOutline);

        nonvirtual bool GetStyle_Shadow () const;
        nonvirtual void SetStyle_Shadow (bool isShadow);

        nonvirtual bool GetStyle_Condensed () const;
        nonvirtual void SetStyle_Condensed (bool isCondensed);

        nonvirtual bool GetStyle_Extended () const;
        nonvirtual void SetStyle_Extended (bool isExtended);
#elif qPlatform_Windows
        nonvirtual bool GetStyle_Strikeout () const;
        nonvirtual void SetStyle_Strikeout (bool isStrikeout);
#endif

        using FontSize = uint16_t;
        nonvirtual FontSize GetPointSize () const;
        nonvirtual void     SetPointSize (FontSize pointSize);

#if qPlatform_Windows
        nonvirtual long PeekAtTMHeight () const;        // Speed tweek
        nonvirtual void PokeAtTMHeight (long tmHeight); // ditto
#endif

        nonvirtual Led_Color GetTextColor () const;
        nonvirtual void      SetTextColor (const Led_Color& textColor);

    public:
#if qPlatform_MacOS
        nonvirtual void GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const;
        nonvirtual void SetOSRep (short fontID, short fontSize, Style fontStyle);
#elif qPlatform_Windows
        nonvirtual LOGFONT GetOSRep () const;
        nonvirtual void    GetOSRep (LOGFONT* logFont) const;
        nonvirtual void    SetOSRep (LOGFONT logFont);
        nonvirtual void    LightSetOSRep (LOGFONT logFont);
#elif qStroika_FeatureSupported_XWindows
        static string mkOSRep (const string& foundry, const string& family, const string& weight, const string& slant, const string& pointSize);
        nonvirtual string GetOSRep () const;
        nonvirtual void   SetFromOSRep (const string& osRep);
#endif

    public:
        nonvirtual void MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes);

        // OSRep.
    private:
#if qPlatform_MacOS
        short fFontSpecifier;
        short fFontSize;
        Style fFontStyle;
#elif qPlatform_Windows
        LOGFONT            fFontInfo; // Could make this MUCH smaller on windows - do for future release!
#elif qStroika_FeatureSupported_XWindows
        FontNameSpecifier fFontFamily;
        bool              fBold : 1;
        bool              fItalics : 1;
        bool              fUnderline : 1;
        short             fFontSize;
#endif
        SubOrSuperScript fSubOrSuperScript;
        Led_Color        fTextColor;
    };
#if qPlatform_Windows
    bool operator== (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs);
    bool operator!= (const Led_FontSpecification::FontNameSpecifier& lhs, const Led_FontSpecification::FontNameSpecifier& rhs);
#endif

    bool operator== (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs);
    bool operator!= (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs);

    /*
    @CLASS:         Led_IncrementalFontSpecification
    @DESCRIPTION:   <p><code>Led_IncrementalFontSpecification</code> is a simple subclass of
        @'Led_FontSpecification' which adds a bool flag for each
        font attribute indicating whether or not it is really specified.
        With this, and the @'Led_FontSpecification::MergeIn' method,
        you can specify just one or two changes to a font record, pass them around,
        and apply them to an existing font choice.</p>
            <p>See also, @'Led_FontSpecification'</p>
    */
    class Led_IncrementalFontSpecification : public Led_FontSpecification {
    private:
        using inherited = Led_FontSpecification;

    public:
        Led_IncrementalFontSpecification ();

        // I may end up regretting this, for all the confusion it
        // can cause, but it sure makes a number of things simpler
        // and clearer-- LGP 960520
        Led_IncrementalFontSpecification (const Led_FontSpecification& fontSpec);

        /*
         *  Basic form of this API. For a feature of a font, say its size, there is a
         *  GetFEATURE () -
         *      REQUIRES FEATURE VALID
         *  GetFEATURE (bool* valid, FEATUURE* feature)-
         *      Either valid or feature can be nullptr, but if NON-nullptr filled in with
         *      feature/valid (except feature NOT filled in if not valid.
         *  InvalidateFEATURE ()-
         *      Just sets valid-flag false.
         *  SetFEATURE(FEATURE)-
         *      Specifies this as the feature value, and sets the valid flag TRUE.
         *
         *  (NOTE: MAYBE simplify this with member templates if/when that supported by compilers? - LGP 970314)
         */
    public:
        nonvirtual FontNameSpecifier GetFontNameSpecifier () const;
        nonvirtual bool              GetFontNameSpecifier_Valid () const;
        nonvirtual void              InvalidateFontNameSpecifier ();
        nonvirtual void              SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier);
        nonvirtual void              SetFontName (const Led_SDK_String& fontName);

        // Style info
        nonvirtual bool GetStyle_Plain () const; // special composite of all other styles...
        nonvirtual bool GetStyle_Plain_Valid () const;
        nonvirtual void InvalidateStyles ();
        nonvirtual void SetStyle_Plain ();

        nonvirtual bool GetStyle_Bold () const;
        nonvirtual bool GetStyle_Bold_Valid () const;
        nonvirtual void InvalidateStyle_Bold ();
        nonvirtual void SetStyle_Bold (bool isBold);

        nonvirtual bool GetStyle_Italic () const;
        nonvirtual bool GetStyle_Italic_Valid () const;
        nonvirtual void InvalidateStyle_Italic ();
        nonvirtual void SetStyle_Italic (bool isItalic);

        nonvirtual bool GetStyle_Underline () const;
        nonvirtual bool GetStyle_Underline_Valid () const;
        nonvirtual void InvalidateStyle_Underline ();
        nonvirtual void SetStyle_Underline (bool isUnderline);

        nonvirtual SubOrSuperScript GetStyle_SubOrSuperScript () const;
        nonvirtual bool             GetStyle_SubOrSuperScript_Valid () const;
        nonvirtual void             InvalidateStyle_SubOrSuperScript ();
        nonvirtual void             SetStyle_SubOrSuperScript (SubOrSuperScript subOrSuperScript);

#if qPlatform_MacOS
        nonvirtual bool GetStyle_Outline () const;
        nonvirtual bool GetStyle_Outline_Valid () const;
        nonvirtual void InvalidateStyle_Outline ();
        nonvirtual void SetStyle_Outline (bool isOutline);

        nonvirtual bool GetStyle_Shadow () const;
        nonvirtual bool GetStyle_Shadow_Valid () const;
        nonvirtual void InvalidateStyle_Shadow ();
        nonvirtual void SetStyle_Shadow (bool isShadow);

        nonvirtual bool GetStyle_Condensed () const;
        nonvirtual bool GetStyle_Condensed_Valid () const;
        nonvirtual void InvalidateStyle_Condensed ();
        nonvirtual void SetStyle_Condensed (bool isCondensed);

        nonvirtual bool GetStyle_Extended () const;
        nonvirtual bool GetStyle_Extended_Valid () const;
        nonvirtual void InvalidateStyle_Extended ();
        nonvirtual void SetStyle_Extended (bool isExtended);
#elif qPlatform_Windows
        nonvirtual bool    GetStyle_Strikeout () const;
        nonvirtual bool    GetStyle_Strikeout_Valid () const;
        nonvirtual void    InvalidateStyle_Strikeout ();
        nonvirtual void    SetStyle_Strikeout (bool isStrikeout);
#endif

        /*
            *  Note the closely related nature of PointSize and PointSizeIncement.
            *  If one is set, that automatically unsets the other. You cannot set both
            *  at the same time. The former is used to specify the point size exactly.
            *  The latter is used to specify an ajdustment to the pointsize.
            */
        nonvirtual unsigned short GetPointSize () const;
        nonvirtual bool           GetPointSize_Valid () const;
        nonvirtual void           InvalidatePointSize ();
        nonvirtual void           SetPointSize (FontSize pointSize);
#if qPlatform_Windows
        nonvirtual void PokeAtTMHeight (long tmHeight); // ditto
#endif

        nonvirtual short GetPointSizeIncrement () const;
        nonvirtual bool  GetPointSizeIncrement_Valid () const;
        nonvirtual void  InvalidatePointSizeIncrement ();
        nonvirtual void  SetPointSizeIncrement (short pointSizeIncrement);

        nonvirtual Led_Color GetTextColor () const;
        nonvirtual bool      GetTextColor_Valid () const;
        nonvirtual void      InvalidateTextColor ();
        nonvirtual void      SetTextColor (const Led_Color& textColor);

    public:
#if qPlatform_MacOS
        nonvirtual void GetOSRep (short* fontID, short* fontSize, Style* fontStyle) const;
        nonvirtual void SetOSRep (short fontID, short fontSize, Style fontStyle); // marks all attribs as valid
#elif qPlatform_Windows
        nonvirtual LOGFONT GetOSRep () const;
        nonvirtual void    GetOSRep (LOGFONT* logFont) const;
        nonvirtual void    SetOSRep (LOGFONT logFont); // marks all attribs as valid
        nonvirtual void    LightSetOSRep (LOGFONT logFont);
        nonvirtual bool    GetDidSetOSRepCallFlag () const; // special flag indicating user did SetOSRep() cuz that
                                                            // sets things valid that we don't have special 'valid' flags
                                                            // for. Makes MergeIn() just copy flat out!
#endif

    public:
        nonvirtual void MergeIn (const Led_IncrementalFontSpecification& addInTheseAttributes);

    private:
        bool fFontSpecifierValid : 1;
        bool fStyleValid_Bold : 1;
        bool fStyleValid_Italic : 1;
        bool fStyleValid_Underline : 1;
        bool fStyleValid_SubOrSuperScript : 1;
#if qPlatform_MacOS
        bool fStyleValid_Outline : 1;
        bool fStyleValid_Shadow : 1;
        bool fStyleValid_Condensed : 1;
        bool fStyleValid_Extended : 1;
#elif qPlatform_Windows
        bool fStyleValid_Strikeout : 1;
        bool fDidSetOSRepCallFlag : 1;
#endif
        bool fFontSizeValid : 1;
        bool fFontSizeIncrementValid : 1;
        bool fTextColorValid : 1;
    };

    bool operator== (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);
    bool operator!= (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);

    Led_IncrementalFontSpecification Intersection (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs);

    // Must support for other Platforms - but not done yet... Also - should have OPTIONS specified to this class (CTOR) picking various filter
    // options...
    class Led_InstalledFonts {
    public:
        enum FilterOptions {
#if qPlatform_Windows
            eSkipAtSignFonts = 0x1,
            eSkipRasterFonts = 0x2,
#endif

#if qPlatform_Windows
            eDefaultFilterOptions = eSkipAtSignFonts | eSkipRasterFonts
#else
            eDefaultFilterOptions = 0
#endif
        };
        Led_InstalledFonts (
#if qStroika_FeatureSupported_XWindows
            Display* display,
#endif
            FilterOptions filterOptions = eDefaultFilterOptions);

    public:
        nonvirtual const vector<Led_SDK_String>& GetUsableFontNames () const;

    private:
        FilterOptions          fFilterOptions;
        vector<Led_SDK_String> fFontNames;

#if qPlatform_Windows
    private:
        static BOOL FAR PASCAL FontFamilyAdderProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int fontType, LPVOID pThis);
#endif
    };

    /*
    @CLASS:         Led_GDIGlobals
    @DESCRIPTION:   <p>Something of a hack version of GDI global variables. We want to keep certain GDI global variables
        computed ONCE, for speed reasons. And yet - if we get a certain windows message, we must refresh our cached global
        variables. The compromise is that all these globals are associated with this class, so that there is one place to
        call to refresh those globals.</p>
    */
    class Led_GDIGlobals {
    public:
        Led_GDIGlobals ();

    public:
        static Led_GDIGlobals& Get ();

    private:
        static Led_GDIGlobals* sThe;

    public:
        nonvirtual void InvalidateGlobals ();

    public:
        nonvirtual Led_Distance GetMainScreenLogPixelsH () const;
        nonvirtual Led_Distance GetMainScreenLogPixelsV () const;

    public:
        Led_Distance fLogPixelsH;
        Led_Distance fLogPixelsV;

    private:
        class _Global_DESTRUCTOR_;
        friend class _Global_DESTRUCTOR_;
    };

    Led_Rect operator- (const Led_Rect& lhs, const Led_Point& rhs);
    Led_Rect operator+ (Led_Point p, Led_Rect r);
    Led_Rect operator+ (Led_Rect r, Led_Point p);
    bool     Intersect (const Led_Rect& lhs, const Led_Rect& rhs);
    bool     Intersect (const Led_Rect& lhs, const Led_Region& rhs);
    bool     Intersect (const Led_Region& lhs, const Led_Rect& rhs);
    bool     Intersect (const Led_Region& lhs, const Led_Region& rhs);

    Led_Rect Intersection (const Led_Rect& lhs, const Led_Rect& rhs);
    Led_Rect operator* (const Led_Rect& lhs, const Led_Rect& rhs);

    Led_Size operator+ (Led_Size lhs, Led_Size rhs);
    Led_Size operator* (int lhs, Led_Size rhs);

    Led_Rect InsetRect (const Led_Rect& r, int vBy, int hBy);
    Led_Rect EnsureRectInRect (const Led_Rect& r, Led_Rect enlosingR);
    Led_Rect EnsureRectOnScreen (const Led_Rect& r);

#if qPlatform_MacOS
    Led_Point AsLedPoint (Point p);
    Point     AsQDPoint (Led_Point p);
    Led_Rect  AsLedRect (Rect r);
    Rect      AsQDRect (Led_Rect r);
    Led_Size  AsLedSize (Point s);
    Point     GetRectOrigin (const Rect& r);
    Point     GetRectSize (const Rect& r);
    short     GetRectHeight (const Rect& r);
    short     GetRectWidth (const Rect& r);
#elif qPlatform_Windows
    Led_Point AsLedPoint (POINT p);
    POINT     AsPOINT (Led_Point p);
    Led_Rect  AsLedRect (RECT r);
    RECT      AsRECT (Led_Rect p);
    SIZE      AsSIZE (Led_Size s);
    Led_Size  AsLedSize (SIZE s);
#elif qStroika_FeatureSupported_XWindows
    Led_Rect AsLedRect (const XRectangle& r);
    XRectangle AsXRect (const Led_Rect& r);
#endif

    Led_TWIPS      Led_CvtScreenPixelsToTWIPSV (Led_Coordinate from);
    Led_TWIPS      Led_CvtScreenPixelsToTWIPSH (Led_Coordinate from);
    Led_Coordinate Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS from);
    Led_Coordinate Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS from);

    /*
    @CLASS:         Led_FontMetrics
    @DESCRIPTION:   <p><code>Led_FontMetrics</code> is a portable wrapper class on the Macintosh
        <em>FontInfo</em> structure, or the Windows <em>TEXTMETRIC</em> structure. It provides
        portable access to things like GetLineHeight (), and GetAscent (), etc...</p>
    */
    class Led_FontMetrics {
#if qStroika_FeatureSupported_XWindows
    public:
        struct PlatformSpecific {
            Led_Distance fAscent;
            Led_Distance fDescent;
            Led_Distance fLeading;
            Led_Distance fMaxCharWidth;
        };
#endif
    public:
        Led_FontMetrics ();
#if qPlatform_MacOS
        Led_FontMetrics (const FontInfo& from);
#elif qPlatform_Windows
        Led_FontMetrics (const TEXTMETRIC& from);
#elif qStroika_FeatureSupported_XWindows
        Led_FontMetrics (const PlatformSpecific& from);
#endif
        Led_FontMetrics (const Led_FontMetrics& from);
        const Led_FontMetrics& operator= (const Led_FontMetrics& rhs);

    public:
        nonvirtual Led_Distance GetAscent () const;
        nonvirtual Led_Distance GetDescent () const;
        nonvirtual Led_Distance GetLeading () const;
        nonvirtual Led_Distance GetHeight () const;     // does NOT include leading
        nonvirtual Led_Distance GetLineHeight () const; // includes leading

    public:
        nonvirtual Led_Distance GetMaxCharacterWidth () const;
#if qPlatform_Windows
        nonvirtual Led_Distance GetAveCharacterWidth () const;
#endif

        // Convertion operator to make it easier to make GDI calls with one of our guys on a
        // with something expected a system specific one - like to fill in its value!
    public:
#if qPlatform_MacOS
        operator const FontInfo* () const;
        operator FontInfo* ();
#elif qPlatform_Windows
        operator const TEXTMETRIC* () const;
        operator TEXTMETRIC* ();
#endif

    private:
#if qPlatform_MacOS
        FontInfo fPlatformSpecific;
#elif qPlatform_Windows
        TEXTMETRIC fPlatformSpecific;
#elif qStroika_FeatureSupported_XWindows
        PlatformSpecific fPlatformSpecific;
#endif
    };

    Led_Color Led_GetTextColor ();
    Led_Color Led_GetTextBackgroundColor ();
    Led_Color Led_GetSelectedTextColor ();
    Led_Color Led_GetSelectedTextBackgroundColor ();

    class OffscreenTablet;

    /*
    @CLASS:         Led_Tablet_
    @DESCRIPTION:   <p>See also @'Led_Tablet' - since that is what Led tends to make use of directly.</p>
            <p>This class is used to wrap a low level graphics drawing device. On Windows - this is an HDC.
        On the Mac - a GrafPtr (also CGrafPtr and GWorldPtr). On X-Windows - a drawable and display, and GC.</p>
            <p>This class right now is a very thin wrapper on those drawing prodedures (mostly for backward compatability reasons.
        Eventually - it may do a better job of wrapping those concepts/APIs genericly.</p>
    */
    class Led_Tablet_ {
    public:
#if qPlatform_MacOS
        Led_Tablet_ (GrafPtr gp);
#elif qPlatform_Windows
        enum OwnDCControl { eOwnsDC,
                            eDoesntOwnDC };

        Led_Tablet_ (HDC hdc = nullptr, OwnDCControl ownsDC = eOwnsDC);
#elif qStroika_FeatureSupported_XWindows
        Led_Tablet_ (Display* display, Drawable drawable);
#endif

    public:
        virtual ~Led_Tablet_ ();

    public:
#if qPlatform_MacOS
        nonvirtual operator GrafPtr () const;
#elif qPlatform_Windows
        nonvirtual operator HDC () const;
#endif

    public:
        nonvirtual Led_Coordinate CvtFromTWIPSV (Led_TWIPS from) const;
        nonvirtual Led_Coordinate CvtFromTWIPSH (Led_TWIPS from) const;
        nonvirtual Led_TWIPS CvtToTWIPSV (Led_Coordinate from) const;
        nonvirtual Led_TWIPS CvtToTWIPSH (Led_Coordinate from) const;

    public:
        nonvirtual Led_Point CvtFromTWIPS (Led_TWIPS_Point from) const;
        nonvirtual Led_TWIPS_Point CvtToTWIPS (Led_Point from) const;
        nonvirtual Led_Rect CvtFromTWIPS (Led_TWIPS_Rect from) const;
        nonvirtual Led_TWIPS_Rect CvtToTWIPS (Led_Rect from) const;

    public:
        nonvirtual void ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, Led_Coordinate scrollBy);
        nonvirtual void FrameRegion (const Led_Region& r, const Led_Color& c);

    public:
        nonvirtual void FrameRectangle (const Led_Rect& r, Led_Color c, Led_Distance borderWidth);

    public:
#if qPlatform_MacOS
        nonvirtual void SetPort ();
#elif qPlatform_Windows
        nonvirtual BOOL BitBlt (int x, int y, int nWidth, int nHeight, Led_Tablet_* pSrcDC, int xSrc, int ySrc, DWORD dwRop);
        nonvirtual BOOL CreateCompatibleDC (Led_Tablet_* pDC);
        nonvirtual COLORREF SetTextColor (COLORREF crColor);
        nonvirtual COLORREF SetBkColor (COLORREF crColor);
        nonvirtual BOOL IsPrinting () const;
        nonvirtual BOOL RoundRect (int x1, int y1, int x2, int y2, int x3, int y3);
        nonvirtual BOOL TextOut (int x, int y, LPCTSTR lpszString, int nCount);
        //      nonvirtual  SIZE    GetTextExtent (LPCTSTR lpszString, int nCount) const;
        nonvirtual int SetBkMode (int nBkMode);
        nonvirtual SIZE GetWindowExt () const;
        nonvirtual SIZE GetViewportExt () const;
        nonvirtual BOOL Rectangle (int x1, int y1, int x2, int y2);
        nonvirtual BOOL Rectangle (const RECT& r);
        nonvirtual BOOL Rectangle (LPCRECT lpRect);
        nonvirtual BOOL GetTextMetrics (LPTEXTMETRIC lpMetrics) const;
        nonvirtual HBITMAP SelectObject (HBITMAP hBitmap);
#if defined(STRICT)
        nonvirtual HFONT SelectObject (HFONT hFont);
#endif
        nonvirtual POINT SetWindowOrg (int x, int y);
        nonvirtual int   GetDeviceCaps (int nIndex) const;
        nonvirtual BOOL Attach (HDC hDC, OwnDCControl ownsDC = eOwnsDC);
        nonvirtual HDC Detach ();
#elif qStroika_FeatureSupported_XWindows
    public:
        nonvirtual void SetFont (const Led_FontSpecification& fontSpec);

    private:
        map<string, XFontStruct*> fFontCache;
        enum { kMaxFontCacheSize = 5 };

    public:
        nonvirtual void SetDrawableOrigin (const Led_Point& origin);

    private:
        Led_Point fDrawableOrigin;
#endif

#if qPlatform_Windows
    private:
        nonvirtual HWND         GetWindow () const;
        nonvirtual unsigned int SetTextAlign (unsigned int nTextAlign);
#endif

#if qStroika_FeatureSupported_XWindows
    public:
        static int IgnoreXErrorHandler (Display* display, XErrorEvent* error);
#endif

#if qStroika_FeatureSupported_XWindows
    private:
        nonvirtual Led_SDK_String BestMatchFont (const Led_FontSpecification& fsp, const vector<Led_SDK_String>& fontsList);

    public:
        static void ParseFontName (const Led_SDK_String& fontName, Led_SDK_String* familyName, Led_SDK_String* fontSize, Led_SDK_String* fontWeight, Led_SDK_String* fontSlant);
#endif

    public:
        nonvirtual void MoveTo (const Led_Point& to);
        nonvirtual void LineTo (const Led_Point& to);

#if qStroika_FeatureSupported_XWindows
    private:
        Led_Point fCurDrawLineLoc;
#endif

    public:
        nonvirtual void MeasureText (const Led_FontMetrics& precomputedFontMetrics,
                                     const Led_tChar* text, size_t nTChars, Led_Distance* charLocations);
        nonvirtual void TabbedTextOut (const Led_FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
                                       TextDirection direction,
                                       Led_Point outputAt, Led_Coordinate hTabOrigin, const Led_TabStopList& tabStopList,
                                       Led_Distance* amountDrawn, Led_Coordinate hScrollOffset);

    public:
        nonvirtual void SetBackColor (const Led_Color& backColor);
        nonvirtual void SetForeColor (const Led_Color& foreColor);

    public:
        nonvirtual void EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Led_Color& eraseColor);
        nonvirtual void HilightArea_SolidHelper (const Led_Rect& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);
        nonvirtual void HilightArea_SolidHelper (const Led_Region& hilightArea, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);

#if qPlatform_Windows
    private:
        class RecolorHelper;

    private:
        RecolorHelper* fRecolorHelper;
#endif

    public:
        class ClipNarrowAndRestore;

    public:
        nonvirtual Led_Region GetClip () const;
        nonvirtual bool       GetClip (Led_Region* r) const;
        nonvirtual void       SetClip ();
        nonvirtual void       SetClip (const Led_Rect& clipTo);
        nonvirtual void       SetClip (const Led_Region& clipTo);

    public:
        nonvirtual Led_FontMetrics GetFontMetrics () const;

#if qPlatform_MacOS
    private:
        GrafPtr fGrafPort;
#elif qPlatform_Windows
    public:
        HDC          m_hDC;       // The output DC (must be first data member)
        HDC          m_hAttribDC; // The Attribute DC
        BOOL         m_bPrinting;
        OwnDCControl fOwnsDC;

    private:
        mutable Led_Distance fLogPixelsV;
        mutable Led_Distance fLogPixelsH;
#elif qStroika_FeatureSupported_XWindows
    private:
        Display* fDisplay;
        Drawable fDrawable;
        GC fGC;
        Colormap fColormap;
        mutable XFontStruct* fCachedFontInfo;
        map<string, string> fFontMappingCache;
#endif

    public:
        friend class OffscreenTablet;
    };

    /*
    @CLASS:         Led_Tablet_::ClipNarrowAndRestore
    @DESCRIPTION:   <p>Further narrow the existing clip region in the given tablet to the constructor. Then
                restore the clip region for the tablet to what it was when the contructor was called.</p>
    */
    class Led_Tablet_::ClipNarrowAndRestore {
    public:
        ClipNarrowAndRestore (Led_Tablet_* tablet);
        ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Rect& clipFurtherTo);
        ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Region& clipFurtherTo);
        ~ClipNarrowAndRestore ();

    private:
        Led_Tablet_* fTablet;
        bool         fHasOldClip;
        Led_Region   fOldClip;
    };

    /*
    @CLASS:         Led_Tablet
    @DESCRIPTION:   <p>A pointer to a @'Led_Tablet_' structure. These pointers are used throughout Led.</p>
        <p>In versions of Led prior to Led 3.0 - this typedef refered directly to a Mac GrafPort or MFC CDC.</p>
        <p>Now it refers to a structure which wraps those lower level concepts (and doesn't depend on MFC anymore).</p>
    */
    using Led_Tablet = Led_Tablet_*;

#if qPlatform_Windows
    class Led_WindowDC : public Led_Tablet_ {
    public:
        Led_WindowDC (HWND hWnd)
            : m_hWnd (hWnd)
        {
            Require (m_hWnd == nullptr or ::IsWindow (m_hWnd));
            if (!Attach (::GetWindowDC (m_hWnd))) {
                Led_ThrowOutOfMemoryException ();
            }
        }

        ~Led_WindowDC ()
        {
            AssertNotNull (m_hDC);
            ::ReleaseDC (m_hWnd, Detach ());
        }

    private:
        HWND m_hWnd;
    };
#endif

#if qPlatform_MacOS
    class Led_MacPortAndClipRegionEtcSaver {
    public:
        Led_MacPortAndClipRegionEtcSaver ();
        ~Led_MacPortAndClipRegionEtcSaver ();

    private:
        GrafPtr   fSavedPort;
        short     fOldLeft;
        short     fOldTop;
        RGBColor  fRGBFgColor;
        RGBColor  fRGBBkColor;
        RgnHandle fOldClip;
    };
#endif

#if qPlatform_Windows
    class Led_Bitmap {
    public:
        Led_Bitmap ();
        ~Led_Bitmap ();

    public:
        nonvirtual void DeleteObject ();
        nonvirtual      operator HBITMAP () const;

    public:
        nonvirtual Led_Size GetImageSize () const;
        nonvirtual BOOL CreateCompatibleBitmap (HDC hdc, Led_Distance nWidth, Led_Distance nHeight);
        nonvirtual BOOL CreateCompatibleDIBSection (HDC hdc, Led_Distance nWidth, Led_Distance nHeight);
        nonvirtual void LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName);

    private:
        HBITMAP  m_hObject;
        Led_Size fImageSize;
    };
#endif

    /*
    @CLASS:         OffscreenTablet
    @DESCRIPTION:   <p>An offscreen tablet is a helper object used to do offscreen imaging. This is useful in
                avoidance of flicker. Also, by encapsulating this procedure into a class, it becomes easier
                to add the functionality to several places in Led, yet with very different underlying implementations
                on each platform.</p>
    */
    class OffscreenTablet {
    public:
        OffscreenTablet ();
        ~OffscreenTablet ();

    public:
        nonvirtual void Setup (Led_Tablet origTablet);
        nonvirtual Led_Tablet PrepareRect (const Led_Rect& currentRowRect, Led_Distance extraToAddToBottomOfRect = 0);
        nonvirtual void       BlastBitmapToOrigTablet ();

    private:
        class OT : public Led_Tablet_ {
        private:
            using inherited = Led_Tablet_;

        public:
#if qPlatform_MacOS
            OT (GrafPtr gp);
#elif qPlatform_Windows
            OT (HDC hdc = nullptr, OwnDCControl ownsDC = eOwnsDC);
#elif qStroika_FeatureSupported_XWindows
            OT (Display* display, Drawable drawable);
#endif
        };

    private:
        Led_Tablet fOrigTablet;
        Led_Rect   fOffscreenRect;
        Led_Tablet fOffscreenTablet;
#if qPlatform_MacOS
        GDHandle  fOrigDevice;
        CGrafPtr  fOrigPort;
        GWorldPtr fOffscreenGWorld;
#elif qPlatform_Windows
        OT         fMemDC;
        Led_Bitmap fMemoryBitmap; // only can create / select inside loop cuz there is where we know the size.
        // but decare outside, so stays around for successive rows which are the same size.
        HBITMAP fOldBitmapInDC; // used for save/restore of bitmap associated with the DC.
#elif qStroika_FeatureSupported_XWindows
        Drawable fPixmap;
#endif
    };

    /*
    @CLASS:         Led_GDI_Obj_Selector
    @DESCRIPTION:   <p><code>Led_GDI_Obj_Selector</code> is a stack-based class designed to help
        out selecting objects into a Led_Tablet (windows DC, grafport, etc).
            <p>The constructor takes a tablet, and object to select into it (HGDIObject, etc),
        and selects it into the tablet. It saves gthe results of the SelectObject calls (old values).
        And on its destructor, it restores the old values.</p>
            <p>This is useful when you want to make sure that an object you've selected into a tablet (HDC)
        will be released, and restored to its original state through all paths through
        the code, including in the even of exceptions.</p>
    */
    class Led_GDI_Obj_Selector {
    public:
#if qPlatform_Windows
        Led_GDI_Obj_Selector (Led_Tablet tablet, HGDIOBJ objToSelect);
#elif qPlatform_MacOS || qStroika_FeatureSupported_XWindows
        Led_GDI_Obj_Selector (Led_Tablet tablet, const Led_Pen& pen);
#endif
    public:
        ~Led_GDI_Obj_Selector ();

    private:
        Led_Tablet fTablet;
#if qPlatform_Windows
        HGDIOBJ fRestoreObject;
        HGDIOBJ fRestoreAttribObject;
#elif qPlatform_MacOS
        Led_Pen fRestorePen;
#endif
    };

#if qPlatform_Windows
    /*
    @CLASS:         Led_Win_Obj_Selector
    @DESCRIPTION:   <p>Alias for newer @'Led_GDI_Obj_Selector'.</p>
    */
    using Led_Win_Obj_Selector = Led_GDI_Obj_Selector;
#endif

/*
        *  Trap Caching support
        *
        *      This is a groty hack - but can be quite a big speed improvment for
        *  the Mac (at least 68K - I've never tried on PowerPC). For now
        *  (and perhaps always) we only support this for the 68K code.
        */
#if qPlatform_MacOS
    void     GDI_RGBForeColor (const RGBColor& color);
    void     GDI_RGBBackColor (const RGBColor& color);
    RGBColor GDI_GetForeColor ();
    RGBColor GDI_GetBackColor ();
#endif

// Even for windows we have have this defined if we build including QuickTime support!
#ifndef qHaveMacPictureDefined
#define qHaveMacPictureDefined qPlatform_MacOS
#endif

/*
    @CLASS:         Led_Picture
    @DESCRIPTION:   <p><code>Led_Picture</code> is a portable abstraction of a Macintosh Picture object.
        It can be displayed both on windows, and on the Mac (on Windows, it is only displayed if Apples
        QuickTime is installed). There are a bunch of routines (e.g Led_GetMacPictTop) which portable
        allow access to the size of the picture (even on windows if QT not available). And there are
        portable routines to draw the picture (again, with the windows QT caveat).</p>
    */
#if qHaveMacPictureDefined
    using Led_Picture = Picture;
#else
    struct Led_Picture {
        short picSize;
        short picFrameTop;
        short picFrameLeft;
        short picFrameBottom;
        short picFrameRight;
        // other data off end (specified by picSize) - note byte order for shorts is MAC
    };
#endif

    short    Led_GetMacPictTop (const Led_Picture* picture);
    short    Led_GetMacPictLeft (const Led_Picture* picture);
    short    Led_GetMacPictBottom (const Led_Picture* picture);
    short    Led_GetMacPictRight (const Led_Picture* picture);
    short    Led_GetMacPictWidth (const Led_Picture* picture);
    short    Led_GetMacPictHeight (const Led_Picture* picture);
    Led_Size Led_GetMacPictSize (const Led_Picture* picture);
#if qPlatform_MacOS
    short    Led_GetMacPictTop (const Led_Picture* const* picture);
    short    Led_GetMacPictLeft (const Led_Picture* const* picture);
    short    Led_GetMacPictBottom (const Led_Picture* const* picture);
    short    Led_GetMacPictRight (const Led_Picture* const* picture);
    short    Led_GetMacPictWidth (const Led_Picture* const* picture);
    short    Led_GetMacPictHeight (const Led_Picture* const* picture);
    Led_Size Led_GetMacPictSize (const Led_Picture* const* picture);
#endif

// Windows DIB support
#ifndef qHaveWindowsDIBDefined
#define qHaveWindowsDIBDefined qPlatform_Windows
#endif

#if !qHaveWindowsDIBDefined
    // structs copied (paraphrased) from MSVC 4.1 includes (WinGDI.h)
    struct BITMAPINFOHEADER {
        unsigned long  biSize;
        long           biWidth;
        long           biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned long  biCompression;
        unsigned long  biSizeImage;
        long           biXPelsPerMeter;
        long           biYPelsPerMeter;
        unsigned long  biClrUsed;
        unsigned long  biClrImportant;
    };
    struct BITMAPCOREHEADER {
        unsigned long  bcSize;
        unsigned short bcWidth;
        unsigned short bcHeight;
        unsigned short bcPlanes;
        unsigned short bcBitCount;
    };
    struct RGBQUAD {
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved;
    };
    struct RGBTRIPLE {
        unsigned char rgbtBlue;
        unsigned char rgbtGreen;
        unsigned char rgbtRed;
    };
    struct BITMAPINFO {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[1];
    };
#endif
    /*
    @CLASS:         Led_DIB
    @DESCRIPTION:   <p><em>Led_DIB</em> is a MS-Windows DIB (device independent bitmap) object. Much
        like @'Led_Picture', it has a host of accessor routines to get at its size, etc. But slightly
        better than Led_Picture, Led_DIB can be rendered on the Mac without assuming any special
        software (ie no analog to QT) is available. The downside is that I only support a few types
        of DIBs, but I appear to have most that windows currently generates. And it wouldn't be
        too hard to extend the code to support a few more types, if I knew what they were.</p>
    */
    using Led_DIB = BITMAPINFO;

    Led_Size    Led_GetDIBImageSize (const Led_DIB* dib);
    size_t      Led_GetDIBPalletByteCount (const Led_DIB* dib);
    size_t      Led_GetDIBImageRowByteCount (const Led_DIB* dib);
    size_t      Led_GetDIBImageByteCount (const Led_DIB* dib);
    Led_DIB*    Led_CloneDIB (const Led_DIB* dib);
    const void* Led_GetDIBBitsPointer (const Led_DIB* dib);

#if qPlatform_Windows
    Led_DIB* Led_DIBFromHBITMAP (HDC hDC, HBITMAP hbm);
#endif

    void AddRectangleToRegion (Led_Rect addRect, Led_Region* toRgn);

#if qProvideIMESupport
    class Led_IME {
    public:
        Led_IME ();

    public:
        static Led_IME& Get ();

    private:
        static Led_IME* sThe;

    private:
        nonvirtual bool Available () const; //tmphack - don't think this is ever used
    public:
        nonvirtual void ForgetPosition ();
        nonvirtual void NotifyPosition (HWND pWnd, const SHORT x, const SHORT y);
        nonvirtual void NotifyOfFontChange (HWND hWnd, const LOGFONT& lf);
        nonvirtual void IMEOn (HWND pWnd);
        nonvirtual void IMEOff (HWND pWnd);
        nonvirtual void Enable ();
        nonvirtual void Disable ();
        nonvirtual wstring GetCompositionResultStringW (HWND hWnd);

    private:
        nonvirtual void UpdatePosition (const HWND hWnd, const SHORT x, const SHORT y);
        nonvirtual void SendSimpleMessage (HWND pWnd, UINT fnc, WPARAM wParam);

        LRESULT (FAR PASCAL* fSendIMEMessageProc)
        (HWND, DWORD);
        BOOL (FAR PASCAL* fIMEEnableProc)
        (HWND, BOOL);
        HIMC (FAR PASCAL* fImmGetContext)
        (HWND);
        BOOL (FAR PASCAL* fImmSetCompositionFont)
        (HIMC, const LOGFONT*);
        BOOL (FAR PASCAL* fImmReleaseContext)
        (HWND, HIMC);
        LONG (FAR PASCAL* fImmGetCompositionStringW)
        (HIMC, DWORD, LPVOID, DWORD);
        BOOL (FAR PASCAL* fImmSetCompositionWindow)
        (HIMC, const void*);
        BOOL (FAR PASCAL* fImmSetOpenStatus)
        (HIMC, BOOL);

        bool  fWinNlsAvailable;
        SHORT fLastX;
        SHORT fLastY;

    private:
        class _Global_DESTRUCTOR_;
        friend class _Global_DESTRUCTOR_;
    };
#endif

    Led_Rect CenterRectInRect (const Led_Rect& r, const Led_Rect& centerIn);
#if qPlatform_Windows
    void Led_CenterWindowInParent (HWND w);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "GDI.inl"

#endif /*_Stroika_Frameworks_Led_GDI_h_*/
