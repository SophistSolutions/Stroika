/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_GDI_h_
#define _Stroika_Frameworks_Led_GDI_h_ 1

#include "../StroikaPreComp.h"

/*
 * Graphix Library support. This code encapsulates access to underlying GDI for modularity sake.
 *  For historical reasons - as of Led 3.0 - there is still some direct access to underlying GDI in parts of Led
 *  but that will gradually (hopefully right after 3.0) be eliminated.</p>
 *     Also - no class library dependencies (ie no MFC dependencies). So we can build MFC-Free Win32 apps more
 *  easily.
 */

#include <algorithm>
#include <cmath>
#include <compare>
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

    /**
     * <code>CoordinateType</code> is the <code>signed</code> analog of @'DistanceType'.
     *    @'Led_Point' is a tuple of <code>Coordinates</code>s
     *     (vertical and horizontal).</p>
     */
    using CoordinateType = long;

    /*
    @CLASS:         DistanceType
    @DESCRIPTION:   <p><code>DistanceType</code> is an unsigned type, specifying the distance (always non-negative)
        between two graphics locations. @'Led_Size' is a tuple of <code>DistanceType</code>s
        (vertical and horizontal).</p>
    */
    using DistanceType = unsigned long;

    /*
     *  Many distances are specified in Led in TWIPS - 1/20 of a printers point.
     * 
     *  This means - 1/1440 of an inch.</p>
     *
     *      This size refers to the size when printed on a page. So it can be scaled - depnding on screen resolution.
     *  Led will often save this internally - and scale it at the last minute to the resolution of the @'Tablet*' being printed on.
     *      
     *      NB: This marks a change from Led 2.3 and earlier - where most distances were stored in pixels (still many are).
     *      
     *      NB: declard as a class instead of a typedef so we get better type checking. Shouldn't affect sizes or code
     *  generation - I would hope!
     */
    class TWIPS {
    public:
        constexpr explicit TWIPS (long v);
        operator long () const;

    private:
        long fValue;

    public:
        nonvirtual TWIPS& operator+= (const TWIPS& rhs);
        nonvirtual TWIPS& operator-= (const TWIPS& rhs);
        nonvirtual TWIPS& operator*= (double rhs);

    public:
        static const TWIPS kPoint;
        static const TWIPS kInch;
        static const TWIPS kOneInch;
    };
    TWIPS operator+ (const TWIPS& lhs, const TWIPS& rhs);
    TWIPS operator- (const TWIPS& lhs, const TWIPS& rhs);

    inline constexpr TWIPS TWIPS::kPoint   = TWIPS{20}; // a printers 'point' (1/72 of an inch)
    inline constexpr TWIPS TWIPS::kInch    = TWIPS{1440};
    inline constexpr TWIPS TWIPS::kOneInch = TWIPS{1440};

    class Tablet;

    /*
     *      Even though WinSDK supports GetTabbedTextExtent/TabbedTextOut () - they do a bad job.
     *  In particular, they offer no (obvious) way to specify the tab origin for
     *  GetTabbedTextExtent(). This makes proper text display nearly impossible.
     *
     *      Also, the Mac provides NO tab support at all. Because of the Mac - we NEED
     *  to implement our own tab support. Given we are doing that anyhow, may as
     *  well use our portable support on both platforms.
     *      For both calculating widths, and doing imaging, we follow essentially
     *  the same algorithm.
     *      o    Walk the text looking for a tab or EOL.
     *      o    Take the tab-free initial segment and image(calculate) it.
     *      o    if now on a tab, image (calculate) that.
     *      o    Advance indexes over what we've imaged (calculated) and repeat.
     */
    class TabStopList {
    protected:
        TabStopList () = default;

    public:
        virtual ~TabStopList () = default;

    public:
        /*
        @DESCRIPTION:   <p>Compute where the 'ith' tabstop lies (i >= 0, and can be arbitrarily high).
            Note that though tabstop <code>i=0</code> is the first tabstop (which by convention is usually NOT
            at the left margin - but is really the logically 'first' one).</p>
                <p>Note - this used to return Distance - but in Led 3.0 - it changed to returning TWIPS (see SPR#0767);</p>
        */
        virtual TWIPS ComputeIthTab (size_t i) const = 0;
        /*
        @DESCRIPTION:   <p>Given a distance from the left margin, find the immediately following tabstops position.
            This is what is actaully used in the imagers to compute where to draw text. It can logically
            be implemented by walking the tabstoplist and calling @'TextImager::TabStopList::ComputeIthTab', and
            returning the next one after the one past(or equal to) <code>afterPos</code>.</p>
                <p>Note - this used to take/return Distance - but in Led 3.0 - it changed to take/return TWIPS (see SPR#0767);</p>
                <p>Now there is an overloaded version using Distance that takes a Tablet* as argument.</p>
        */
        virtual TWIPS           ComputeTabStopAfterPosition (TWIPS afterPos) const = 0;
        nonvirtual DistanceType ComputeTabStopAfterPosition (Tablet* tablet, DistanceType afterPos) const;
    };

#if qPlatform_Windows
    /**
     */
    class FontObject {
    public:
        FontObject () = default;
        ~FontObject ();
        nonvirtual      operator HFONT () const;
        nonvirtual int  GetObject (int nCount, LPVOID lpObject) const;
        nonvirtual BOOL DeleteObject ();
        nonvirtual BOOL CreateFontIndirect (const LOGFONT* lpLogFont);
        nonvirtual BOOL Attach (HFONT hObject);

    public:
        HFONT m_hObject{nullptr};
    };
#endif

#if qPlatform_Windows
    /**
     */
    class Brush {
    public:
        Brush (COLORREF crColor);
        ~Brush ();
        nonvirtual      operator HBRUSH () const;
        nonvirtual BOOL Attach (HBRUSH hObject);
        nonvirtual BOOL DeleteObject ();

    private:
        HBRUSH m_hObject{nullptr};
    };
#endif

    /**
     */
    template <typename COORD_TYPE>
    struct Point_Base {
    public:
        using CoordinateType = COORD_TYPE;

    public:
        Point_Base ();
        Point_Base (COORD_TYPE newV, COORD_TYPE newH);
        template <typename OTHER_POINT_BASE_TYPE>
        explicit Point_Base (OTHER_POINT_BASE_TYPE o)
            : v (COORD_TYPE (o.v))
            , h (COORD_TYPE (o.h))
        {
        }

    public:
        COORD_TYPE v;
        COORD_TYPE h;

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const Point_Base& rhs) const = default;
    };

    /*
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
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
        nonvirtual DistanceType   GetHeight () const;
        nonvirtual DistanceType   GetWidth () const;
        nonvirtual POINT_TYPE     GetTopLeft () const;
        nonvirtual POINT_TYPE     GetTopRight () const;
        nonvirtual POINT_TYPE     GetBotRight () const;
        nonvirtual POINT_TYPE     GetOrigin () const;
        nonvirtual SIZE_TYPE      GetSize () const;
        nonvirtual bool           Contains (POINT_TYPE p) const;
        nonvirtual bool           Contains (THIS_TYPE r) const;

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

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const Rect_Base& rhs) const = default;
        bool                      operator== (const Rect_Base& rhs) const;
    };

    /**
     *  Simple typedef of @'Point_Base<COORD_TYPE>' using @'Distance'.
     */
    using Led_Size = Point_Base<DistanceType>;

    /**
     *  Simple typedef of @'Point_Base<COORD_TYPE>' using @'CoordinateType'.
     */
    using Led_Point = Point_Base<CoordinateType>;
    Led_Point operator- (const Led_Point& lhs, const Led_Point& rhs);

    /*
    @DESCRIPTION:   <p>Simple typedef of @'Point_Base<COORD_TYPE>' using @'TWIPS'.</p>
    */
    using TWIPS_Point = Point_Base<TWIPS>;

    /*
    @DESCRIPTION:   <p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'Led_Point' and @'Led_Size'.</p>
    */
    using Led_Rect = Rect_Base<Led_Point, Led_Size>;

    /*
    @DESCRIPTION:   <p>Simple typedef of @'Rect_Base<POINT_TYPE,SIZE_TYPE>' using @'TWIPS_Point'.</p>
    */
    using TWIPS_Rect = Rect_Base<TWIPS_Point, TWIPS_Point>;

    /**
     * \brief Portable GDI abstraction for 'Region' object.
     */
    class Region {
    public:
        Region ();
        Region (const Led_Rect& r);
        Region (const Region& from);
        virtual ~Region ();
        nonvirtual const Region& operator= (const Region& rhs);

    public:
        nonvirtual bool     IsEmpty () const;
        nonvirtual Led_Rect GetBoundingRect () const;

    public:
#if qPlatform_MacOS
        Region (RgnHandle rgn);
        RgnHandle GetOSRep () const;
        RgnHandle GetOSRep ();

    private:
        bool fOwned;
#elif qPlatform_Windows
             operator HRGN () const;
        int  CombineRgn (Region* pRgn1, Region* pRgn2, int nCombineMode);
        BOOL PtInRegion (int x, int y) const;
        BOOL PtInRegion (POINT point) const;

    private:
        BOOL DeleteObject ();
#endif
    private:
#if qPlatform_MacOS
        RgnHandle fRgn;
#elif qPlatform_Windows
        HRGN fRgn;
#endif
    };
    Region operator* (const Region& lhs, const Region& rhs);
    Region operator+ (const Region& lhs, const Region& rhs);

    /**
     * This class is a portable representation of a color. It can be constructed either
     * from its basic RGB componets, or from the native color representations on a particular platform.
     */
    class Color {
    public:
        // regardless of Mac or Windows, we use the same size ColorValue so we can write
        // portable code more easily...
        using ColorValue = unsigned short;
        enum { kColorValueMin = 0,
               kColorValueMax = USHRT_MAX };

    public:
        constexpr Color (const Color&) = default;
        constexpr Color (ColorValue redValue, ColorValue greenValue, ColorValue blueValue);
#if qPlatform_MacOS
        explicit Color (const RGBColor& rgbColor);
#elif qPlatform_Windows
        explicit Color (COLORREF colorRef);
#endif

    public:
        nonvirtual ColorValue GetRed () const;
        nonvirtual ColorValue GetGreen () const;
        nonvirtual ColorValue GetBlue () const;

        /*
     *  Some useful predefined values.
     */
    public:
        static const Color kBlack;
        static const Color kWhite;
        static const Color kRed;
        static const Color kGreen;
        static const Color kBlue;
        static const Color kCyan;
        static const Color kMagenta;
        static const Color kYellow;
        static const Color kMaroon;
        static const Color kOlive;
        static const Color kNavyBlue;
        static const Color kPurple;
        static const Color kTeal;
        static const Color kGray;
        static const Color kSilver; // aka 'light gray'
        static const Color kDarkGreen;
        static const Color kLimeGreen;
        static const Color kFuchsia;
        static const Color kAqua;

    public:
#if qPlatform_MacOS
        nonvirtual RGBColor GetOSRep () const;
#elif qPlatform_Windows
        nonvirtual COLORREF GetOSRep () const;
#endif

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const Color& rhs) const = default;
        constexpr bool            operator== (const Color& rhs) const  = default;

    private:
        ColorValue fRed;
        ColorValue fGreen;
        ColorValue fBlue;
    };

    Color operator* (Color lhs, float factor);
    Color operator/ (Color lhs, float divBy);
    Color operator+ (Color lhs, Color rhs);
    Color operator- (Color lhs, Color rhs);

    unsigned int Distance (Color lhs, Color rhs);
    unsigned int Distance_Squared (Color lhs, Color rhs);
#if qPlatform_Windows
    unsigned int Distance_Squared (COLORREF lhs, COLORREF rhs);
#endif

    /**
     *  Helper class to keep track of GDI information used for drawing.
     *  Very different implementations befween Mac and Windows.
     *
     *  Note - this class is used in conjunction with @'GDI_Obj_Selector'.
     */
    class Pen {
#if qPlatform_MacOS
    public:
        static const Pattern kWhitePattern;
        static const Pattern kLightGrayPattern;
        static const Pattern kGrayPattern;
        static const Pattern kDarkGrayPattern;
        static const Pattern kBlackPattern;

    public:
        Pen (short penStyle = srcCopy, const Pattern* penPat = &kBlackPattern, const Color& color = Color::kBlack);

    public:
        short   fPenStyle;
        Pattern fPenPat;
        Color   fPenColor;
#endif
#if qPlatform_Windows
    public:
        Pen (int nPenStyle, int nWidth, COLORREF crColor);
        ~Pen ();
        nonvirtual      operator HPEN () const;
        nonvirtual BOOL Attach (HPEN hObject);
        nonvirtual BOOL DeleteObject ();

    private:
        HPEN m_hObject;
#endif
    };

    /*
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
    enum Justification { eLeftJustify,
                         eRightJustify,
                         eCenterJustify,
                         eFullyJustify,
                         eDefaultForScriptJustify };

    /**
     *  There are two defined text directions:
     *      <li><em>eLeftToRight</em></li>
     *      <li><em>eRightToLeft</em></li>
     */
    enum TextDirection {
        eLeftToRight,
        eRightToLeft
    };

    /**
     *      Different styles of bullet / list markers for list items in the WordProcessor.</p>
     *
     *       Numeric values come from RTF 1.5 Spec \levelnfc (except for eListStyle_None which is special)</p>
     */
    enum ListStyle {
        eListStyle_None   = 9999,
        eListStyle_Bullet = 23
    };

    /*
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
    class LineSpacing {
    public:
        enum Rule {
            eSingleSpace,
            eOnePointFiveSpace,
            eDoubleSpace,
            eAtLeastTWIPSSpacing,
            eExactTWIPSSpacing,
            eExactLinesSpacing
        };
        Rule     fRule{eSingleSpace};
        unsigned fArg{0};

    public:
        LineSpacing () = default;
        LineSpacing (Rule rule);
        LineSpacing (Rule rule, TWIPS twips);
        LineSpacing (Rule rule, unsigned lineCount);
        bool operator== (LineSpacing rhs) const;
    };

    class IncrementalFontSpecification;

    /**
     *      <code>FontSpecification</code> is a utility class which portably represents
     *  a user font choice. This largely corresponds to the MS-Windows <code>LOGFONT</code> structure
     *  or the Macintosh <code>txFace, txSize, txStyle</code>.</p>
     *      In addition to being a portable represenation of this information, it
     *  also contains handy wrapper accessors, and extra information like subscript,
     *  superscript, and font color.</p>
     *      See also, @'IncrementalFontSpecification'</p>
     */
    class FontSpecification {
    public:
        // Force users to be EXPLICIT about this object-slicing, since many of the fields
        // maybe invalid... Will the compiler REALLY do this check???? We'll see - LGP 970314
        FontSpecification ();
#if qPlatform_Windows
        explicit FontSpecification (const LOGFONT& logFont);
#endif
        explicit FontSpecification (const IncrementalFontSpecification& from);

    public:
#if qPlatform_MacOS
        using FontNameSpecifier = short;
#elif qPlatform_Windows
        struct FontNameSpecifier { // So struct copies etc will work and so we can define op==
            FontNameSpecifier ();
            FontNameSpecifier (const Led_SDK_Char* from);
            Led_SDK_Char fName[LF_FACESIZE];
            bool         operator== (const FontNameSpecifier& rhs) const
            {
                return ::_tcscmp (fName, rhs.fName) == 0;
            }
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

        nonvirtual Color GetTextColor () const;
        nonvirtual void  SetTextColor (const Color& textColor);

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
        static string     mkOSRep (const string& foundry, const string& family, const string& weight, const string& slant, const string& pointSize);
        nonvirtual string GetOSRep () const;
        nonvirtual void   SetFromOSRep (const string& osRep);
#endif

    public:
        /**
         */
        strong_ordering operator<=> (const FontSpecification& rhs) const;

    public:
        /**
         */
        bool operator== (const FontSpecification& rhs) const;

    public:
        nonvirtual void MergeIn (const IncrementalFontSpecification& addInTheseAttributes);

    private:
#if qPlatform_MacOS
        short fFontSpecifier;
        short fFontSize;
        Style fFontStyle;
#elif qPlatform_Windows
        LOGFONT fFontInfo; // Could make this MUCH smaller on windows - do for future release!
#elif qStroika_FeatureSupported_XWindows
        FontNameSpecifier fFontFamily;
        bool              fBold : 1;
        bool              fItalics : 1;
        bool              fUnderline : 1;
        short             fFontSize;
#endif
        SubOrSuperScript fSubOrSuperScript;
        Color            fTextColor;
    };

    /*
     *      <code>IncrementalFontSpecification</code> is a simple subclass of
     *  @'FontSpecification' which adds a bool flag for each
     *  font attribute indicating whether or not it is really specified.
     *  With this, and the @'FontSpecification::MergeIn' method,
     *  you can specify just one or two changes to a font record, pass them around,
     *  and apply them to an existing font choice.</p>
     *      See also, @'FontSpecification'
     */
    class IncrementalFontSpecification : public FontSpecification {
    private:
        using inherited = FontSpecification;

    public:
        IncrementalFontSpecification ();

        // I may end up regretting this, for all the confusion it
        // can cause, but it sure makes a number of things simpler
        // and clearer-- LGP 960520
        IncrementalFontSpecification (const FontSpecification& fontSpec);

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
        nonvirtual bool GetStyle_Strikeout () const;
        nonvirtual bool GetStyle_Strikeout_Valid () const;
        nonvirtual void InvalidateStyle_Strikeout ();
        nonvirtual void SetStyle_Strikeout (bool isStrikeout);
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

        nonvirtual Color GetTextColor () const;
        nonvirtual bool  GetTextColor_Valid () const;
        nonvirtual void  InvalidateTextColor ();
        nonvirtual void  SetTextColor (const Color& textColor);

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

        bool operator<=> (const IncrementalFontSpecification& rhs) = delete;
        bool operator== (const IncrementalFontSpecification& rhs) const;

    public:
        nonvirtual void MergeIn (const IncrementalFontSpecification& addInTheseAttributes);

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

    IncrementalFontSpecification Intersection (const IncrementalFontSpecification& lhs, const IncrementalFontSpecification& rhs);

    // Must support for other Platforms - but not done yet... Also - should have OPTIONS specified to this class (CTOR) picking various filter
    // options...
    class InstalledFonts {
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
        InstalledFonts (
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

    /**
     *  Something of a hack version of GDI global variables. We want to keep certain GDI global variables
     *  computed ONCE, for speed reasons. And yet - if we get a certain windows message, we must refresh our cached global
     *  variables. The compromise is that all these globals are associated with this class, so that there is one place to
     *   call to refresh those globals.
     */
    class Globals {
    public:
        Globals ();

    public:
        static Globals& Get ();

    private:
        static Globals* sThe;

    public:
        nonvirtual void InvalidateGlobals ();

    public:
        nonvirtual DistanceType GetMainScreenLogPixelsH () const;
        nonvirtual DistanceType GetMainScreenLogPixelsV () const;

    public:
        DistanceType fLogPixelsH;
        DistanceType fLogPixelsV;

    private:
        class _Global_DESTRUCTOR_;
        friend class _Global_DESTRUCTOR_;
    };

    Led_Rect operator- (const Led_Rect& lhs, const Led_Point& rhs);
    Led_Rect operator+ (Led_Point p, Led_Rect r);
    Led_Rect operator+ (Led_Rect r, Led_Point p);
    bool     Intersect (const Led_Rect& lhs, const Led_Rect& rhs);
    bool     Intersect (const Led_Rect& lhs, const Region& rhs);
    bool     Intersect (const Region& lhs, const Led_Rect& rhs);
    bool     Intersect (const Region& lhs, const Region& rhs);

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

    TWIPS          Led_CvtScreenPixelsToTWIPSV (CoordinateType from);
    TWIPS          Led_CvtScreenPixelsToTWIPSH (CoordinateType from);
    CoordinateType Led_CvtScreenPixelsFromTWIPSV (TWIPS from);
    CoordinateType Led_CvtScreenPixelsFromTWIPSH (TWIPS from);

    /*
    @CLASS:         FontMetrics
    @DESCRIPTION:   <p><code>FontMetrics</code> is a portable wrapper class on the Macintosh
        <em>FontInfo</em> structure, or the Windows <em>TEXTMETRIC</em> structure. It provides
        portable access to things like GetLineHeight (), and GetAscent (), etc...</p>
    */
    class FontMetrics {
#if qStroika_FeatureSupported_XWindows
    public:
        struct PlatformSpecific {
            DistanceType fAscent;
            DistanceType fDescent;
            DistanceType fLeading;
            DistanceType fMaxCharWidth;
        };
#endif
    public:
        FontMetrics ();
#if qPlatform_MacOS
        FontMetrics (const FontInfo& from);
#elif qPlatform_Windows
        FontMetrics (const TEXTMETRIC& from);
#elif qStroika_FeatureSupported_XWindows
        FontMetrics (const PlatformSpecific& from);
#endif
        FontMetrics (const FontMetrics& from);
        const FontMetrics& operator= (const FontMetrics& rhs);

    public:
        nonvirtual DistanceType GetAscent () const;
        nonvirtual DistanceType GetDescent () const;
        nonvirtual DistanceType GetLeading () const;
        nonvirtual DistanceType GetHeight () const;     // does NOT include leading
        nonvirtual DistanceType GetLineHeight () const; // includes leading

    public:
        nonvirtual DistanceType GetMaxCharacterWidth () const;
#if qPlatform_Windows
        nonvirtual DistanceType GetAveCharacterWidth () const;
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

    Color Led_GetTextColor ();
    Color Led_GetTextBackgroundColor ();
    Color Led_GetSelectedTextColor ();
    Color Led_GetSelectedTextBackgroundColor ();

    class OffscreenTablet;

    /**
     *  This class is used to wrap a low level graphics drawing device. On Windows - this is an HDC.
     *  On the Mac - a GrafPtr (also CGrafPtr and GWorldPtr). On X-Windows - a drawable and display, and GC.</p>
     * 
     *  This class right now is a very thin wrapper on those drawing prodedures (mostly for backward compatability reasons.
     *  Eventually - it may do a better job of wrapping those concepts/APIs genericly.</p>
     */
    class Tablet {
    public:
#if qPlatform_MacOS
        Tablet (GrafPtr gp);
#elif qPlatform_Windows
        enum OwnDCControl { eOwnsDC,
                            eDoesntOwnDC };

        Tablet (HDC hdc = nullptr, OwnDCControl ownsDC = eOwnsDC);
#elif qStroika_FeatureSupported_XWindows
        Tablet (Display* display, Drawable drawable);
#endif

    public:
        virtual ~Tablet ();

    public:
#if qPlatform_MacOS
        nonvirtual operator GrafPtr () const;
#elif qPlatform_Windows
        nonvirtual operator HDC () const;
#endif

    public:
        nonvirtual CoordinateType CvtFromTWIPSV (TWIPS from) const;
        nonvirtual CoordinateType CvtFromTWIPSH (TWIPS from) const;
        nonvirtual TWIPS          CvtToTWIPSV (CoordinateType from) const;
        nonvirtual TWIPS          CvtToTWIPSH (CoordinateType from) const;

    public:
        nonvirtual Led_Point   CvtFromTWIPS (TWIPS_Point from) const;
        nonvirtual TWIPS_Point CvtToTWIPS (Led_Point from) const;
        nonvirtual Led_Rect    CvtFromTWIPS (TWIPS_Rect from) const;
        nonvirtual TWIPS_Rect  CvtToTWIPS (Led_Rect from) const;

    public:
        nonvirtual void ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, CoordinateType scrollBy);
        nonvirtual void FrameRegion (const Region& r, const Color& c);

    public:
        nonvirtual void FrameRectangle (const Led_Rect& r, Color c, DistanceType borderWidth);

    public:
#if qPlatform_MacOS
        nonvirtual void SetPort ();
#elif qPlatform_Windows
        nonvirtual BOOL     BitBlt (int x, int y, int nWidth, int nHeight, Tablet* pSrcDC, int xSrc, int ySrc, DWORD dwRop);
        nonvirtual BOOL     CreateCompatibleDC (Tablet* pDC);
        nonvirtual COLORREF SetTextColor (COLORREF crColor);
        nonvirtual COLORREF SetBkColor (COLORREF crColor);
        nonvirtual BOOL     IsPrinting () const;
        nonvirtual BOOL     RoundRect (int x1, int y1, int x2, int y2, int x3, int y3);
        nonvirtual BOOL     TextOut (int x, int y, LPCTSTR lpszString, int nCount);
        //      nonvirtual  SIZE    GetTextExtent (LPCTSTR lpszString, int nCount) const;
        nonvirtual int     SetBkMode (int nBkMode);
        nonvirtual SIZE    GetWindowExt () const;
        nonvirtual SIZE    GetViewportExt () const;
        nonvirtual BOOL    Rectangle (int x1, int y1, int x2, int y2);
        nonvirtual BOOL    Rectangle (const RECT& r);
        nonvirtual BOOL    Rectangle (LPCRECT lpRect);
        nonvirtual BOOL    GetTextMetrics (LPTEXTMETRIC lpMetrics) const;
        nonvirtual HBITMAP SelectObject (HBITMAP hBitmap);
#if defined(STRICT)
        nonvirtual HFONT SelectObject (HFONT hFont);
#endif
        nonvirtual POINT SetWindowOrg (int x, int y);
        nonvirtual int   GetDeviceCaps (int nIndex) const;
        nonvirtual BOOL  Attach (HDC hDC, OwnDCControl ownsDC = eOwnsDC);
        nonvirtual HDC   Detach ();
#elif qStroika_FeatureSupported_XWindows
    public:
        nonvirtual void SetFont (const FontSpecification& fontSpec);

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
        nonvirtual Led_SDK_String BestMatchFont (const FontSpecification& fsp, const vector<Led_SDK_String>& fontsList);

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
        nonvirtual void MeasureText (const FontMetrics& precomputedFontMetrics,
                                     const Led_tChar* text, size_t nTChars, DistanceType* charLocations);
        nonvirtual void TabbedTextOut (const FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
                                       TextDirection direction,
                                       Led_Point outputAt, CoordinateType hTabOrigin, const TabStopList& tabStopList,
                                       DistanceType* amountDrawn, CoordinateType hScrollOffset);

    public:
        nonvirtual void SetBackColor (const Color& backColor);
        nonvirtual void SetForeColor (const Color& foreColor);

    public:
        nonvirtual void EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Color& eraseColor);
        nonvirtual void HilightArea_SolidHelper (const Led_Rect& hilightArea, Color hilightBackColor, Color hilightForeColor, Color oldBackColor, Color oldForeColor);
        nonvirtual void HilightArea_SolidHelper (const Region& hilightArea, Color hilightBackColor, Color hilightForeColor, Color oldBackColor, Color oldForeColor);

#if qPlatform_Windows
    private:
        class RecolorHelper;

    private:
        RecolorHelper* fRecolorHelper;
#endif

    public:
        class ClipNarrowAndRestore;

    public:
        nonvirtual Region GetClip () const;
        nonvirtual bool   GetClip (Region* r) const;
        nonvirtual void   SetClip ();
        nonvirtual void   SetClip (const Led_Rect& clipTo);
        nonvirtual void   SetClip (const Region& clipTo);

    public:
        nonvirtual FontMetrics GetFontMetrics () const;

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
        mutable DistanceType fLogPixelsV;
        mutable DistanceType fLogPixelsH;
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
    @CLASS:         Tablet::ClipNarrowAndRestore
    @DESCRIPTION:   <p>Further narrow the existing clip region in the given tablet to the constructor. Then
                restore the clip region for the tablet to what it was when the contructor was called.</p>
    */
    class Tablet::ClipNarrowAndRestore {
    public:
        ClipNarrowAndRestore (Tablet* tablet);
        ClipNarrowAndRestore (Tablet* tablet, const Led_Rect& clipFurtherTo);
        ClipNarrowAndRestore (Tablet* tablet, const Region& clipFurtherTo);
        ~ClipNarrowAndRestore ();

    private:
        Tablet* fTablet;
        bool    fHasOldClip;
        Region  fOldClip;
    };

#if qPlatform_MacOS
    /**
     */
    GrafPtr Led_GetCurrentGDIPort ();
#endif

#if qPlatform_Windows
    class WindowDC : public Tablet {
    public:
        WindowDC (HWND hWnd);
        ~WindowDC ();

    private:
        HWND fHWnd_;
    };
#endif

#if qPlatform_MacOS
    class MacPortAndClipRegionEtcSaver {
    public:
        MacPortAndClipRegionEtcSaver ();
        ~MacPortAndClipRegionEtcSaver ();

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
    class Bitmap {
    public:
        Bitmap () = default;
        ~Bitmap ();

    public:
        nonvirtual void DeleteObject ();
        nonvirtual      operator HBITMAP () const;

    public:
        nonvirtual Led_Size GetImageSize () const;
        nonvirtual BOOL     CreateCompatibleBitmap (HDC hdc, DistanceType nWidth, DistanceType nHeight);
        nonvirtual BOOL     CreateCompatibleDIBSection (HDC hdc, DistanceType nWidth, DistanceType nHeight);
        nonvirtual void     LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName);

    private:
        HBITMAP  m_hObject{nullptr};
        Led_Size fImageSize{};
    };
#endif

    /*
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
        nonvirtual void Setup (Tablet* origTablet);
        nonvirtual Tablet* PrepareRect (const Led_Rect& currentRowRect, DistanceType extraToAddToBottomOfRect = 0);
        nonvirtual void    BlastBitmapToOrigTablet ();

    private:
        class OT : public Tablet {
        private:
            using inherited = Tablet;

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
        Tablet*  fOrigTablet;
        Led_Rect fOffscreenRect;
        Tablet*  fOffscreenTablet;
#if qPlatform_MacOS
        GDHandle  fOrigDevice;
        CGrafPtr  fOrigPort;
        GWorldPtr fOffscreenGWorld;
#elif qPlatform_Windows
        OT     fMemDC;
        Bitmap fMemoryBitmap; // only can create / select inside loop cuz there is where we know the size.
        // but decare outside, so stays around for successive rows which are the same size.
        HBITMAP fOldBitmapInDC; // used for save/restore of bitmap associated with the DC.
#elif qStroika_FeatureSupported_XWindows
        Drawable fPixmap;
#endif
    };

    /*
    @DESCRIPTION:   <p><code>GDI_Obj_Selector</code> is a stack-based class designed to help
        out selecting objects into a Tablet* (windows DC, grafport, etc).
            <p>The constructor takes a tablet, and object to select into it (HGDIObject, etc),
        and selects it into the tablet. It saves gthe results of the SelectObject calls (old values).
        And on its destructor, it restores the old values.</p>
            <p>This is useful when you want to make sure that an object you've selected into a tablet (HDC)
        will be released, and restored to its original state through all paths through
        the code, including in the even of exceptions.</p>
    */
    class GDI_Obj_Selector {
    public:
#if qPlatform_Windows
        GDI_Obj_Selector (Tablet* tablet, HGDIOBJ objToSelect);
#elif qPlatform_MacOS || qStroika_FeatureSupported_XWindows
        GDI_Obj_Selector (Tablet* tablet, const Pen& pen);
#endif
    public:
        ~GDI_Obj_Selector ();

    private:
        Tablet* fTablet;
#if qPlatform_Windows
        HGDIOBJ fRestoreObject;
        HGDIOBJ fRestoreAttribObject;
#elif qPlatform_MacOS
        Pen fRestorePen;
#endif
    };

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

    void AddRectangleToRegion (Led_Rect addRect, Region* toRgn);

#if qProvideIMESupport
    class IME {
    public:
        IME ();

    public:
        static IME& Get ();

    private:
        static IME* sThe;

    private:
        nonvirtual bool Available () const; //tmphack - don't think this is ever used
    public:
        nonvirtual void    ForgetPosition ();
        nonvirtual void    NotifyPosition (HWND pWnd, const SHORT x, const SHORT y);
        nonvirtual void    NotifyOfFontChange (HWND hWnd, const LOGFONT& lf);
        nonvirtual void    IMEOn (HWND pWnd);
        nonvirtual void    IMEOff (HWND pWnd);
        nonvirtual void    Enable ();
        nonvirtual void    Disable ();
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
