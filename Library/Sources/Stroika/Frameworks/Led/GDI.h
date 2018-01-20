/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_GDI_h_
#define _Stroika_Frameworks_Led_GDI_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    LedGDI
@DESCRIPTION:
        <p>Graphix Library support. This code encapsulates access to underlying GDI for modularity sake.
    For historical reasons - as of Led 3.0 - there is still some direct access to underlying GDI in parts of Led
    but that will gradually (hopefully right after 3.0) be eliminated.</p>
        <p>Also - no class library dependencies (ie no MFC dependencies). So we can build MFC-Free Win32 apps more
    easily.</p>
 */

#if _MSC_VER == 1200
//A bit of a hack for MSVC60, cuz this needs to be done before including <vector> - otherwise we get
// lots of needless warnigns - regardless of what is done later -- LGP 980925
#pragma warning(4 : 4786)
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "Support.h"

// WHY DONT WE NEED TO INCLUDE <Windows.h> here??? - LGP 991213

#if qXWindows
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace Stroika {
    namespace Frameworks {
        namespace Led {

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
#if qXWindows
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
#elif qXWindows
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

                nonvirtual unsigned short GetPointSize () const;
                nonvirtual void           SetPointSize (unsigned short pointSize);
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
#elif qXWindows
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
#elif qXWindows
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
                nonvirtual void           SetPointSize (unsigned short pointSize);
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
#if qXWindows
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
#elif qXWindows
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
#if qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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

#if qXWindows
            public:
                static int IgnoreXErrorHandler (Display* display, XErrorEvent* error);
#endif

#if qXWindows
            private:
                nonvirtual Led_SDK_String BestMatchFont (const Led_FontSpecification& fsp, const vector<Led_SDK_String>& fontsList);

            public:
                static void ParseFontName (const Led_SDK_String& fontName, Led_SDK_String* familyName, Led_SDK_String* fontSize, Led_SDK_String* fontWeight, Led_SDK_String* fontSlant);
#endif

            public:
                nonvirtual void MoveTo (const Led_Point& to);
                nonvirtual void LineTo (const Led_Point& to);

#if qXWindows
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
#elif qXWindows
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
                <p>Now it refers to a structure which wraps those lower level concepts (and doesnt depend on MFC anymore).</p>
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
#elif qXWindows
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
#elif qXWindows
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
#elif qPlatform_MacOS || qXWindows
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

                short(FAR PASCAL* fSendIMEMessageProc) (HWND, DWORD);
                BOOL (FAR PASCAL* fIMEEnableProc)
                (HWND, BOOL);
                DWORD (FAR PASCAL* fImmGetContext)
                (HWND);
                BOOL (FAR PASCAL* fImmSetCompositionFont)
                (DWORD, const LOGFONT*);
                BOOL (FAR PASCAL* fImmReleaseContext)
                (HWND, DWORD);
                LONG (FAR PASCAL* fImmGetCompositionStringW)
                (DWORD, DWORD, LPVOID, DWORD);
                BOOL (FAR PASCAL* fImmSetCompositionWindow)
                (DWORD, const void*);
                BOOL (FAR PASCAL* fImmSetOpenStatus)
                (DWORD, BOOL);

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
            inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetTop (CoordinateType top)
            {
                this->top = top;
            }
            template <typename POINT_TYPE, typename SIZE_TYPE>
            inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetLeft (CoordinateType left)
            {
                this->left = left;
            }
            template <typename POINT_TYPE, typename SIZE_TYPE>
            inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetBottom (CoordinateType bottom)
            {
                this->bottom = bottom;
            }
            template <typename POINT_TYPE, typename SIZE_TYPE>
            inline void Rect_Base<POINT_TYPE, SIZE_TYPE>::SetRight (CoordinateType right)
            {
                this->right = right;
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
                static Led_Rect kWideOpened = Led_Rect (-10000, -10000, 20000, 20000);
                XRectangle xrectangle = AsXRect (kWideOpened);
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
#elif qXWindows
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

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4018)
#endif
                if ((winLeft + winWidth) > enlosingR.GetRight ()) {
                    winLeft = enlosingR.GetRight () - winWidth;
                }
                if ((winTop + winHeight) > enlosingR.GetBottom ()) {
                    winTop = enlosingR.GetBottom () - winHeight;
                }
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                return Led_Rect (winTop, winLeft, winHeight, winWidth);
            }
            /*
            @METHOD:        EnsureRectOnScreen
            @DESCRIPTION:   <p>Utility routine to ensure the first rect (typically used for a window) fits on the sceen.
                        Pin the edges so it fits. See also @'EnsureRectInRect'.</p>
            */
            inline Led_Rect EnsureRectOnScreen (Led_Rect& r)
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
                Led_Arg_Unused (r);
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
                return fPlatformSpecific.fAscent;
#endif
            }
            inline Led_Distance Led_FontMetrics::GetDescent () const
            {
#if qPlatform_MacOS
                return (fPlatformSpecific.descent);
#elif qPlatform_Windows
                return (fPlatformSpecific.tmDescent);
#elif qXWindows
                return fPlatformSpecific.fDescent;
#endif
            }
            inline Led_Distance Led_FontMetrics::GetLeading () const
            {
#if qPlatform_MacOS
                return (fPlatformSpecific.leading);
#elif qPlatform_Windows
                return (fPlatformSpecific.tmExternalLeading);
#elif qXWindows
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
#elif qXWindows
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
    }
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
namespace Stroika {
    namespace Frameworks {
        namespace Led {

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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
                fBold = isBold;
#endif
            }
            inline bool Led_FontSpecification::GetStyle_Italic () const
            {
#if qPlatform_MacOS
                return (fFontStyle & italic);
#elif qPlatform_Windows
                return !!fFontInfo.lfItalic;
#elif qXWindows
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
#elif qXWindows
                fItalics = isItalic;
#endif
            }
            inline bool Led_FontSpecification::GetStyle_Underline () const
            {
#if qPlatform_MacOS
                return (fFontStyle & underline);
#elif qPlatform_Windows
                return !!fFontInfo.lfUnderline;
#elif qXWindows
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
#elif qXWindows
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
            inline unsigned short Led_FontSpecification::GetPointSize () const
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
                    return ::MulDiv (-fFontInfo.lfHeight, 72, Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
                }
#elif qXWindows
                return fFontSize;
#endif
            }
            inline void Led_FontSpecification::SetPointSize (unsigned short pointSize)
            {
#if qPlatform_MacOS
                fFontSize = pointSize;
#elif qPlatform_Windows
                fFontInfo.lfHeight = ::MulDiv (-long(pointSize), Led_GDIGlobals::Get ().GetMainScreenLogPixelsV (), 72);
#elif qXWindows
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
#elif qXWindows
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
            inline void Led_IncrementalFontSpecification::SetPointSize (unsigned short pointSize)
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
#elif qXWindows
                return (Led_Color::kBlack);
#endif
            }
            inline Led_Color Led_GetTextBackgroundColor ()
            {
#if qPlatform_MacOS
                return (Led_Color::kWhite);
#elif qPlatform_Windows
                return Led_Color (::GetSysColor (COLOR_WINDOW));
#elif qXWindows
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
#elif qXWindows
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
#elif qXWindows
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
            inline Led_Tablet_::ClipNarrowAndRestore::ClipNarrowAndRestore (Led_Tablet_* tablet, const Led_Region& clipFurtherTo)
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
#elif qXWindows
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
            inline Led_IME& Led_IME::Get ()
            {
                if (sThe == nullptr) {
                    new Led_IME ();
                }
                AssertNotNull (sThe);
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
// This is OK because sThe is set in the Led_IME::CTOR
#pragma warning(suppress : 6011)
#endif
                return (*sThe);
            }
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
    }
}

#endif /*_Stroika_Frameworks_Led_GDI_h_*/
