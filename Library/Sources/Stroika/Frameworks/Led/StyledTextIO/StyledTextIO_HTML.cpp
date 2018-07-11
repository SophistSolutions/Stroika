/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include <cctype>
#include <cstdio> // for sprintf()

#include "../../../Foundation/Characters/CString/Utilities.h"
#include "../../../Foundation/Characters/CodePage.h"
#include "../../../Foundation/Characters/LineEndings.h"

#include "../Config.h"

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4786) //qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif

#include "../StyledTextEmbeddedObjects.h"

#include "StyledTextIO_HTML.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

namespace {

    bool ParseStringToInt_ (const string& s, int* t)
    {
        int   l = 0;
        char* e = nullptr;
        l       = strtol (s.c_str (), &e, 10);
        if (s.c_str () == e) {
            return false;
        }
        else {
            *t = l;
            return true;
        }
    }

    inline bool IsASCIISpace (Led_tChar c)
    {
        return isascii (c) and isspace (c);
    }

#if qStaticInitializerOfPairOfStringStringInternalCompilerBug
    static vector<pair<string, string>> _mk_Color_Name_Table ()
    {
        static pair<string, string> kColorNameTable[] = {
            pair<string, string> ("black", "000000"),
            pair<string, string> ("silver", "C0C0C0"),
            pair<string, string> ("gray", "808080"),
            pair<string, string> ("white", "FFFFFF"),
            pair<string, string> ("maroon", "800000"),
            pair<string, string> ("red", "FF0000"),
            pair<string, string> ("purple", "800080"),
            pair<string, string> ("fuchsia", "FF00FF"),
            pair<string, string> ("green", "008000"),
            pair<string, string> ("lime", "00FF00"),
            pair<string, string> ("olive", "808000"),
            pair<string, string> ("yellow", "FFFF00"),
            pair<string, string> ("navy", "000080"),
            pair<string, string> ("blue", "0000FF"),
            pair<string, string> ("teal", "008080"),
            pair<string, string> ("aqua", "00FFFF"),
        };
        return vector<pair<string, string>> (&kColorNameTable[0], &kColorNameTable[NEltsOf (kColorNameTable)]);
    }
    static vector<pair<string, string>> kColorNameTable = _mk_Color_Name_Table ();
#else
    static pair<string, string> kColorNameTable[] = {
        pair<string, string> ("black", "000000"),
        pair<string, string> ("silver", "C0C0C0"),
        pair<string, string> ("gray", "808080"),
        pair<string, string> ("white", "FFFFFF"),
        pair<string, string> ("maroon", "800000"),
        pair<string, string> ("red", "FF0000"),
        pair<string, string> ("purple", "800080"),
        pair<string, string> ("fuchsia", "FF00FF"),
        pair<string, string> ("green", "008000"),
        pair<string, string> ("lime", "00FF00"),
        pair<string, string> ("olive", "808000"),
        pair<string, string> ("yellow", "FFFF00"),
        pair<string, string> ("navy", "000080"),
        pair<string, string> ("blue", "0000FF"),
        pair<string, string> ("teal", "008080"),
        pair<string, string> ("aqua", "00FFFF"),
    };
#endif

    static inline char NumToHexChar (unsigned i)
    {
        Require (i <= 15);
        if (i <= 9) {
            return static_cast<char> (i + '0');
        }
        else {
            return static_cast<char> (i - 10 + 'a');
        }
    }
    static inline unsigned short HexCharToNum (char c)
    {
        if (c >= '0' and c <= '9') {
            return c - '0';
        }
        else if (c >= 'a' and c <= 'f') {
            return c - 'a' + 10;
        }
        else if (c >= 'A' and c <= 'F') {
            return c - 'A' + 10;
        }
        else {
            return 0;
        }
    }
    static inline bool ParseColorString (string colorStr, Led_Color* color)
    {
#if qStaticInitializerOfPairOfStringStringInternalCompilerBug
        for (auto i = kColorNameTable.begin (); i != kColorNameTable.end (); ++i) {
            if (Led_CasedStringsEqual (colorStr, i->first)) {
                colorStr = i->second;
                break;
            }
        }
#else
        for (size_t i = 0; i < NEltsOf (kColorNameTable); ++i) {
            if (Led_CasedStringsEqual (colorStr, kColorNameTable[i].first)) {
                colorStr = kColorNameTable[i].second;
                break;
            }
        }
#endif
        // Cheezy quick hack color parser - for spr# 0622 - LGP990827
        if (colorStr.length () == 6) {
            unsigned short red   = HexCharToNum (colorStr[0]) * 16 + HexCharToNum (colorStr[1]);
            unsigned short green = HexCharToNum (colorStr[2]) * 16 + HexCharToNum (colorStr[3]);
            unsigned short blue  = HexCharToNum (colorStr[4]) * 16 + HexCharToNum (colorStr[5]);
            *color               = Led_Color (red == 255 ? 0xffff : red << 8, green == 255 ? 0xffff : green << 8, blue == 255 ? 0xffff : blue << 8);
            return true;
        }
        return false;
    }
}

/*
 ********************************************************************************
 ************************************ HTMLInfo **********************************
 ********************************************************************************
 */
HTMLInfo::HTMLInfo ()
    : fDocTypeTag ()
    , fHTMLTag ()
    , fHeadTag ()
    , fStartBodyTag ()
    , fUnknownHeaderTags ()
    , fTitle ()
{
}

HTMLInfo::EntityRefMapEntry HTMLInfo::sDefaultEntityRefMapTable[] = {

    EntityRefMapEntry ("euro", 0x20ac), // As part of bug request SPR#0852 - this number was given to us. Not in the HTML 4.0 spec (but must be somewhere!)

    /*
     * FROM http://www.w3.org/TR/WD-html40-970708/sgml/entities.html#h-10.5.1
     */
    EntityRefMapEntry ("nbsp", 0x00a0),
    EntityRefMapEntry ("iexcl", 0x00a1),
    EntityRefMapEntry ("cent", 0x00a2),
    EntityRefMapEntry ("pound", 0x00a3),
    EntityRefMapEntry ("curren", 0x00a4),
    EntityRefMapEntry ("yen", 0x00a5),
    EntityRefMapEntry ("brvbar", 0x00a6),
    EntityRefMapEntry ("sect", 0x00a7),
    EntityRefMapEntry ("uml", 0x00a8),
    EntityRefMapEntry ("copy", 0x00a9),
    EntityRefMapEntry ("ordf", 0x00aa),
    EntityRefMapEntry ("laquo", 0x00ab),
    EntityRefMapEntry ("not", 0x00ac),
    EntityRefMapEntry ("shy", 0x00ad),
    EntityRefMapEntry ("reg", 0x00ae),
    EntityRefMapEntry ("macr", 0x00af),
    EntityRefMapEntry ("deg", 0x00b0),
    EntityRefMapEntry ("plusmn", 0x00b1),
    EntityRefMapEntry ("sup2", 0x00b2),
    EntityRefMapEntry ("sup3", 0x00b3),
    EntityRefMapEntry ("acute", 0x00b4),
    EntityRefMapEntry ("micro", 0x00b5),
    EntityRefMapEntry ("para", 0x00b6),
    EntityRefMapEntry ("middot", 0x00b7),
    EntityRefMapEntry ("cedil", 0x00b8),
    EntityRefMapEntry ("sup1", 0x00b9),
    EntityRefMapEntry ("ordm", 0x00ba),
    EntityRefMapEntry ("raquo", 0x00bb),
    EntityRefMapEntry ("frac14", 0x00bc),
    EntityRefMapEntry ("frac12", 0x00bd),
    EntityRefMapEntry ("frac34", 0x00be),
    EntityRefMapEntry ("iquest", 0x00bf),
    EntityRefMapEntry ("Agrave", 0x00c0),
    EntityRefMapEntry ("Aacute", 0x00c1),
    EntityRefMapEntry ("Acirc", 0x00c2),
    EntityRefMapEntry ("Atilde", 0x00c3),
    EntityRefMapEntry ("Auml", 0x00c4),
    EntityRefMapEntry ("Aring", 0x00c5),
    EntityRefMapEntry ("AElig", 0x00c6),
    EntityRefMapEntry ("Ccedil", 0x00c7),
    EntityRefMapEntry ("Egrave", 0x00c8),
    EntityRefMapEntry ("Eacute", 0x00c9),
    EntityRefMapEntry ("Ecirc", 0x00ca),
    EntityRefMapEntry ("Euml", 0x00cb),
    EntityRefMapEntry ("Igrave", 0x00cc),
    EntityRefMapEntry ("Iacute", 0x00cd),
    EntityRefMapEntry ("Icirc", 0x00ce),
    EntityRefMapEntry ("Iuml", 0x00cf),
    EntityRefMapEntry ("ETH", 0x00d0),
    EntityRefMapEntry ("Ntilde", 0x00d1),
    EntityRefMapEntry ("Ograve", 0x00d2),
    EntityRefMapEntry ("Oacute", 0x00d3),
    EntityRefMapEntry ("Ocirc", 0x00d4),
    EntityRefMapEntry ("Otilde", 0x00d5),
    EntityRefMapEntry ("Ouml", 0x00d6),
    EntityRefMapEntry ("times", 0x00d7),
    EntityRefMapEntry ("Oslash", 0x00d8),
    EntityRefMapEntry ("Ugrave", 0x00d9),
    EntityRefMapEntry ("Uacute", 0x00da),
    EntityRefMapEntry ("Ucirc", 0x00db),
    EntityRefMapEntry ("Uuml", 0x00dc),
    EntityRefMapEntry ("Yacute", 0x00dd),
    EntityRefMapEntry ("THORN", 0x00de),
    EntityRefMapEntry ("szlig", 0x00df),
    EntityRefMapEntry ("agrave", 0x00e0),
    EntityRefMapEntry ("aacute", 0x00e1),
    EntityRefMapEntry ("acirc", 0x00e2),
    EntityRefMapEntry ("atilde", 0x00e3),
    EntityRefMapEntry ("auml", 0x00e4),
    EntityRefMapEntry ("aring", 0x00e5),
    EntityRefMapEntry ("aelig", 0x00e6),
    EntityRefMapEntry ("ccedil", 0x00e7),
    EntityRefMapEntry ("egrave", 0x00e8),
    EntityRefMapEntry ("eacute", 0x00e9),
    EntityRefMapEntry ("ecirc", 0x00ea),
    EntityRefMapEntry ("euml", 0x00eb),
    EntityRefMapEntry ("igrave", 0x00ec),
    EntityRefMapEntry ("iacute", 0x00ed),
    EntityRefMapEntry ("icirc", 0x00ee),
    EntityRefMapEntry ("iuml", 0x00ef),
    EntityRefMapEntry ("eth", 0x00f0),
    EntityRefMapEntry ("ntilde", 0x00f1),
    EntityRefMapEntry ("ograve", 0x00f2),
    EntityRefMapEntry ("oacute", 0x00f3),
    EntityRefMapEntry ("ocirc", 0x00f4),
    EntityRefMapEntry ("otilde", 0x00f5),
    EntityRefMapEntry ("ouml", 0x00f6),
    EntityRefMapEntry ("divide", 0x00f7),
    EntityRefMapEntry ("oslash", 0x00f8),
    EntityRefMapEntry ("ugrave", 0x00f9),
    EntityRefMapEntry ("uacute", 0x00fa),
    EntityRefMapEntry ("ucirc", 0x00fb),
    EntityRefMapEntry ("uuml", 0x00fc),
    EntityRefMapEntry ("yacute", 0x00fd),
    EntityRefMapEntry ("thorn", 0x00fe),
    EntityRefMapEntry ("yuml", 0x00ff),

    /*
     * FROM http://www.w3.org/TR/WD-html40-970708/sgml/entities.html#h-10.5.2
     */
    //  Latin Extended-B
    EntityRefMapEntry ("fnof", 0x0192),

    //  Greek
    EntityRefMapEntry ("Alpha", 0x391),
    EntityRefMapEntry ("Beta", 0x392),
    EntityRefMapEntry ("Gamma", 0x393),
    EntityRefMapEntry ("Delta", 0x394),
    EntityRefMapEntry ("Epsilon", 0x395),
    EntityRefMapEntry ("Zeta", 0x396),
    EntityRefMapEntry ("Eta", 0x397),
    EntityRefMapEntry ("Theta", 0x398),
    EntityRefMapEntry ("Iota", 0x399),
    EntityRefMapEntry ("Kappa", 0x39a),
    EntityRefMapEntry ("Lambda", 0x39b),
    EntityRefMapEntry ("Mu", 0x39c),
    EntityRefMapEntry ("Nu", 0x39d),
    EntityRefMapEntry ("Xi", 0x39e),
    EntityRefMapEntry ("Omicron", 0x39f),
    EntityRefMapEntry ("Pi", 0x3a0),
    EntityRefMapEntry ("Rho", 0x3a1),
    EntityRefMapEntry ("Sigma", 0x3a3),
    EntityRefMapEntry ("Tau", 0x3a4),
    EntityRefMapEntry ("Upsilon", 0x3a5),
    EntityRefMapEntry ("Phi", 0x3a6),
    EntityRefMapEntry ("Chi", 0x3a7),
    EntityRefMapEntry ("Psi", 0x3a8),
    EntityRefMapEntry ("Omega", 0x3a9),
    EntityRefMapEntry ("alpha", 0x03b1),
    EntityRefMapEntry ("beta", 0x03b2),
    EntityRefMapEntry ("gamma", 0x03b3),
    EntityRefMapEntry ("delta", 0x03b4),
    EntityRefMapEntry ("epsilon", 0x03b5),
    EntityRefMapEntry ("zeta", 0x03b6),
    EntityRefMapEntry ("eta", 0x03b7),
    EntityRefMapEntry ("theta", 0x03b8),
    EntityRefMapEntry ("iota", 0x03b9),
    EntityRefMapEntry ("kappa", 0x03ba),
    EntityRefMapEntry ("lambda", 0x03bb),
    EntityRefMapEntry ("mu", 0x03bc),
    EntityRefMapEntry ("nu", 0x03bd),
    EntityRefMapEntry ("xi", 0x03be),
    EntityRefMapEntry ("omicron", 0x03bf),
    EntityRefMapEntry ("pi", 0x03c0),
    EntityRefMapEntry ("rho", 0x03c1),
    EntityRefMapEntry ("sigmaf", 0x03c2),
    EntityRefMapEntry ("sigma", 0x03c3),
    EntityRefMapEntry ("tau", 0x03c4),
    EntityRefMapEntry ("upsilon", 0x03c5),
    EntityRefMapEntry ("phi", 0x03c6),
    EntityRefMapEntry ("chi", 0x03c7),
    EntityRefMapEntry ("psi", 0x03c8),
    EntityRefMapEntry ("omega", 0x03c9),
    EntityRefMapEntry ("thetasym", 0x03d1),
    EntityRefMapEntry ("upsih", 0x03d2),
    EntityRefMapEntry ("piv", 0x03d6),

    //  General Punctuation
    EntityRefMapEntry ("bull", 0x2022),
    EntityRefMapEntry ("hellip", 0x2026),
    EntityRefMapEntry ("prime", 0x2032),
    EntityRefMapEntry ("Prime", 0x2033),
    EntityRefMapEntry ("oline", 0x203e),
    EntityRefMapEntry ("frasl", 0x2044),

    //  Letterlike Symbols
    EntityRefMapEntry ("weierp", 0x2118),
    EntityRefMapEntry ("image", 0x2111),
    EntityRefMapEntry ("real", 0x211c),
    EntityRefMapEntry ("trade", 0x2122),
    EntityRefMapEntry ("alefsym", 0x2135),

    //  Arrows
    EntityRefMapEntry ("larr", 0x2190),
    EntityRefMapEntry ("uarr", 0x2191),
    EntityRefMapEntry ("rarr", 0x2192),
    EntityRefMapEntry ("darr", 0x2193),
    EntityRefMapEntry ("harr", 0x2194),
    EntityRefMapEntry ("crarr", 0x21b5),
    EntityRefMapEntry ("lArr", 0x21d0),
    EntityRefMapEntry ("uArr", 0x21d1),
    EntityRefMapEntry ("rArr", 0x21d2),
    EntityRefMapEntry ("dArr", 0x21d3),
    EntityRefMapEntry ("hArr", 0x21d4),

    //  Mathematical Operators
    //THESE ARE IN DECIMAL - CONVERT TO HEX TO BE CONSISTENT WITH OTHERS!!!!
    EntityRefMapEntry ("forall", 8704),
    EntityRefMapEntry ("part", 8706),
    EntityRefMapEntry ("exist", 8707),
    EntityRefMapEntry ("empty", 8709),
    EntityRefMapEntry ("nabla", 8711),
    EntityRefMapEntry ("isin", 8712),
    EntityRefMapEntry ("notin", 8713),
    EntityRefMapEntry ("ni", 8715),
    EntityRefMapEntry ("prod", 8719),
    EntityRefMapEntry ("sum", 8721),
    EntityRefMapEntry ("minus", 8722),
    EntityRefMapEntry ("lowast", 8727),
    EntityRefMapEntry ("radic", 8730),
    EntityRefMapEntry ("prop", 8733),
    EntityRefMapEntry ("infin", 8734),
    EntityRefMapEntry ("ang", 8736),
    EntityRefMapEntry ("and", 8869),
    EntityRefMapEntry ("or", 8870),
    EntityRefMapEntry ("cap", 8745),
    EntityRefMapEntry ("cup", 8746),
    EntityRefMapEntry ("int", 8747),
    EntityRefMapEntry ("there4", 8756),
    EntityRefMapEntry ("sim", 8764),
    EntityRefMapEntry ("cong", 8773),
    EntityRefMapEntry ("asymp", 8776),
    EntityRefMapEntry ("ne", 8800),
    EntityRefMapEntry ("equiv", 8801),
    EntityRefMapEntry ("le", 8804),
    EntityRefMapEntry ("ge", 8805),
    EntityRefMapEntry ("sub", 8834),
    EntityRefMapEntry ("sup", 8835),
    EntityRefMapEntry ("nsub", 8836),
    EntityRefMapEntry ("sube", 8838),
    EntityRefMapEntry ("supe", 8839),
    EntityRefMapEntry ("oplus", 8853),
    EntityRefMapEntry ("otimes", 8855),
    EntityRefMapEntry ("perp", 8869),
    EntityRefMapEntry ("sdot", 8901),

    //  Miscellaneous Technical
    //THESE ARE IN DECIMAL - CONVERT TO HEX TO BE CONSISTENT WITH OTHERS!!!!
    EntityRefMapEntry ("lceil", 8968),
    EntityRefMapEntry ("rceil", 8969),
    EntityRefMapEntry ("lfloor", 8970),
    EntityRefMapEntry ("rfloor", 8971),
    EntityRefMapEntry ("lang", 9001),
    EntityRefMapEntry ("rang", 9002),

    //  Geometric Shapes
    //THESE ARE IN DECIMAL - CONVERT TO HEX TO BE CONSISTENT WITH OTHERS!!!!
    EntityRefMapEntry ("loz", 9674),

    //  Miscellaneous Symbols
    //THESE ARE IN DECIMAL - CONVERT TO HEX TO BE CONSISTENT WITH OTHERS!!!!
    EntityRefMapEntry ("spades", 9824),
    EntityRefMapEntry ("clubs", 9827),
    EntityRefMapEntry ("hearts", 9829),
    EntityRefMapEntry ("diams", 9830),

    /*
     * FROM http://www.w3.org/TR/WD-html40-970708/sgml/entities.html#h-10.5.3
     */

    // C0 Controls and Basic Latin
    EntityRefMapEntry ("quot", 0x0022),
    EntityRefMapEntry ("amp", 0x0026),
    EntityRefMapEntry ("lt", 0x003c),
    EntityRefMapEntry ("gt", 0x003e),

    // Latin Extended-A
    EntityRefMapEntry ("OElig", 0x0152),
    EntityRefMapEntry ("oelig", 0x0153),
    EntityRefMapEntry ("Scaron", 0x0160),
    EntityRefMapEntry ("scaron", 0x0161),
    EntityRefMapEntry ("Yuml", 0x0178),

    // Spacing Modifier Letters
    EntityRefMapEntry ("circ", 0x2c6),
    EntityRefMapEntry ("tilde", 0x2dc),

    // General Punctuation
    EntityRefMapEntry ("ensp", 8194),
    EntityRefMapEntry ("emsp", 8195),
    EntityRefMapEntry ("thinsp", 8201),
    EntityRefMapEntry ("zwnj", 8204),
    EntityRefMapEntry ("zwj", 8205),
    EntityRefMapEntry ("lrm", 8206),
    EntityRefMapEntry ("rlm", 8207),
    EntityRefMapEntry ("ndash", 0x2013),
    EntityRefMapEntry ("mdash", 0x2014),
    EntityRefMapEntry ("lsquo", 0x2018),
    EntityRefMapEntry ("rsquo", 0x2019),
    EntityRefMapEntry ("sbquo", 0x201a),
    EntityRefMapEntry ("ldquo", 0x201c),
    EntityRefMapEntry ("rdquo", 0x201d),
    EntityRefMapEntry ("bdquo", 0x201e),
    EntityRefMapEntry ("dagger", 0x2020),
    EntityRefMapEntry ("Dagger", 0x2021),
    EntityRefMapEntry ("permil", 0x2030),
    EntityRefMapEntry ("lsaquo", 0x2039),
    EntityRefMapEntry ("rsaquo", 0x203a),
};
const size_t HTMLInfo::kDefaultEntityRefMapTable_Count = NEltsOf (HTMLInfo::sDefaultEntityRefMapTable);

Led_FontSpecification::FontSize HTMLInfo::HTMLFontSizeToRealFontSize (int size)
{
    size = min (size, 7);
    size = max (size, 1);
    switch (size) {
        case 1:
            return 7;
        case 2:
            return 9;
        case 3:
            return 10;
        case 4:
            return 12;
        case 5:
            return 14;
        case 6:
            return 18;
        case 7:
            return 24;
        default:
            Assert (false);
    }
    Assert (false);
    return 12;
}

int HTMLInfo::RealFontSizeToHTMLFontSize (Led_FontSpecification::FontSize size)
{
    if (size <= 7) {
        return 1;
    }
    else if (size <= 9) {
        return 2;
    }
    else if (size <= 10) {
        return 3;
    }
    else if (size <= 12) {
        return 4;
    }
    else if (size <= 14) {
        return 5;
    }
    else if (size <= 18) {
        return 6;
    }
    else {
        return 7;
    }
}

/*
 ********************************************************************************
 ******************************* StyledTextIOReader_HTML ************************
 ********************************************************************************
 */
StyledTextIOReader_HTML::StyledTextIOReader_HTML (SrcStream* srcStream, SinkStream* sinkStream, HTMLInfo* saveHTMLInfoInto)
    : StyledTextIOReader (srcStream, sinkStream)
    , fInAPara (false)
    , fSaveHTMLInfoInto (saveHTMLInfoInto)
    , fReadingBody (false)
    , fFontStack ()
    , fComingTextIsTitle (false)
    , fNormalizeInputWhitespace (true)
    , fHTMLBaseFontSize (3)
    , fHTMLFontSize (3)
    , fLastCharSpace (true)
    , fCurAHRefStart (size_t (-1))
    , fCurAHRefText ()
    , fULNestingCount (0)
    , fLIOpen (false)
    , fTableOpenCount (0)
    , fTableRowOpen (false)
    , fTableCellOpen (false)
    , fHiddenTextMode (false)
    , fHiddenTextAccumulation ()
{
    if (fSaveHTMLInfoInto != nullptr) {
        *fSaveHTMLInfoInto = HTMLInfo (); // reset to default values before reading...
    }
    fFontStack.push_back (TextImager::GetStaticDefaultFont ());
}

void StyledTextIOReader_HTML::Read ()
{
    while (true) {
        size_t lastWroteBefore = GetSrcStream ().current_offset ();
        switch (ScanTilNextHTMLThingy ()) {
            case eEntityRef: {
                EmitText (MapInputTextToTString (GrabString (lastWroteBefore)));
                size_t curPos = GetSrcStream ().current_offset ();
                ScanTilAfterHTMLThingy (eEntityRef);
                HandleHTMLThingy (eEntityRef, GrabString (curPos));
            } break;

            case eTag: {
                EmitText (MapInputTextToTString (GrabString (lastWroteBefore)));
                size_t curPos = GetSrcStream ().current_offset ();
                ScanTilAfterHTMLThingy (eTag);
                HandleHTMLThingy (eTag, GrabString (curPos));
            } break;

            case eBangComment: {
                ScanTilAfterHTMLThingy (eBangComment);
            } break;

            case eEOF: {
                EmitText (MapInputTextToTString (GrabString (lastWroteBefore)));
                GetSinkStream ().EndOfBuffer ();
                return;
            } break;

            default: {
                Assert (false);
            } break;
        }
    }
}

bool StyledTextIOReader_HTML::QuickLookAppearsToBeRightFormat ()
{
    SrcStreamSeekSaver savePos (GetSrcStream ());

Again:
    if (GetSrcStream ().current_offset () >= 1024) {
        return false;
    }
    switch (ScanTilNextHTMLThingy ()) {
        case eEntityRef: {
            ScanTilAfterHTMLThingy (eEntityRef);
            goto Again;
        } break;

        case eTag: {
            // If the first tag appears before the first 1K, and if it is one of a set of common start
            // tags, then it is probably a valid HTML file...
            if (GetSrcStream ().current_offset () < 1024) {
                size_t curPos = GetSrcStream ().current_offset ();
                ScanTilAfterHTMLThingy (eTag);
                string tmp = GrabString (curPos);
                char   tagBuf[1024];
                bool   isStartTag = true;
                ExtractHTMLTagIntoTagNameBuf (tmp.c_str (), tmp.length (), tagBuf, sizeof (tagBuf), &isStartTag);
                if (isStartTag and
                        ::strcmp (tagBuf, "html") == 0 or
                    ::strcmp (tagBuf, "!doctype") == 0 or
                    ::strcmp (tagBuf, "title") == 0 or
                    ::strcmp (tagBuf, "head") == 0 or
                    ::strcmp (tagBuf, "body") == 0) {
                    return true;
                }
                return false;
            }
            else {
                return false;
            }
        } break;

        case eEOF: {
            return false;
        } break;

        default: {
            Assert (false);
            return false; // Not reached
        } break;
    }
    Assert (false);
    return false; // Not reached
}

/*
@METHOD:        StyledTextIOReader_HTML::GetEntityRefMapTable
@DESCRIPTION:   <p>Return a vector of @'StyledTextIOReader_HTML::EntityRefMapEntry' to be used in reading in
    HTML text. These entity refs will be recognized in the input text, and mapped to the appropriate given character.</p>
        <p>NB: The return value is a REFERENCE to vector. The lifetime of that vector must be at least til the
    next call of this function, or til the death of this object. Typically, it is assumed, a static table will be used
    so the lifetime will be the end of the process.</p>
*/
const vector<StyledTextIOReader_HTML::EntityRefMapEntry>& StyledTextIOReader_HTML::GetEntityRefMapTable () const
{
    using ELTV             = vector<StyledTextIOReader_HTML::EntityRefMapEntry>;
    static bool sFirstTime = true;
    static ELTV sResult;
    if (sFirstTime) {
        sFirstTime = false;
        sResult    = ELTV (HTMLInfo::sDefaultEntityRefMapTable, HTMLInfo::sDefaultEntityRefMapTable + HTMLInfo::kDefaultEntityRefMapTable_Count);
    }
    return sResult;
}

StyledTextIOReader_HTML::ThingyType StyledTextIOReader_HTML::ScanTilNextHTMLThingy ()
{
    try {
        // Looks like infinite loop but is not - cuz PeekNextChar () at EOF
        for (char c = PeekNextChar ();; c = PeekNextChar ()) {
            switch (c) {
                case '&': {
                    return eEntityRef;
                } break;

                case '<': {
                    if (LookingAt ("<!--")) {
                        return eBangComment;
                    }
                    else {
                        return eTag;
                    }
                } break;

                default: {
                    // ignore - plain characters
                    ConsumeNextChar ();
                } break;
            }
        }
    }
    catch (ReadEOFException& /*eof*/) {
        return eEOF;
    }
    Assert (false);
    return (eEOF); // not reached
}

void StyledTextIOReader_HTML::ScanTilAfterHTMLThingy (ThingyType thingy)
{
    Require (thingy != eEOF);
    switch (thingy) {
        case eEntityRef: {
            // should check first char is '&'
            ConsumeNextChar ();
            try {
                // read all successive alpha characters? Right way to parse entityrefs?
                // LGP 960907
                char c;
                while (isalnum (c = PeekNextChar ()) or c == '#') {
                    ConsumeNextChar ();
                }
                if (c == ';') {
                    ConsumeNextChar ();
                }
            }
            catch (ReadEOFException& /*eof*/) {
                return; // OK to have entity ref at EOF
            }
        } break;

        case eTag: {
            // Probably must be more careful about comments and quoted strings???...
            char c;
            while ((c = GetNextChar ()) != '>')
                ;
        } break;

        case eBangComment: {
            while (true) {
                if (LookingAt ("-->")) {
                    ConsumeNextChar ();
                    ConsumeNextChar ();
                    ConsumeNextChar ();
                    break;
                }
                else {
                    ConsumeNextChar ();
                }
            }
        } break;

        default: {
            Assert (false);
        } break;
    }
}

bool StyledTextIOReader_HTML::LookingAt (const char* text) const
{
    bool        result = true;
    const char* p      = text;
    for (; *p != '\0';) {
        if (PeekNextChar () == *p) {
            ConsumeNextChar ();
            ++p;
        }
        else {
            result = false;
            break;
        }
    }
    // unwind consumed chars
    size_t i = p - text;
    while (i != 0) {
        PutBackLastChar ();
        i--;
    }
    return result;
}

Led_tString StyledTextIOReader_HTML::MapInputTextToTString (const string& text)
{
#if qWideCharacters
    Memory::SmallStackBuffer<Led_tChar> wBuf (text.length () + 1);
    CodePageConverter                   cpc (kCodePage_ANSI);
    size_t                              outCharCnt = text.length ();
    cpc.MapToUNICODE (text.c_str (), text.length (), wBuf, &outCharCnt);
    wBuf[outCharCnt] = '\0';
    return Led_tString (wBuf);
#else
    return text;
#endif
}

void StyledTextIOReader_HTML::EmitText (const Led_tChar* text, size_t nBytes, bool skipNLCheck)
{
    if (fComingTextIsTitle) {
        if (fSaveHTMLInfoInto != nullptr) {
            fSaveHTMLInfoInto->fTitle += Led_tString (text, nBytes);
        }
        return;
    }
    if (fHiddenTextMode) {
        fHiddenTextAccumulation += Led_tString (text, nBytes);
        return;
    }

    if (fULNestingCount > 0 and not fLIOpen) {
        // Drop that text on the floor
        return;
    }

    Memory::SmallStackBuffer<Led_tChar> outBuf (nBytes);
    nBytes = Characters::NormalizeTextToNL<Led_tChar> (text, nBytes, outBuf, nBytes);

    if (not skipNLCheck and fNormalizeInputWhitespace) {
        Assert (fNormalizeInputWhitespace);
        Assert (not skipNLCheck);

        // Normalize space (including NLs) to one
        Led_tChar* out = outBuf;
        for (size_t i = 0; i < nBytes; i++) {
            Led_tChar c             = outBuf[i];
            bool      thisCharSpace = IsASCIISpace (c);
            if (thisCharSpace) {
                if (not fLastCharSpace) {
                    *out = ' ';
                    out++;
                }
            }
            else {
                *out = c;
                out++;
            }
            fLastCharSpace = thisCharSpace;
        }
        size_t newNBytes = out - static_cast<Led_tChar*> (outBuf);
        Assert (newNBytes <= nBytes);
        nBytes = newNBytes;
    }

    if (nBytes > 0) {
        fReadingBody = true;
        GetSinkStream ().AppendText (outBuf, nBytes, &fFontStack.back ());
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingy (StyledTextIOReader_HTML::ThingyType thingy, const char* text, size_t nBytes)
{
    switch (thingy) {
        case eEntityRef:
            HandleHTMLThingy_EntityReference (text, nBytes);
            break;
        case eTag:
            HandleHTMLThingy_Tag (text, nBytes);
            break;
        case eEOF:
            break;
        default:
            Assert (false);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingy_EntityReference (const char* text, size_t nBytes)
{
    RequireNotNull (text);
    Require (nBytes >= 1);

    // For understood entity references, emit the character. For others, just emit the original enity ref..
    string refName = string (&text[1], nBytes - 1);
    if (refName.length () > 0) {
        if (refName[refName.length () - 1] == ';') {
            refName.erase (refName.length () - 1);
        }
    }
    if (refName.length () > 0) {
#if !qWideCharacter
#if qPlatform_MacOS
        const CodePage kInternalCodePageToMapTo = kCodePage_MAC;
#else
        const CodePage kInternalCodePageToMapTo = kCodePage_ANSI;
#endif
#endif
        if (refName[0] == '#') {
            wchar_t result = static_cast<wchar_t> (::atoi (refName.c_str () + 1));
#if qWideCharacters
            EmitText (&result, 1);
#else
            CodePageConverter              cpc (kInternalCodePageToMapTo);
            size_t                         outCharCnt = cpc.MapFromUNICODE_QuickComputeOutBufSize (&result, 1);
            Memory::SmallStackBuffer<char> buf (outCharCnt);
            cpc.MapFromUNICODE (&result, 1, buf, &outCharCnt);
            EmitText (buf, outCharCnt);
#endif
            return;
        }
        else {
            const vector<StyledTextIOReader_HTML::EntityRefMapEntry>& entityRefs = GetEntityRefMapTable ();
            for (auto i = entityRefs.begin (); i != entityRefs.end (); ++i) {
                if (refName == (*i).fEntityRefName) {
#if qWideCharacters
                    EmitText (&(*i).fCharValue, 1);
#else
                    CodePageConverter              cpc (kInternalCodePageToMapTo);
                    size_t                         outCharCnt = cpc.MapFromUNICODE_QuickComputeOutBufSize (&(*i).fCharValue, 1);
                    Memory::SmallStackBuffer<char> buf (outCharCnt);
                    cpc.MapFromUNICODE (&(*i).fCharValue, 1, buf, &outCharCnt);
                    EmitText (buf, outCharCnt);
#endif
                    return;
                }
            }
        }
    }

    // Even if qThrowAwayMostUnknownHTMLTags, we should still emit unknown entity refs, I think ... LGP 961015
    EmitText (MapInputTextToTString (string (text, nBytes)));
}

void StyledTextIOReader_HTML::HandleHTMLThingy_Tag (const char* text, size_t nBytes)
{
    RequireNotNull (text);

    char tagBuf[1024];
    bool isStartTag = true;

    ExtractHTMLTagIntoTagNameBuf (text, nBytes, tagBuf, sizeof (tagBuf), &isStartTag);

    if (0) {
    }
    else if (::strcmp (tagBuf, "!doctype") == 0) {
        HandleHTMLThingyTag_BANG_doctype (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "a") == 0) {
        HandleHTMLThingyTag_a (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "b") == 0) {
        HandleHTMLThingyTag_b (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "basefont") == 0) {
        HandleHTMLThingyTag_basefont (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "blockquote") == 0) {
        HandleHTMLThingyTag_blockquote (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "br") == 0) {
        HandleHTMLThingyTag_br (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "body") == 0) {
        HandleHTMLThingyTag_body (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "code") == 0) {
        HandleHTMLThingyTag_code (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "comment") == 0) {
        HandleHTMLThingyTag_comment (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "dir") == 0) {
        HandleHTMLThingyTag_dir (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "div") == 0) {
        HandleHTMLThingyTag_div (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "em") == 0) {
        HandleHTMLThingyTag_em (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "font") == 0) {
        HandleHTMLThingyTag_font (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "head") == 0) {
        HandleHTMLThingyTag_head (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "html") == 0) {
        HandleHTMLThingyTag_html (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "hr") == 0) {
        HandleHTMLThingyTag_hr (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h1") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h2") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h3") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h4") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h5") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h6") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h7") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h8") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "h9") == 0) {
        HandleHTMLThingyTag_hN (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "i") == 0) {
        HandleHTMLThingyTag_i (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "img") == 0) {
        HandleHTMLThingyTag_img (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "li") == 0) {
        HandleHTMLThingyTag_li (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "listing") == 0) {
        HandleHTMLThingyTag_listing (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "ol") == 0) {
        HandleHTMLThingyTag_ol (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "p") == 0) {
        HandleHTMLThingyTag_p (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "plaintext") == 0) {
        HandleHTMLThingyTag_plaintext (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "pre") == 0) {
        HandleHTMLThingyTag_pre (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "s") == 0) {
        HandleHTMLThingyTag_s (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "samp") == 0) {
        HandleHTMLThingyTag_samp (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "small") == 0) {
        HandleHTMLThingyTag_small (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "span") == 0) {
        HandleHTMLThingyTag_span (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "strike") == 0) {
        HandleHTMLThingyTag_strike (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "strong") == 0) {
        HandleHTMLThingyTag_strong (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "sub") == 0) {
        HandleHTMLThingyTag_sub (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "sup") == 0) {
        HandleHTMLThingyTag_sup (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "table") == 0) {
        HandleHTMLThingyTag_table (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "td") == 0) {
        HandleHTMLThingyTag_td (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "th") == 0) {
        HandleHTMLThingyTag_th (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "title") == 0) {
        HandleHTMLThingyTag_title (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "tr") == 0) {
        HandleHTMLThingyTag_tr (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "tt") == 0) {
        HandleHTMLThingyTag_tt (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "u") == 0) {
        HandleHTMLThingyTag_u (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "ul") == 0) {
        HandleHTMLThingyTag_ul (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "var") == 0) {
        HandleHTMLThingyTag_var (isStartTag, text, nBytes);
    }
    else if (::strcmp (tagBuf, "xmp") == 0) {
        HandleHTMLThingyTag_xmp (isStartTag, text, nBytes);
    }
    else {
        HandleHTMLThingyTagUnknown (isStartTag, text, nBytes);
    }
}

void StyledTextIOReader_HTML::ExtractHTMLTagIntoTagNameBuf (const char* text, size_t nBytes, char* tagBuf, size_t tagBufSize, bool* isStartTag)
{
    RequireNotNull (text);
    RequireNotNull (tagBuf);
    RequireNotNull (isStartTag);

    {
        const char* in     = &text[0];
        const char* in_end = in + nBytes;
        char*       out    = &tagBuf[0];
        in++; // skip '<'
        if (*in == '/') {
            in++; // skip endTag character, and set flag
            *isStartTag = false;
        }
        for (; in < in_end; in++) {
            char c = *in;
            if (isalnum (c) or (c == '!') or (c == '_') or (c == '-')) {
                if (isupper (c)) {
                    c = static_cast<char> (tolower (c));
                }
                *out = c;
                out++;
            }
            else {
                break;
            }

            if (size_t (out - tagBuf) >= tagBufSize - 2) {
                break;
            }
        }
        *out = '\0';
    }
}

Led_IncrementalFontSpecification StyledTextIOReader_HTML::ExtractFontSpecFromCSSStyleAttribute (const char* text, size_t nBytes)
{
    Led_IncrementalFontSpecification f;
    string                           fullCSSString (text, text + nBytes);
    string                           itemValue;
    if (ParseCSSTagArgOut (fullCSSString, "font-family", &itemValue)) {
        // really not right - could be comma separated list - just grab first and ignore the rest, and map
        // predefined generic family items (serif => Times, etc... -- see http://www.w3.org/TR/1999/REC-CSS1-19990111
        size_t comma = itemValue.find ('.');
        if (comma != string::npos) {
            itemValue = itemValue.substr (0, comma);
        }
        f.SetFontName (Led_ANSI2SDKString (itemValue));
    }
    if (ParseCSSTagArgOut (fullCSSString, "font-size", &itemValue)) {
        if (itemValue.length () >= 3 and
            Led_CasedStringsEqual (itemValue.substr (itemValue.length () - 2, 2), "pt")) {
            int sizeVal = 0;
            if (ParseStringToInt_ (itemValue.substr (0, itemValue.length () - 2), &sizeVal) and
                sizeVal >= 2 and sizeVal <= 128) {
                f.SetPointSize (static_cast<uint8_t> (sizeVal)); // cast OK cuz sizeVal in range
            }
        }
    }
    if (ParseCSSTagArgOut (fullCSSString, "font-style", &itemValue)) {
        if (Led_CasedStringsEqual (itemValue, "italic")) {
            f.SetStyle_Italic (true);
        }
        else if (Led_CasedStringsEqual (itemValue, "normal")) {
            f.SetStyle_Italic (false);
        }
    }
    if (ParseCSSTagArgOut (fullCSSString, "font-weight", &itemValue)) {
        int bv = 0;
        if (Led_CasedStringsEqual (itemValue, "bold") or
            Led_CasedStringsEqual (itemValue, "bolder") or
            (itemValue.length () > 0 and isdigit (itemValue[0]) and ParseStringToInt_ (itemValue, &bv) and bv >= 400)) {
            f.SetStyle_Bold (true);
        }
        else {
            f.SetStyle_Bold (false);
        }
    }
    if (ParseCSSTagArgOut (fullCSSString, "color", &itemValue)) {
        Led_Color clr = Led_Color::kBlack;
        if (ParseColorString (itemValue, &clr)) {
            f.SetTextColor (clr);
        }
    }
    return f;
}

void StyledTextIOReader_HTML::ApplyCSSStyleAttributeToCurrentFontStack (const char* text, size_t nBytes)
{
    fFontStack.back ().MergeIn (ExtractFontSpecFromCSSStyleAttribute (text, nBytes));
}

void StyledTextIOReader_HTML::GrabAndApplyCSSStyleFromTagText (const char* text, size_t nBytes)
{
    if (nBytes >= 11) { // speed check - smallest tag case would be "<a style=a>"
        string tagText (text, nBytes);
        string tagValue;
        if (ParseHTMLTagArgOut (tagText, "style", &tagValue)) {
            ApplyCSSStyleAttributeToCurrentFontStack (tagValue.c_str (), tagValue.length ());
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_BANG_doctype (bool /*start*/, const char* text, size_t nBytes)
{
    if (fSaveHTMLInfoInto != nullptr) {
        fSaveHTMLInfoInto->fDocTypeTag = string (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_a (bool start, const char* text, size_t nBytes)
{
#if qThrowAwayMostUnknownHTMLTags
    if (start) {
        string tagText (text, nBytes);

        fCurAHRefStart          = GetSrcStream ().current_offset ();
        fCurAHRefText           = tagText;
        fHiddenTextMode         = true;
        fHiddenTextAccumulation = Led_tString ();
    }
    else {
        if (fCurAHRefStart != size_t (-1)) {
            string tagValue;
            (void)ParseHTMLTagArgOut (fCurAHRefText, "href", &tagValue);
            EmbeddedObjectCreatorRegistry::Assoc assoc;
            if (EmbeddedObjectCreatorRegistry::Get ().Lookup (StandardURLStyleMarker::kEmbeddingTag, &assoc)) {
                AssertNotNull (assoc.fReadFromMemory);
#if qWideCharacters
                CodePageConverter              cpc (kCodePage_ANSI);
                size_t                         outCharCnt = cpc.MapFromUNICODE_QuickComputeOutBufSize (fHiddenTextAccumulation.c_str (), fHiddenTextAccumulation.length ());
                Memory::SmallStackBuffer<char> buf (outCharCnt);
                cpc.MapFromUNICODE (fHiddenTextAccumulation.c_str (), fHiddenTextAccumulation.length (), buf, &outCharCnt);
                buf[outCharCnt] = '\0';
                Led_URLD urld   = Led_URLD (tagValue.c_str (), buf);
#else
                Led_URLD urld = Led_URLD (tagValue.c_str (), fHiddenTextAccumulation.c_str ());
#endif
                GetSinkStream ().AppendEmbedding ((assoc.fReadFromMemory) (StandardURLStyleMarker::kEmbeddingTag, urld.PeekAtURLD (), urld.GetURLDLength ()));
            }
        }
        fCurAHRefStart          = size_t (-1);
        fCurAHRefText           = string ();
        fHiddenTextMode         = false;
        fHiddenTextAccumulation = Led_tString ();
        fLastCharSpace          = false;
    }
#else
    HandleHTMLThingyTagUnknown (start, text, nBytes);
#endif
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_b (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_Bold (true);
        fFontStack.back () = fsp;
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_basefont (bool start, const char* text, size_t nBytes)
{
    if (start) {
        // we don't bother restoring old value for this on a close. Should we keep a stack??? - LGP 961014
        string tagText (text, nBytes);
        string tagValue;
        if (ParseHTMLTagArgOut (tagText, "size", &tagValue)) {
            if (tagValue.length () > 0) {
                int tagNum = Led_DigitCharToNumber (tagValue[0]);
                if (tagNum >= 1 and tagNum <= 7) {
                    fHTMLBaseFontSize = tagNum;
                }
            }
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_body (bool start, const char* text, size_t nBytes)
{
    if (start and fSaveHTMLInfoInto != nullptr) {
        fSaveHTMLInfoInto->fStartBodyTag = string (text, nBytes);
    }
    fReadingBody = true;
    if (start) {
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_big (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        SetHTMLFontSize (fHTMLFontSize + 1);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_blockquote (bool start, const char* text, size_t nBytes)
{
    // NB: This ignores the other attributes, like centering, etc...

    // Probably should add a routine to only force a break UNLESS we just emitted a NL. This is really sloppy.
    EmitForcedLineBreak ();

    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);

    // ALSO, most handle NEWLINES specially in this context. Note this won't work if tags are nested, which
    // I doubt they are very often (and I doubt its legal). But maybe should add context stack?
    fNormalizeInputWhitespace = not start;
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_br (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    // This tag should only HAVE a start version (no </br>).
    if (start) {
        if (fComingTextIsTitle or fHiddenTextMode) {
            // IGNORE
        }
        else {
            GetSinkStream ().AppendSoftLineBreak ();
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_code (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_comment (bool start, const char* text, size_t nBytes)
{
    HandleHTMLThingyTagUnknown (start, text, nBytes);

    // ALSO, should handle NEWLINES in this context. Note this won't work if tags are nested, which
    // I doubt they are very often (and I doubt its legal). But maybe should add context stack?
    fNormalizeInputWhitespace = not start;
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_dir (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    // According to HTML 4.0 spec, these are now deprecated, but should be treated just as a UL, when encountered.
    HandleHTMLThingyTag_ul (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_div (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    BasicFontStackOperation (start);
    if (start) {
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
        string tagText (text, nBytes);

        // SOON FIX THIS !@!!!
        // Figure out what justification to set and turn it on
        if (tagText.find ("left") != -1) {
            GetSinkStream ().SetJustification (eLeftJustify);
        }
        else if (tagText.find ("center") != -1) {
            GetSinkStream ().SetJustification (eCenterJustify);
        }
        else if (tagText.find ("right") != -1) {
            GetSinkStream ().SetJustification (eRightJustify);
        }
    }
    else {
        EmitForcedLineBreak (); // So we have at least ONE LINE above to apply the above justication to - else we can easily end up with NOTHING having the above justification (SPR#0707)
        // Set left justification by default. Probably should be maintaining a STACK of justifications -
        // and restoring it here. But this will frequently be good enough. And good enuf for now til I rewrite
        // the entire HTML reader...
        GetSinkStream ().SetJustification (eLeftJustify);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_em (bool start, const char* text, size_t nBytes)
{
    // by default, handle this same as italics
    StyledTextIOReader_HTML::HandleHTMLThingyTag_i (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_font (bool start, const char* text, size_t nBytes)
{
    BasicFontStackOperation (start);
    if (start) {
        // try to parse out SIZE= field and/or FACE=
        string tagText (text, nBytes);
        string tagValue;
        if (ParseHTMLTagArgOut (tagText, "face", &tagValue)) {
            Led_FontSpecification fsp = fFontStack.back ();
            if (tagValue.find (',') != string::npos) {
                tagValue = tagValue.substr (0, tagValue.find (','));
            }
            fsp.SetFontName (Led_ANSI2SDKString (tagValue));
#if qPlatform_MacOS
            // cuz the mac does so badly on this, and just sets the font to chicago if its unknown, and cuz
            // this looks so bad, try a nicer default font...
            {
                short fontID;
                short fontSize;
                Style fontStyle;
                fsp.GetOSRep (&fontID, &fontSize, &fontStyle);
                if (fontID == 0 and not Led_CasedStringsEqual (tagValue, "Chicago")) {
                    fsp.SetFontNameSpecifier (applFont); // a much more pleasing default
                }
            }
#endif
            fFontStack.back () = fsp;
        }
        if (ParseHTMLTagArgOut (tagText, "color", &tagValue)) {
            Led_Color newColor = Led_Color::kBlack;
            if (ParseColorString (tagValue, &newColor)) {
                Led_FontSpecification fsp = fFontStack.back ();
                fsp.SetTextColor (newColor);
                fFontStack.back () = fsp;
            }
        }
        if (ParseHTMLTagArgOut (tagText, "size", &tagValue)) {
            if (tagValue.length () > 0) {
                switch (tagValue[0]) {
                    case '-': {
                        if (tagValue.length () >= 2) {
                            int tagNum = tagValue[1] - '0';
                            if (tagNum >= -7 and tagNum <= 7) {
                                SetHTMLFontSize (fHTMLBaseFontSize - tagNum);
                            }
                        }
                    } break;
                    case '+': {
                        if (tagValue.length () >= 2) {
                            int tagNum = tagValue[1] - '0';
                            if (tagNum >= -7 and tagNum <= 7) {
                                SetHTMLFontSize (fHTMLBaseFontSize + tagNum);
                            }
                        }
                    } break;
                    default: {
                        if (isdigit (tagValue[0])) {
                            int tagNum = Led_DigitCharToNumber (tagValue[0]);
                            if (tagNum >= 1 and tagNum <= 7) {
                                SetHTMLFontSize (tagNum);
                            }
                        }
                    } break;
                }
            }
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_head (bool start, const char* text, size_t nBytes)
{
    if (start and fSaveHTMLInfoInto != nullptr) {
        fSaveHTMLInfoInto->fHeadTag = string (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_html (bool start, const char* text, size_t nBytes)
{
    if (start and fSaveHTMLInfoInto != nullptr) {
        fSaveHTMLInfoInto->fHTMLTag = string (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_hr ([[maybe_unused]] bool start, [[maybe_unused]] const char* text, [[maybe_unused]] size_t nBytes)
{
    EndParaIfOpen ();
// emit it unchanged (since we don't support these), but also emit a line-break as well so
// looks more readable
#if qThrowAwayMostUnknownHTMLTags
    const Led_tChar kSeparator[] = LED_TCHAR_OF ("----------------------------------------------------------------");
    EmitText (kSeparator, Led_tStrlen (kSeparator));
#else
    HandleHTMLThingyTagUnknown (start, text, nBytes);
#endif
    EmitForcedLineBreak ();
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_hN (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    if (start) {
        EmitForcedLineBreak ();
    }
#if qThrowAwayMostUnknownHTMLTags
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_Bold (true);
        fFontStack.back () = fsp;
        if (nBytes > 3 and isdigit (text[2])) {
            int headingLevel = Led_DigitCharToNumber (text[2]);
            switch (headingLevel) {
                case 1:
                    SetHTMLFontSize (7);
                    break;
                case 2:
                    SetHTMLFontSize (6);
                    break;
                case 3:
                    SetHTMLFontSize (5);
                    break;
                case 4:
                    SetHTMLFontSize (4);
                    break;
                case 5:
                    SetHTMLFontSize (3);
                    break;
                default: {
                    // ignored
                }
            }
        }
    }
#endif
    if (start) {
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
    // emit it unchanged (since we don't support these), but also emit a line-break as well so
    // looks more readable
    HandleHTMLThingyTagUnknown (start, text, nBytes);
    if (not start) {
        EmitForcedLineBreak ();
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_i (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_Italic (true);
        fFontStack.back () = fsp;
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_img (bool start, const char* text, size_t nBytes)
{
    HandleHTMLThingyTagUnknown (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_li (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    BasicFontStackOperation (start);
    if (start) {
        fLastCharSpace = true;
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
    else {
        EmitForcedLineBreak ();
    }
    fLIOpen = start;
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_listing (bool start, const char* text, size_t nBytes)
{
    // by default, handle this same as 'tt' - or 'teletype'
    EndParaIfOpen ();
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_ol (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    // emit it unchanged (since we don't support these), but also emit a line-break as well so
    // looks more readable
    HandleHTMLThingyTagUnknown (start, text, nBytes);
    EmitForcedLineBreak ();
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_p (bool start, const char* text, size_t nBytes)
{
    if (start) {
        StartPara ();
        if (start) {
            GrabAndApplyCSSStyleFromTagText (text, nBytes);
        }
        if (nBytes > 5) {
            string tagTxt (text, nBytes);
            // Figure out what justification to set and turn it on
            if (tagTxt.find ("left") != -1) {
                GetSinkStream ().SetJustification (eLeftJustify);
            }
            else if (tagTxt.find ("center") != -1) {
                GetSinkStream ().SetJustification (eCenterJustify);
            }
            else if (tagTxt.find ("right") != -1) {
                GetSinkStream ().SetJustification (eRightJustify);
            }
            else {
                // Unclear if I should treat this as inherit from someplace - or just a specification of LEFT? Docs seem to indicate left (HTML DOCS)
                GetSinkStream ().SetJustification (eLeftJustify);
            }
        }
    }
    else {
        EndParaIfOpen ();
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_plaintext (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);

    // ALSO, most handle NEWLINES specially in this context. Note this won't work if <pre> tags are nested, which
    // I doubt they are very often (and I doubt its legal). But maybe should add context stack?
    fNormalizeInputWhitespace = not start;

    // ALSO should disable other tag processing, but this ISNT implemented here... LGP 961003
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_pre (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);

    // ALSO, most handle NEWLINES specially in this context. Note this won't work if <pre> tags are nested, which
    // I doubt they are very often (and I doubt its legal). But maybe should add context stack?
    fNormalizeInputWhitespace = not start;
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_s (bool start, const char* text, size_t nBytes)
{
    HandleHTMLThingyTag_strike (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_samp (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        SetHTMLFontSize (2);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_small (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        SetHTMLFontSize (fHTMLFontSize - 1);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_span (bool start, const char* text, size_t nBytes)
{
    BasicFontStackOperation (start);
    if (start) {
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_strike ([[maybe_unused]] bool start, [[maybe_unused]] const char* text, [[maybe_unused]] size_t nBytes)
{
#if qPlatform_Windows
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_Strikeout (true);
        fFontStack.back () = fsp;
    }
#endif
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_strong (bool start, const char* text, size_t nBytes)
{
    // by default, handle this same as 'b' - BOLD
    StyledTextIOReader_HTML::HandleHTMLThingyTag_b (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_sub (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_SubOrSuperScript (Led_FontSpecification::eSubscript);
        fFontStack.back () = fsp;
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_sup (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_SubOrSuperScript (Led_FontSpecification::eSuperscript);
        fFontStack.back () = fsp;
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_table (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    GetSinkStream ().Flush ();
    fLastCharSpace = true;
    /*
     *  MINIMAL nested table support.
     *
     *      Since in order for tables to be nested, you must start a table while already inside a table cell,
     *  we can avoid keeping track of some information on a stack (cheap trick).
     *
     *      If we are starting a table, (even if nested) - we set the row/cell flags to FALSE. Then - any time we
     *  end a table (with count>1), we set them to TRUE (since to have been a nested table, it must have been
     *  contained in some open table row/cell.
     */
    if (start) {
        GetSinkStream ().StartTable ();
        fTableOpenCount++;
        fTableRowOpen  = false;
        fTableCellOpen = false;
    }
    else {
        if (fTableOpenCount > 0) {
            if (fTableCellOpen) {
                // must implicitly close previous cell
                GetSinkStream ().EndTableCell ();
                fTableCellOpen = false;
                BasicFontStackOperation (fTableCellOpen);
            }
            if (fTableRowOpen) {
                // must implicitly close previous row
                GetSinkStream ().EndTableRow ();
                fTableRowOpen = false;
            }
            GetSinkStream ().EndTable ();
            fTableOpenCount--;
            // if tableRowCount is now > 0 - we must have ended table inside an existing cell. If == 0, doesn't matter
            // as values are meaningless.
            fTableRowOpen  = true;
            fTableCellOpen = true;
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_td (bool start, const char* text, size_t nBytes)
{
    // Ignore td calls if not in a table or row.
    if (fTableOpenCount > 0 and fTableRowOpen) {
        GetSinkStream ().Flush ();
        if (start) {
            if (fTableCellOpen) {
                // must implicitly close previous cell
                GetSinkStream ().EndTableCell ();
                fTableCellOpen = false;
                BasicFontStackOperation (fTableCellOpen);
            }
            GetSinkStream ().StartTableCell (1);
            fTableCellOpen = true;
            BasicFontStackOperation (fTableCellOpen);
            GrabAndApplyCSSStyleFromTagText (text, nBytes);
        }
        else {
            // simply ignore a close cell command if we're not already in a table cell.
            if (fTableCellOpen) {
                GetSinkStream ().EndTableCell ();
                fTableCellOpen = false;
                BasicFontStackOperation (fTableCellOpen);
            }
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_th (bool start, const char* text, size_t nBytes)
{
    // Treat a th as identical to a td element (maybe later force text to bold too)
    HandleHTMLThingyTag_td (start, text, nBytes);
    if (start) {
        fFontStack.back ().SetStyle_Bold (true);
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_title (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    fComingTextIsTitle = start;
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_tr (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    // Ignore (largely) tr calls if not in a table.
    if (fTableOpenCount > 0) {
        GetSinkStream ().Flush ();

        if (fTableCellOpen) {
            // must implicitly close previous cell
            GetSinkStream ().EndTableCell ();
            fTableCellOpen = false;
            BasicFontStackOperation (fTableCellOpen);
        }
        if (fTableRowOpen) {
            // must implicitly close previous row
            GetSinkStream ().EndTableRow ();
            fTableRowOpen = false;
        }

        if (start) {
            GetSinkStream ().StartTableRow ();
            fTableRowOpen = true;
        }
    }
    else {
        // slight hack --- probably best we can do for ill-formed input HTML
        EndParaIfOpen ();
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (bool start, const char* text, size_t nBytes)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetFontName (Led_SDK_TCHAROF ("courier")); // will this work on mac and PC? - want teletype, monospace font here! - LGP 961003
        fFontStack.back () = fsp;
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_u (bool start, const char* /*text*/, size_t /*nBytes*/)
{
    BasicFontStackOperation (start);
    if (start) {
        Led_FontSpecification fsp = fFontStack.back ();
        fsp.SetStyle_Underline (true);
        fFontStack.back () = fsp;
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_ul (bool start, const char* text, size_t nBytes)
{
    EndParaIfOpen ();
    if (start) {
        fULNestingCount++;
        EmitForcedLineBreak ();
        ListStyle listStyle = eListStyle_Bullet; // should get style from param to UL - as in 'type=disc'
        GetSinkStream ().SetListStyle (listStyle);
        GrabAndApplyCSSStyleFromTagText (text, nBytes);
    }
    else {
        fULNestingCount--;
        if (fULNestingCount == 0) {
            EmitForcedLineBreak ();
            ListStyle listStyle = eListStyle_None; // should get style from param to saved-stack...
            GetSinkStream ().SetListStyle (listStyle);
        }
    }
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_var (bool start, const char* text, size_t nBytes)
{
    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTag_xmp (bool start, const char* text, size_t nBytes)
{
    // by default, handle this same as 'tt' - or 'teletype'
    StyledTextIOReader_HTML::HandleHTMLThingyTag_tt (start, text, nBytes);
}

void StyledTextIOReader_HTML::HandleHTMLThingyTagUnknown ([[maybe_unused]] bool start, [[maybe_unused]] const char* text, [[maybe_unused]] size_t nBytes)
{
#if !qThrowAwayMostUnknownHTMLTags
    EmitText (text, nBytes);
#endif
}

void StyledTextIOReader_HTML::EmitForcedLineBreak ()
{
    EmitText (LED_TCHAR_OF ("\n"), 1, true);
}

void StyledTextIOReader_HTML::BasicFontStackOperation (bool start)
{
    if (start) {
        fFontStack.push_back (fFontStack.back ());
    }
    else {
        // Assume end-font-setting-tag matches earlier start-font-setting-tag. This is RIGHT for valid HTML files, and
        // I'm not that worried about properly dealing with bogus files
        if (fFontStack.size () > 1) {
            fFontStack.pop_back ();
        }
    }
}

bool StyledTextIOReader_HTML::ParseHTMLTagArgOut (const string& tagText, const string& attrName, string* attrValue)
{
    /*
     *  Really primitive, half-assed implementation. But good enuf for my current usage.
     *      --LGP 961014
     *  Slighyly improved as part of SPR#1504 and SPR#0574 and SPR#1505.
     */
    size_t len = tagText.length ();
    size_t i   = 0;
    string tagName;

    if (i == 0 and tagText[i] == '<') {
        ++i;
    } // SKIP leading <
    while (i < len and isspace (tagText[i])) {
        ++i;
    } // SKIP WHITESPACE

    // grab tag name
    {
        size_t startOfArgName = i;
        while (isalpha (tagText[i])) {
            i++;
            if (i >= len) {
                return false;
            }
        }
        tagName = string (tagText.c_str () + startOfArgName, (i - startOfArgName));
    }

    // Check each attr
    for (; i < len; i++) {
        while (i < len and isspace (tagText[i])) {
            ++i;
        } // SKIP WHITESPACE

        size_t startOfArgName = i;
        while (isalpha (tagText[i])) {
            i++;
            if (i >= len) {
                return false;
            }
        }
        string argName (tagText.c_str () + startOfArgName, (i - startOfArgName));

        while (i < len and isspace (tagText[i])) {
            ++i;
        } // SKIP WHITESPACE

        // Look for '=' in a=b (or a='b')
        if (i < len and tagText[i] == '=') {
            i++;
        }
        else {
            return false;
        }

        while (i < len and isspace (tagText[i])) {
            ++i;
        } // SKIP WHITESPACE

        bool gotStartQuote = false;
        char startQuote    = '\0';
        if (i < len) {
            if (tagText[i] == '\'' or tagText[i] == '\"') {
                gotStartQuote = true;
                startQuote    = tagText[i];
                ++i;
            }
        }

        size_t startOfArgValue = i;
        while ((not isspace (tagText[i]) or gotStartQuote) and tagText[i] != startQuote and tagText[i] != '>') {
            i++;
            if (i >= len) {
                break;
            }
        }
        string argValue (tagText.c_str () + startOfArgValue, (i - startOfArgValue));
        if (Led_CasedStringsEqual (attrName, argName)) {
            *attrValue = argValue;
            return true;
        }
    }

    return false;
}

bool StyledTextIOReader_HTML::ParseCSSTagArgOut (const string& text, const string& attrName, string* attrValue)
{
    size_t len = text.length ();

    for (size_t i = 0; i < len; i++) {
        while (i < len and isspace (text[i]) or (text[i] == ';')) {
            ++i;
        } // SKIP WHITESPACE (or semi-colon - attribute separator)

        string argName;
        {
            size_t startOfArgName = i;
            while (isalpha (text[i]) or text[i] == '-') {
                i++;
                if (i >= len) {
                    return false;
                }
            }
            argName = string (text.c_str () + startOfArgName, (i - startOfArgName));
        }

        while (i < len and isspace (text[i])) {
            ++i;
        } // SKIP WHITESPACE

        // Look for ':' in a:b (or a:'b' etc)
        if (i < len and text[i] == ':') {
            i++;
        }
        else {
            return false;
        }

        while (i < len and isspace (text[i])) {
            ++i;
        } // SKIP WHITESPACE

        char startQuote = '\0';
        if (i < len) {
            if (text[i] == '\'' or text[i] == '\"') {
                startQuote = text[i];
                ++i;
            }
        }

        {
            size_t startOfArgValue = i;
            while (text[i] != ';') {
                if (text[i] == startQuote) {
                    break;
                }
                i++;
                if (i >= len) {
                    break;
                }
            }
            string argValue (text.c_str () + startOfArgValue, (i - startOfArgValue));
            if (Led_CasedStringsEqual (attrName, argName)) {
                *attrValue = argValue;
                return true;
            }
        }
    }
    return false;
}

void StyledTextIOReader_HTML::StartPara ()
{
    EndParaIfOpen ();
    Assert (not fInAPara);
    fInAPara = true;
}

void StyledTextIOReader_HTML::EndParaIfOpen ()
{
    if (fInAPara) {
        EmitForcedLineBreak ();
        fInAPara       = false;
        fLastCharSpace = true;
        GetSinkStream ().SetJustification (eLeftJustify); // unclear if this is needed or desirable?
    }
    Ensure (not fInAPara);
}

void StyledTextIOReader_HTML::SetHTMLFontSize (int to)
{
    fHTMLFontSize             = min (to, 7);
    fHTMLFontSize             = max (fHTMLFontSize, 1);
    Led_FontSpecification fsp = fFontStack.back ();
    fsp.SetPointSize (HTMLInfo::HTMLFontSizeToRealFontSize (fHTMLFontSize));
    fFontStack.back () = fsp;
}

/*
 ********************************************************************************
 ******************* StyledTextIOWriter_HTML::WriterContext *********************
 ********************************************************************************
 */

size_t StyledTextIOWriter_HTML::WriterContext::GetCurSrcOffset () const
{
    return GetSrcStream ().current_offset ();
}

SimpleEmbeddedObjectStyleMarker* StyledTextIOWriter_HTML::WriterContext::GetCurSimpleEmbeddedObjectStyleMarker () const
{
    size_t                                   offset         = GetCurSrcOffset ();
    vector<SimpleEmbeddedObjectStyleMarker*> embeddingsList = GetSrcStream ().CollectAllEmbeddingMarkersInRange (offset - 1, offset);
    Assert (embeddingsList.size () <= 1); // cuz we gave a range of one, and can only have a single
    // embedding in one place. Allow for there to be NONE - if the user
    // wants to allow having NUL characters in his text for other reasons.
    if (embeddingsList.empty ()) {
        return nullptr;
    }
    else {
        return embeddingsList[0];
    }
}

/*
 ********************************************************************************
 ************************** StyledTextIOWriter_HTML *****************************
 ********************************************************************************
 */
StyledTextIOWriter_HTML::StyledTextIOWriter_HTML (SrcStream* srcStream, SinkStream* sinkStream, const HTMLInfo* getHTMLInfoFrom)
    : StyledTextIOWriter (srcStream, sinkStream)
    , fGetHTMLInfoFrom (getHTMLInfoFrom)
    , fStyleRunSummary ()
    , fSoftLineBreakChar (srcStream->GetSoftLineBreakCharacter ())
{
}

StyledTextIOWriter_HTML::~StyledTextIOWriter_HTML ()
{
}

/*
@METHOD:        StyledTextIOReader_HTML::GetEntityRefMapTable
@DESCRIPTION:   <p>Return a vector of @'StyledTextIOReader_HTML::EntityRefMapEntry' to be used in reading in
    HTML text. These entity refs will be recognized in the input text, and mapped to the appropriate given character.</p>
        <p>NB: The return value is a REFERENCE to vector. The lifetime of that vector must be at least til the
    next call of this function, or til the death of this object. Typically, it is assumed, a static table will be used
    so the lifetime will be the end of the process.</p>
*/
const vector<StyledTextIOWriter_HTML::EntityRefMapEntry>& StyledTextIOWriter_HTML::GetEntityRefMapTable () const
{
    static bool                                               sFirstTime = true;
    static vector<StyledTextIOReader_HTML::EntityRefMapEntry> sResult;
    if (sFirstTime) {
        sFirstTime = false;
        for (size_t i = 0; i < HTMLInfo::kDefaultEntityRefMapTable_Count; ++i) {
#if !qWriteOutMostHTMLEntitiesByName
            // Then skip all but a handfull of very important ones...
            if (
                HTMLInfo::sDefaultEntityRefMapTable[i].fEntityRefName != "amp" and
                HTMLInfo::sDefaultEntityRefMapTable[i].fEntityRefName != "gt" and
                HTMLInfo::sDefaultEntityRefMapTable[i].fEntityRefName != "lt" and
                HTMLInfo::sDefaultEntityRefMapTable[i].fEntityRefName != "quot"

            ) {
                continue;
            }
#endif
            sResult.push_back (HTMLInfo::sDefaultEntityRefMapTable[i]);
        }
    }
    return sResult;
}

void StyledTextIOWriter_HTML::Write ()
{
    WriterContext writerContext (*this);
    WriteHeader (writerContext);
    WriteBody (writerContext);
    WriteCloseTag (writerContext, "html");
    write ("\r\n");
}

void StyledTextIOWriter_HTML::WriteHeader (WriterContext& writerContext)
{
    if (fGetHTMLInfoFrom != nullptr and fGetHTMLInfoFrom->fDocTypeTag.length () != 0) {
        write (fGetHTMLInfoFrom->fDocTypeTag);
    }

    if (fGetHTMLInfoFrom == nullptr or fGetHTMLInfoFrom->fHTMLTag.length () == 0) {
        WriteOpenTag (writerContext, "html");
    }
    else {
        WriteOpenTagSpecial (writerContext, "html", fGetHTMLInfoFrom->fHTMLTag);
    }
    write ("\r\n");

    if (fGetHTMLInfoFrom == nullptr or fGetHTMLInfoFrom->fHeadTag.length () == 0) {
        WriteOpenTag (writerContext, "head");
    }
    else {
        WriteOpenTagSpecial (writerContext, "head", fGetHTMLInfoFrom->fHeadTag);
    }
    write ("\r\n");

    if (fGetHTMLInfoFrom != nullptr) {
        for (size_t i = 0; i < fGetHTMLInfoFrom->fUnknownHeaderTags.size (); i++) {
            write (fGetHTMLInfoFrom->fUnknownHeaderTags[i]);
        }
    }

    if (fGetHTMLInfoFrom != nullptr and fGetHTMLInfoFrom->fTitle.length () != 0) {
        WriteOpenTag (writerContext, "title");
        write (MapOutputTextFromTString (fGetHTMLInfoFrom->fTitle));
        WriteCloseTag (writerContext, "title");
        write ("\r\n");
    }

    WriteCloseTag (writerContext, "head");
    write ("\r\n");
}

void StyledTextIOWriter_HTML::WriteBody (WriterContext& writerContext)
{
    AssureStyleRunSummaryBuilt (writerContext);

    if (fGetHTMLInfoFrom == nullptr or fGetHTMLInfoFrom->fStartBodyTag.length () == 0) {
        WriteOpenTag (writerContext, "body");
    }
    else {
        WriteOpenTagSpecial (writerContext, "body", fGetHTMLInfoFrom->fStartBodyTag);
    }
    WriteInnerBody (writerContext);
    write ("\r\n");
    WriteCloseTag (writerContext, "body");
    write ("\r\n");
}

void StyledTextIOWriter_HTML::WriteInnerBody (WriterContext& writerContext)
{
    AssureStyleRunSummaryBuilt (writerContext);

    write ("\r\n");

    /*
     *  Walk through the characters, and output them one at a time. Walk
     *  SIMULTANEOUSLY through the style run information, and output new controlling
     *  tags on the fly.
     */
    writerContext.fLastEmittedISR    = StandardStyledTextImager::InfoSummaryRecord (Led_IncrementalFontSpecification (), 0);
    writerContext.fLastStyleChangeAt = 0;
    writerContext.fIthStyleRun       = 0;
    Led_tChar c                      = '\0';
    while (writerContext.GetSrcStream ().readNTChars (&c, 1) != 0) {
        WriteBodyCharacter (writerContext, c);
    }
    write ("\r\n");
}

void StyledTextIOWriter_HTML::WriteBodyCharacter (WriterContext& writerContext, Led_tChar c)
{
    ListStyle     listStyle   = eListStyle_None;
    unsigned char indentLevel = 0;
    writerContext.GetSrcStream ().GetListStyleInfo (&listStyle, &indentLevel);

    bool justOpenedList = false;
    if (listStyle == eListStyle_None) {
        if (writerContext.fEmittingListItem) {
            WriteCloseTag (writerContext, "li");
            write ("\r\n");
            writerContext.fEmittingListItem = false;
        }
        if (writerContext.fEmittingList) {
            WriteCloseTag (writerContext, "ul");
            write ("\r\n");
            writerContext.fEmittingList = false;
        }
    }
    else {
        if (writerContext.fEmittedStartOfPara) {
            WriteCloseTag (writerContext, "p");
            write ("\r\n");
            writerContext.fEmittedStartOfPara = false;
        }
        if (not writerContext.fEmittingList) {
            WriteOpenTag (writerContext, "ul");
            write ("\r\n");
            writerContext.fEmittingList = true;
        }
        if (not writerContext.fEmittingListItem) {
            WriteOpenTag (writerContext, "li");
            writerContext.fEmittingListItem = true;
            justOpenedList                  = true;
        }
    }

    if (not writerContext.fEmittedStartOfPara and (not writerContext.fEmittingListItem) and (not writerContext.fInTableCell)) {
        // No matter what - even if we're looking at \n - emit start of para.
        // Check src text justification...
        switch (writerContext.GetSrcStream ().GetJustification ()) {
            case eLeftJustify:
                WriteOpenTag (writerContext, "p", "align=\"left\"");
                break;
            case eCenterJustify:
                WriteOpenTag (writerContext, "p", "align=\"center\"");
                break;
            case eRightJustify:
                WriteOpenTag (writerContext, "p", "align=\"right\"");
                break;
            case eFullyJustify:
                WriteOpenTag (writerContext, "p", "align=\"justify\"");
                break;
            default:
                break; // ignore
        }
        writerContext.fEmittedStartOfPara = true;
    }

    // -1 for PREV character, NOT 0
    if (writerContext.GetSrcStream ().current_offset () - 1 == 0) {
        EmitBodyFontInfoChange (writerContext, fStyleRunSummary[writerContext.fIthStyleRun], false);
    }
    if (writerContext.GetSrcStream ().current_offset () - 1 == writerContext.fLastStyleChangeAt + writerContext.fLastEmittedISR.fLength) {
        writerContext.fLastStyleChangeAt = writerContext.GetSrcStream ().current_offset () - 1;
        writerContext.fIthStyleRun++;
    }

    // If the next char is a NL (in which case we will terminate the scope anyhow) - just do the close-tags - and skip the open ones...
    EmitBodyFontInfoChange (writerContext, fStyleRunSummary[writerContext.fIthStyleRun], c == '\n');

    switch (c) {
        case '\n': {
            if (writerContext.fEmittingListItem) {
                if (not justOpenedList) {
                    WriteCloseTag (writerContext, "li");
                    writerContext.fEmittingListItem = false;
                    write ("\r\n");
                }
            }
            else if (writerContext.fInTableCell) {
                WriteOpenCloseTag (writerContext, "br");
                write ("\r\n");
            }
            else if (writerContext.fEmittedStartOfPara) {
                WriteCloseTag (writerContext, "p");
                write ("\r\n");
                writerContext.fEmittedStartOfPara = false;
            }
        } break;

        case kEmbeddingSentinalChar: {
            unique_ptr<Table> table (writerContext.GetSrcStream ().GetTableAt (writerContext.GetCurSrcOffset () - 1));
            if (table.get () != nullptr) {
                WriteTable (writerContext, table.get ());
                [[maybe_unused]] size_t x = table->GetOffsetEnd ();
                Assert (x == 1);
                break;
            }

            // write now we cannot write any embeddings out in HTML files - so skip the character
            SimpleEmbeddedObjectStyleMarker* embedding = writerContext.GetCurSimpleEmbeddedObjectStyleMarker ();
            if (StandardURLStyleMarker* aHRef = dynamic_cast<StandardURLStyleMarker*> (embedding)) {
                Led_URLD urld = aHRef->GetURLData ();
                WriteOpenTag (writerContext, "a", "href=\"" + urld.GetURL () + "\"");
                write (urld.GetTitle ().c_str ());
                WriteCloseTag (writerContext, "a");
            }
        } break;

        default: {
            if (c == fSoftLineBreakChar) {
                WriteOpenCloseTag (writerContext, "br");
                break;
            }
            wchar_t unicodeC = c;
#if !qWideCharacters
// The entity refs we write must be in UNICODE. For non UNICODE Led - make the best guess we can (could parameterize this to make
// better - but for I18N - really should just use UNICODE).
#if qPlatform_MacOS
            const CodePage kInternalCodePageToMapFrom = kCodePage_MAC;
#else
            const CodePage kInternalCodePageToMapFrom = kCodePage_ANSI;
#endif
            if (static_cast<unsigned int> (c) >= 128) {
                // ascii chars OK - just have to worry about non-ascii ones...
                // Those must be mapped TO UNICODE
                CodePageConverter cpc (kInternalCodePageToMapFrom);
                size_t            outCharCnt = 1;
                cpc.MapToUNICODE (&c, 1, &unicodeC, &outCharCnt);
                Assert (outCharCnt == 1);
            }
#endif

            /*
                 *  Check if the char should be written as an entity-ref, and otherwise simply emit it.
                 */
            const vector<StyledTextIOReader_HTML::EntityRefMapEntry>& entityRefs = GetEntityRefMapTable ();
            vector<EntityRefMapEntry>::const_iterator                 i          = entityRefs.begin ();
            for (; i != entityRefs.end (); ++i) {
                if (unicodeC == (*i).fCharValue) {
                    write ('&');
                    write ((*i).fEntityRefName);
                    write (';');
                    break;
                }
            }
            if (i == entityRefs.end ()) {
                write (MapOutputTextFromWString (wstring (&unicodeC, 1)));
            }
        } break;
    }

    if (IsASCIISpace (c) and writerContext.GetSrcStream ().current_offset () - writerContext.fLastForcedNLAt > 80) {
        write ("\r\n"); // should write a newline every once in a while...
        writerContext.fLastForcedNLAt = writerContext.GetSrcStream ().current_offset ();
    }
}

/*
@METHOD:        StyledTextIOWriter_HTML::WriteTable
@DESCRIPTION:   <p>Write the contents of a table object. This creates a nested
            @'StyledTextIOWriter_HTML::WriterContext' object for each cell, and calls
            @'StyledTextIOWriter_HTML::WriteInnerBody' to write each cells contents.
            </p>
*/
void StyledTextIOWriter_HTML::WriteTable (WriterContext& writerContext, Table* table)
{
    RequireNotNull (table);
    write ("\r\n");

    using CellInfo = Table::CellInfo;

    WriteOpenTag (writerContext, "table");
    write ("\r\n");
    size_t rows = table->GetRows ();
    for (size_t r = 0; r < rows; ++r) {
        WriteOpenTag (writerContext, "tr");
        vector<CellInfo> cellInfos;
        table->GetRowInfo (r, &cellInfos);
        size_t columns = cellInfos.size ();
        for (size_t c = 0; c < columns; c++) {
            WriteOpenTag (writerContext, "td");
            unique_ptr<SrcStream> srcStream = unique_ptr<SrcStream> (table->MakeCellSubSrcStream (r, c));
            if (srcStream.get () != nullptr) {
                WriterContext                                       wc (writerContext, *srcStream.get ());
                vector<StandardStyledTextImager::InfoSummaryRecord> x = fStyleRunSummary;
                fStyleRunSummary.clear ();
                AssureStyleRunSummaryBuilt (wc);
                WriteInnerBody (wc);
                {
                    // close all tags on the tmp tag stack
                    for (vector<string>::reverse_iterator i = wc.fTagStack.rbegin (); i != wc.fTagStack.rend (); ++i) {
                        string thisTag = *i;
                        write ("</" + thisTag + ">");
                    }
                }
                fStyleRunSummary = x;
            }
            WriteCloseTag (writerContext, "td");
        }
        WriteCloseTag (writerContext, "tr");
        write ("\r\n");
    }
    WriteCloseTag (writerContext, "table");
    write ("\r\n");
}

/*
@METHOD:        StyledTextIOWriter_HTML::WriteOpenTag
@DESCRIPTION:   <p>Write the given tag (given by name - eg. "b" for bold) to the output. Save it in the tagstack
            so that later closing tags will be implicitly closed.</p>
                <p>See also @'StyledTextIOWriter_HTML::WriteCloseTag'.</p>
*/
void StyledTextIOWriter_HTML::WriteOpenTag (WriterContext& writerContext, const string& tagName, const string& tagExtras)
{
    Require (not tagName.empty ());
    Require (tagName[0] != '<'); // just the name - not the surrounding brackets...
    Require (tagName[0] != '/');
    writerContext.fTagStack.push_back (tagName);
    write ("<" + tagName);
    string te = tagExtras;
    if (not te.empty () and te[0] != ' ') {
        te = " " + te;
    }
    write (te.c_str ());
    write (">");
}

/*
@METHOD:        StyledTextIOWriter_HTML::WriteOpenTagSpecial
@DESCRIPTION:   <p>Like @'StyledTextIOWriter_HTML::WriteOpenTag'.- but use the tiven argument text as what we write out.
            Helper in case we saved exact text from input document.</p>
                <p>See also @'StyledTextIOWriter_HTML::WriteOpenTag'.</p>
*/
void StyledTextIOWriter_HTML::WriteOpenTagSpecial (WriterContext& writerContext, const string& tagName, const string& tagFullText)
{
    Require (not tagName.empty ());
    Require (tagName[0] != '<'); // just the name - not the surrounding brackets...
    Require (tagName[0] != '/');
    writerContext.fTagStack.push_back (tagName);
    write (tagFullText);
}

/*
@METHOD:        StyledTextIOWriter_HTML::WriteCloseTag
@DESCRIPTION:   <p>Closes the given tag and removes it from the tag stack. First removes any implicity closed
            tags from the tag stack (and emits their closing tag HTML).</p>
                <p>See also @'StyledTextIOWriter_HTML::WriteOpenTag'.</p>
*/
void StyledTextIOWriter_HTML::WriteCloseTag (WriterContext& writerContext, const string& tagName)
{
    // Walk up the stack and see how many tags need to be closed
    size_t countToPop = 0;
    for (vector<string>::reverse_iterator i = writerContext.fTagStack.rbegin (); i != writerContext.fTagStack.rend (); ++i) {
        string thisTag = *i;
        write ("</" + thisTag + ">");
        countToPop++;
        if (thisTag == tagName) { // XML - and our writer - are case sensative
            break;
        }
    }
    writerContext.fTagStack.erase (writerContext.fTagStack.end () - countToPop, writerContext.fTagStack.end ());
}

/*
@METHOD:        StyledTextIOWriter_HTML::WriteOpenCloseTag
@DESCRIPTION:   <p>Writes an open tag - possibly with arguments - and implicitly closes it. Tags like "br" get
            handled that way.</p>
                <p>See also @'StyledTextIOWriter_HTML::WriteOpenTag'.</p>
*/
void StyledTextIOWriter_HTML::WriteOpenCloseTag (WriterContext& /*writerContext*/, [[maybe_unused]] const string& tagName, const string& tagExtras)
{
    Require (not tagName.empty ());
    Require (tagName[0] != '<'); // just the name - not the surrounding brackets...
    Require (tagName[0] != '/');
    // NB: don't add to the tagstack since this is an OPEN & a CLOSE
    write ("<");
    string te = tagExtras;
    if (not te.empty () and te[0] != ' ') {
        te = " " + te;
    }
    write (te.c_str ());
    write ("/>");
}

/*
@METHOD:        StyledTextIOWriter_HTML::IsTagOnStack
@DESCRIPTION:   <p>See if there is an open tag on the stack with the given name.</p>
                <p>See also @'StyledTextIOWriter_HTML::WriteOpenTag'.</p>
*/
bool StyledTextIOWriter_HTML::IsTagOnStack (WriterContext& writerContext, const string& tagName)
{
    for (auto i = writerContext.fTagStack.begin (); i != writerContext.fTagStack.end (); ++i) {
        if (*i == tagName) { // XML - and our writer - are case sensative
            return true;
        }
    }
    return false;
}

static inline string PrintColorString (Led_Color color)
{
    unsigned short red    = color.GetRed () >> 8;
    unsigned short greeen = color.GetGreen () >> 8;
    unsigned short blue   = color.GetBlue () >> 8;
    char           buf[1024];
    buf[0]        = '"';
    buf[1]        = NumToHexChar (red >> 4);
    buf[2]        = NumToHexChar (red % 16);
    buf[3]        = NumToHexChar (greeen >> 4);
    buf[4]        = NumToHexChar (greeen % 16);
    buf[5]        = NumToHexChar (blue >> 4);
    buf[6]        = NumToHexChar (blue % 16);
    buf[7]        = '"';
    buf[8]        = '\0';
    string result = string (buf);
#if qStaticInitializerOfPairOfStringStringInternalCompilerBug
    for (auto i = kColorNameTable.begin (); i != kColorNameTable.end (); ++i) {
        if (Led_CasedStringsEqual (buf, "\"" + i->second + "\"")) {
            //result = "\'" + i->first + "\'";
            result = i->first;
            break;
        }
    }
#else
    for (size_t i = 0; i < NEltsOf (kColorNameTable); ++i) {
        if (Led_CasedStringsEqual (buf, "\"" + kColorNameTable[i].second + "\"")) {
            //result = "\'" + kColorNameTable[i].first + "\'";
            result = kColorNameTable[i].first;
            break;
        }
    }
#endif
    return result;
}

void StyledTextIOWriter_HTML::EmitBodyFontInfoChange (WriterContext& writerContext, const Led_FontSpecification& newOne, bool skipDoingOpenTags)
{
    // Close off old
    bool fontTagChanged = newOne.GetFontName () != writerContext.fLastEmittedISR.GetFontName () or newOne.GetPointSize () != writerContext.fLastEmittedISR.GetPointSize () or newOne.GetTextColor () != writerContext.fLastEmittedISR.GetTextColor ();
    if (fontTagChanged and IsTagOnStack (writerContext, "span")) {
        WriteCloseTag (writerContext, "span");
    }
    if (not newOne.GetStyle_Bold () and IsTagOnStack (writerContext, "b")) {
        WriteCloseTag (writerContext, "b");
    }
    if (not newOne.GetStyle_Italic () and IsTagOnStack (writerContext, "i")) {
        WriteCloseTag (writerContext, "i");
    }
    if (not newOne.GetStyle_Underline () and IsTagOnStack (writerContext, "u")) {
        WriteCloseTag (writerContext, "u");
    }
    if (newOne.GetStyle_SubOrSuperScript () != Led_FontSpecification::eSubscript and IsTagOnStack (writerContext, "sub")) {
        WriteCloseTag (writerContext, "sub");
    }
    if (newOne.GetStyle_SubOrSuperScript () != Led_FontSpecification::eSuperscript and IsTagOnStack (writerContext, "sup")) {
        WriteCloseTag (writerContext, "sup");
    }
#if qPlatform_Windows
    if (not newOne.GetStyle_Strikeout () and IsTagOnStack (writerContext, "strike")) {
        WriteCloseTag (writerContext, "strike");
    }
#endif

    // Open new tags
    if (skipDoingOpenTags) {
        // Set to a BLANK record - cuz we aren't actually emitting any open-tags - so make sure gets done later after the new <p> tag
        writerContext.fLastEmittedISR = StandardStyledTextImager::InfoSummaryRecord (Led_IncrementalFontSpecification (), fStyleRunSummary[writerContext.fIthStyleRun].fLength);
    }
    else {
        if (not IsTagOnStack (writerContext, "span")) {
            char   sprintfBuffer[1024];
            string cssInfo = "style=\"";
            cssInfo += "font-family: '" + Led_SDKString2ANSI (newOne.GetFontName ()) + "'; ";
            cssInfo += (snprintf (sprintfBuffer, NEltsOf (sprintfBuffer), "font-size: %dpt; ", newOne.GetPointSize ()), sprintfBuffer);
            cssInfo += "color: " + PrintColorString (newOne.GetTextColor ());
            cssInfo += "\"";
            WriteOpenTag (writerContext, "span",
#if 1
                          cssInfo
#else
                          "face=\"" + Led_SDKString2ANSI (newOne.GetFontName ()) +
                              "\" size=\"" + Led_NumberToDigitChar (HTMLInfo::RealFontSizeToHTMLFontSize (newOne.GetPointSize ())) +
                              "\" color=" + PrintColorString (newOne.GetTextColor ())
#endif
            );
        }
        if (newOne.GetStyle_Bold () and not IsTagOnStack (writerContext, "b")) {
            WriteOpenTag (writerContext, "b");
        }
        if (newOne.GetStyle_Italic () and not IsTagOnStack (writerContext, "i")) {
            WriteOpenTag (writerContext, "i");
        }
        if (newOne.GetStyle_Underline () and not IsTagOnStack (writerContext, "u")) {
            WriteOpenTag (writerContext, "u");
        }

        switch (newOne.GetStyle_SubOrSuperScript ()) {
            case Led_FontSpecification::eSubscript: {
                if (not IsTagOnStack (writerContext, "sub")) {
                    WriteOpenTag (writerContext, "sub");
                }
            } break;
            case Led_FontSpecification::eSuperscript: {
                if (not IsTagOnStack (writerContext, "sup")) {
                    WriteOpenTag (writerContext, "sup");
                }
            } break;
        }
#if qPlatform_Windows
        if (newOne.GetStyle_Strikeout () and not IsTagOnStack (writerContext, "strike")) {
            WriteOpenTag (writerContext, "strike");
        }
#endif
        writerContext.fLastEmittedISR = fStyleRunSummary[writerContext.fIthStyleRun];
    }
}

void StyledTextIOWriter_HTML::AssureStyleRunSummaryBuilt (WriterContext& writerContext)
{
    if (fStyleRunSummary.empty ()) {
        size_t totalTextLength = writerContext.GetSrcStream ().GetTotalTextLength ();
        fStyleRunSummary       = vector<StandardStyledTextImager::InfoSummaryRecord> (writerContext.GetSrcStream ().GetStyleInfo (0, totalTextLength));
    }
}

string StyledTextIOWriter_HTML::MapOutputTextFromWString (const wstring& text)
{
    // Try to write out non-ascii characters (maybe should only do non- ANSI/ISOLATIN1? chars this way?)
    // as entities - which are interpreted as UNICODE chars
    string result;
    result.reserve (text.length ());
    for (auto i = text.begin (); i != text.end (); ++i) {
        if (static_cast<unsigned int> (*i) <= 127) {
            char c = static_cast<char> (*i);
            result.append (&c, 1);
        }
        else {
            char buf[1024];
            (void)snprintf (buf, NEltsOf (buf), "&#%d;", static_cast<unsigned int> (*i));
            result += buf;
        }
    }
    return result;
}

string StyledTextIOWriter_HTML::MapOutputTextFromTString (const Led_tString& text)
{
#if qWideCharacters
    return MapOutputTextFromWString (text);
#else
    return text;
#endif
}
