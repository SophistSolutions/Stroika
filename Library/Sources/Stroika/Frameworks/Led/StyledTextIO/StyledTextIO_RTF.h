/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_RTF_h_
#define _Stroika_Frameworks_Led_StyledTextIO_RTF_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <map>
#include <memory>
#include <vector>

#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO.h"
#include "Stroika/Frameworks/Led/Support.h"

/*
@MODULE:    StyledTextIO_RTF
@DESCRIPTION:
        <p>The knowledge of how to read RTF is in @'StyledTextIOReader_RTF' and the knowledge of how to write RTF is in
    @'StyledTextIOWriter_RTF'.</p>
 */

namespace Stroika::Frameworks::Led::StyledTextIO {

#ifndef qUseMapForControlWordMap
#define qUseMapForControlWordMap 1
#endif

    /*
    @CLASS:         RTFIO
    @DESCRIPTION:   <p>This RTFIO class serves as a (shared) scope to define some other classes used in reading/writing
                RTF. It probably should be replaced with something using namespaces. But I wanted to make as small and
                simple a change as possible to the old code (that used @'RTFInfo' for this purpose).</p>
    */
    class RTFIO {
    public:
        static constexpr char kRTFOpenGroupChar     = '{';
        static constexpr char kRTFCloseGroupChar    = '}';
        static constexpr char kRTFStartTagChar      = '\\';
        static constexpr char kRTFQuoteNextCharChar = '\\';

    public:
        // perhaps should be more careful to keep these sorted, so easier to find particular one...
        enum ControlWordAtom {
            eMinControlAtom = 0,

            eControlAtom_tab = eMinControlAtom,
            eControlAtom_bullet,
            eControlAtom_endash,
            eControlAtom_emdash,
            eControlAtom_lquote,
            eControlAtom_rquote,
            eControlAtom_ldblquote,
            eControlAtom_rdblquote,

            eControlAtom_ansi,
            eControlAtom_author,
            eControlAtom_b,
            eControlAtom_blue,
            eControlAtom_brdrs,
            eControlAtom_brdrth,
            eControlAtom_brdrsh,
            eControlAtom_brdrdb,
            eControlAtom_brdrdot,
            eControlAtom_brdrdash,
            eControlAtom_brdrhair,
            eControlAtom_brdrdashsm,
            eControlAtom_brdrdashd,
            eControlAtom_brdrdashdd,
            eControlAtom_brdrtriple,
            eControlAtom_brdrtnthsg,
            eControlAtom_brdrthtnsg,
            eControlAtom_brdrtnthtnsg,
            eControlAtom_brdrtnthmg,
            eControlAtom_brdrthtnmg,
            eControlAtom_brdrtnthtnmg,
            eControlAtom_brdrtnthlg,
            eControlAtom_brdrthtnlg,
            eControlAtom_brdrtnthtnlg,
            eControlAtom_brdrwavy,
            eControlAtom_brdrwavydb,
            eControlAtom_brdrdashdotstr,
            eControlAtom_brdremboss,
            eControlAtom_brdrengrave,
            eControlAtom_brdrw,
            eControlAtom_brdrcf,
            eControlAtom_cchs,
            eControlAtom_cell,
            eControlAtom_cellx,
            eControlAtom_cf,
            eControlAtom_clcbpat,
            eControlAtom_cpg,
            eControlAtom_colortbl,
            eControlAtom_deff,
            eControlAtom_deflang,
            eControlAtom_deftab,
            eControlAtom_deleted,
            eControlAtom_dibitmap,
            eControlAtom_dn,
            eControlAtom_emfblip,
            eControlAtom_f,
            eControlAtom_fbidi,
            eControlAtom_fcharset,
            eControlAtom_fdecor,
            eControlAtom_fi,
            eControlAtom_fmodern,
            eControlAtom_footer,
            eControlAtom_fnil,
            eControlAtom_fonttbl,
            eControlAtom_fprq,
            eControlAtom_froman,
            eControlAtom_fs,
            eControlAtom_fswiss,
            eControlAtom_fscript,
            eControlAtom_ftech,
            eControlAtom_green,
            eControlAtom_header,
            eControlAtom_i,
            eControlAtom_ilvl,
            eControlAtom_info,
            eControlAtom_intbl,
            eControlAtom_jpegblip,
            eControlAtom_li,
            eControlAtom_line,
            eControlAtom_listtext,
            eControlAtom_ledprivateobjectembeddingformat,
            eControlAtom_ls,
            eControlAtom_mac,
            eControlAtom_macpict,
            eControlAtom_margb,
            eControlAtom_margl,
            eControlAtom_margr,
            eControlAtom_margt,
            eControlAtom_objdata,
            eControlAtom_object,
            eControlAtom_objemb,
            eControlAtom_objh,
            eControlAtom_objscalex,
            eControlAtom_objscaley,
            eControlAtom_objw,
            eControlAtom_outl,
            eControlAtom_paperh,
            eControlAtom_paperw,
            eControlAtom_par,
            eControlAtom_pard,
            eControlAtom_pc,
            eControlAtom_pca,
            eControlAtom_pich,
            eControlAtom_pichgoal,
            eControlAtom_picscalex,
            eControlAtom_picscaley,
            eControlAtom_pict,
            eControlAtom_picw,
            eControlAtom_picwgoal,
            eControlAtom_plain,
            eControlAtom_pmmetafile,
            eControlAtom_pn,
            eControlAtom_pngblip,
            eControlAtom_pntext,
            eControlAtom_qc,
            eControlAtom_qj,
            eControlAtom_ql,
            eControlAtom_qr,
            eControlAtom_red,
            eControlAtom_result,
            eControlAtom_ri,
            eControlAtom_row,
            eControlAtom_rtf,
            eControlAtom_sa,
            eControlAtom_sb,
            eControlAtom_shad,
            eControlAtom_sl,
            eControlAtom_slmult,
            eControlAtom_sub,
            eControlAtom_super,
            eControlAtom_strike,
            eControlAtom_stylesheet,
            eControlAtom_trleft,
            eControlAtom_trgaph,
            eControlAtom_trowd,
            eControlAtom_trpaddb,
            eControlAtom_trpaddl,
            eControlAtom_trpaddr,
            eControlAtom_trpaddt,
            eControlAtom_trspdb,
            eControlAtom_trspdl,
            eControlAtom_trspdr,
            eControlAtom_trspdt,
            eControlAtom_tx,
            eControlAtom_u,
            eControlAtom_uc,
            eControlAtom_ul,
            eControlAtom_ulnone,
            eControlAtom_up,
            eControlAtom_v,
            eControlAtom_wbitmap,
            eControlAtom_wmetafile,

            eControlAtomDynamicRangeStart,
            eMaxControlAtom = 65000
        };
#if qUseMapForControlWordMap
    public:
        enum {
            eMaxControlAtomNameLen = 31
        };
        struct ControlWordAtomName {
            ControlWordAtomName ()
            //:fName ()
            {
                fName[0] = '\0';
            }
            ControlWordAtomName (const char* c);
            operator char* ()
            {
                return fName;
            }
            operator const char* () const
            {
                return fName;
            }
            char& operator[] (size_t i)
            {
                Require (i <= eMaxControlAtomNameLen);
                return fName[i];
            }
            char fName[eMaxControlAtomNameLen + 1];
        };

    private:
        struct ControlWordAtomName_less {
            bool operator() (const ControlWordAtomName& _Left, const ControlWordAtomName& _Right) const
            {
                return ::strcmp (_Left, _Right) < 0;
            }
        };
        using ControlWordNameMap = map<ControlWordAtomName, ControlWordAtom, ControlWordAtomName_less>;
#else
    private:
        using StringNControlWordAtom = pair<string, ControlWordAtom>;

    public:
#if qFriendDeclarationsDontWorkWithNestedClassesBug
        friend bool operator< (const RTFIO::StringNControlWordAtom& lhs, const RTFIO::StringNControlWordAtom& rhs)
        {
            return lhs.first < rhs.first;
        }
#else
        friend bool operator< (const RTFIO::StringNControlWordAtom& lhs, const RTFIO::StringNControlWordAtom& rhs);
#endif
    private:
        struct StringNControlWordAtom_Comparator;
        friend struct StringNControlWordAtom_Comparator;

    private:
        using ControlWordNameMap = vector<StringNControlWordAtom>;
#endif
    private:
        static ControlWordNameMap sControlWordNameMap;

    public:
        static ControlWordNameMap mkDefaultControlWordNameMap ();

    public:
        static string GetAtomName (ControlWordAtom atom);
#if qUseMapForControlWordMap
        static ControlWordAtom EnterControlWord (const ControlWordAtomName& controlWord);
#else
        static ControlWordAtom EnterControlWord (const char* controlWord);
#endif

        class ControlWord {
        public:
            ControlWord ();

            ControlWordAtom fWord;
            bool            fHasArg;
            long            fValue;
        };

        /*
        @CLASS:         RTFIO::FontTableEntry
        @DESCRIPTION:   <p>These objects are collected together in a @'RTFIO::FontTable'.</p>
        */
        class FontTableEntry {
        public:
            FontTableEntry ();

        public:
            enum FontFamily {
                eNil,
                eRoman,
                eSwiss,
                eModern,
                eScript,
                eDecor,
                eTech,
                eBidi
            };

        public:
            SDKString  fFontName; //  font name
            int        fFNum;     //  font number
            FontFamily fFamily;   //  font family
            uint8_t fCharSet; //  font charset (from https://msdn.microsoft.com/en-us/library/windows/desktop/dd145037(v=vs.85).aspx - this is a byte)
            int fPitch;    //  font pitch
            int fCodePage; //  font code page
        };

        /*
        @CLASS:         RTFIO::FontTable
        @DESCRIPTION:   <p>This object contains a vector of @'RTFIO::FontTableEntry'. This corresponds to the
                    \fonttbl RTF element. Its used to associate font numbers with names and other information.</p>
        */
        class FontTable {
        public:
            FontTable ();
            FontTable (const vector<FontTableEntry>& fontTable);

        public:
            nonvirtual IncrementalFontSpecification GetFontSpec (int fontNumber);              // throws if not present
            nonvirtual const FontTableEntry*        LookupEntryByNumber (int fontNumber);      // return nullptr if not present
            nonvirtual const FontTableEntry*        LookupEntryByName (const SDKString& name); // return nullptr if not present
            nonvirtual FontTableEntry Add (const FontTableEntry& newEntry); // ignores the fFNum, and supplies its own! - returns same entry but with revised fFNum
            nonvirtual int FindSmallestUnusedFontNumber () const;

        public:
            vector<FontTableEntry> fEntries;
        };

        class ColorTable {
        public:
            ColorTable ();
            ColorTable (const vector<Color>& colorTable);

        public:
            nonvirtual Color  LookupColor (size_t colorNumber) const; // throws if not present
            nonvirtual size_t LookupColor (const Color& color) const; // asserts if not present
            nonvirtual size_t EnterColor (const Color& color);        // LookupColor, and if not present, add it. Either way, return index

        public:
            vector<Color> fEntries;
        };

        /*
        @CLASS:         RTFIO::ListTableEntry
        @DESCRIPTION:   <p>These objects are collected together in a @'RTFIO::ListTables'.</p>
        */
        class ListTableEntry {
        public:
            ListTableEntry ();

        public:
            int       fListID;         //  \listidN
            int       fListTemplateID; //  \listtemplateidN
            ListStyle fListStyle;      //  \levelnfcN
            int       fFontID;         // must be dynamicly specified based on current font table
        };

        /*
        @CLASS:         RTFIO::ListOverrideTableEntry
        @DESCRIPTION:   <p>These objects are collected together in a @'RTFIO::ListTables'.</p>
        */
        class ListOverrideTableEntry {
        public:
            ListOverrideTableEntry ();

        public:
            int fListID; //  \listidN
        };

        /*
        @CLASS:         RTFIO::ListTables
        @DESCRIPTION:   <p>This object contains a vector of @'RTFIO::ListTableEntry'. This corresponds to the
                    \listtable RTF element. It also contains a vector of @'RTFIO::ListOverrideTableEntry' entries. These
                    map to \listoverridetable entries. See the RTF 1.5 (or later) spec.</p>
        */
        class ListTables {
        public:
            ListTables ();
            ListTables (const vector<ListTableEntry>& listTableEntries, const vector<ListOverrideTableEntry>& listOverrideTableEntries);

        public:
            vector<ListTableEntry>         fEntries;
            vector<ListOverrideTableEntry> fOverrideEntries;
        };

#if qStroika_Frameworks_Led_SupportGDI
        static const Led_PrivateEmbeddingTag kRTFBodyGroupFragmentEmbeddingTag;
        static const Led_ClipFormat          kRTFBodyGroupFragmentClipFormat;

        // Just use UnknownObject as a tmp hack for these RTF embeddings
        using UnknownRTFEmbedding = StandardUnknownTypeStyleMarker;

        class RTFOLEEmbedding {
        public:
            static const Led_PrivateEmbeddingTag kEmbeddingTag;

            virtual void      PostCreateSpecifyExtraInfo (TWIPS_Point size)           = 0;
            virtual SDKString GetObjClassName () const                                = 0;
            virtual void      DoWriteToOLE1Stream (size_t* nBytes, byte** resultData) = 0;
            virtual Led_Size  GetSize ()                                              = 0;
        };
#endif
    };

    /*
    @CLASS:         RTFInfo
    @DESCRIPTION:   <p>This RTFInfo class is designed to capture any extra information about the RTF file
                you might want to keep track of, as a side-effect of reading it in (beyond simply
                the things which Led supports). For example, if you wanted to keep track of the
                original character set, the author, or the original style sheet information, this
                would be a good place to store those things.</p>
                    <p>NB: This class used to contain all the definitions name contained in @'RTFIO'.</p>
    */
    class RTFInfo {
    public:
        RTFInfo ();

    public:
        static TWIPS GetStaticDefaultTabStopWidth ();

    public:
        TWIPS fDefaultTabStop;

    public:
        nonvirtual TWIPS GetDefaultTabStop () const;

    public:
        static TWIPS_Point GetStaticDefaultPaperSize ();

    public:
        TWIPS_Point fDefaultPaperSize;

    public:
        static void GetStaticDefaultMargins (TWIPS* t, TWIPS* l, TWIPS* b, TWIPS* r);

    public:
        TWIPS fDefaultMarginTop;
        TWIPS fDefaultMarginLeft;
        TWIPS fDefaultMarginBottom;
        TWIPS fDefaultMarginRight;

    public:
        nonvirtual TWIPS GetEffectiveDrawingWidth () const;
    };

    /*
    @CLASS:         StyledTextIOReader_RTF
    @BASES:         @'StyledTextIOReader'
    @DESCRIPTION:   <p>This is the class you construct to read in content as RTF-formatted text. You
        must specify SrcStream of input data (usually a file, or a clipboard record), and a
        SinkStream (usually a text buffer) to write to.</p>
            <p>Then call @'StyledTextIOReader_RTF::Read' () to get the actual reading done.</p>
    */
    class StyledTextIOReader_RTF : public StyledTextIOReader {
    public:
        StyledTextIOReader_RTF (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo* rtfInfo = nullptr);
        ~StyledTextIOReader_RTF ();

    private:
        using inherited = StyledTextIOReader;

    public:
        virtual void Read () override;
        virtual bool QuickLookAppearsToBeRightFormat () override;

    public:
        nonvirtual FontSpecification GetPlainFont () const;
        nonvirtual void              SetPlainFont (const FontSpecification& fsp);

    private:
        FontSpecification fPlainFont;

    public:
        class ReaderContext;

    protected:
        virtual void ReadGroup (ReaderContext& readerContext); // must be looking at a kRTFOpenGroupChar

    protected:
        struct SpecialCharMappings {
            RTFIO::ControlWordAtom fControlWordName;
            wchar_t                fUNICODECharacter;
        };
        static const SpecialCharMappings kMappings[8];

    public:
        nonvirtual Led_tChar GetDefaultUnsupportedCharacterChar () const;

    protected:
        Led_tChar fDefaultUnsupportedCharacterChar;

    protected:
        virtual RTFIO::ControlWord ReadControlWord (); // must be looking at a kRTFStartTagChar
        // HandleControlWord () returns true if it read to end (and consumed end) of current group
        virtual bool HandleControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandlePossibleSpecialCharacterControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual void ReadCommentGroup (ReaderContext& readerContext);

    protected:
        virtual void ReadIn_pn_Group (ReaderContext& readerContext);

    protected:
        virtual bool HandleControlWord_ansi (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_author (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_b (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_brdrXXX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_cchs (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_cell (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_cellx (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_cf (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_clcbpat (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_cpg (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_colortbl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_deff (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_deftab (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_deleted (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_dn (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_f (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_fonttbl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_footer (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_fi (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_fs (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_header (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_i (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ilvl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_info (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_intbl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_li (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_line (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_listtext (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ls (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_mac (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_margX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_object (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_outl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_paperX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_par (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_pard (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_pc (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_pca (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_pict (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_plain (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_pntext (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_qc (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_qj (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ql (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_qr (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ri (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_row (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_rtf (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_sa (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_sb (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_shad (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_sl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_slmult (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_sub (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_super (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_strike (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_stylesheet (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_trleft (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_trgaph (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_trowd (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_trpaddX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_trspdX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_tx (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_u (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_uc (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ul (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_ulnone (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_up (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);
        virtual bool HandleControlWord_v (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);

        virtual bool HandleControlWord_UnknownControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord);

    protected:
        nonvirtual void CheckIfAboutToStartBody (ReaderContext& readerContext);
        virtual void    AboutToStartBody (ReaderContext& readerContext);

    protected:
        virtual RTFIO::FontTableEntry ReadInFontTablesEntry ();
        virtual void                  ReadInObjectSubGroupEntry (ReaderContext& readerContext, vector<char>* data, size_t* resultFoundAt);
        virtual void                  ReadObjData (vector<char>* data);
        virtual void ConstructOLEEmebddingFromRTFInfo (ReaderContext& readerContext, TWIPS_Point size, size_t nBytes, const void* data);
        virtual void ConstructLedEmebddingFromRTFInfo (ReaderContext& readerContext, size_t nBytes, const void* data);

    protected:
        enum ImageFormat {
            eEMF, // enhanced meta file
            ePNG, // portable network graphix file
            eJPEG,
            eMacPICT,
            ePMMetaFile, // OS/2 meta file
            eWMF,        // Windows meta file
            eDIB,
            eBITMAP,                      // device dependent bitmap
            eDefaultImageFormat = eBITMAP //  according to RTF 1.5 spec - "The \wbitmap control word is optional. If no other picture type is specified, the picture is assumed to be a Windows bitmap"
        };
        virtual void ReadPictData (vector<char>* data);
        virtual void ReadTopLevelPictData (TWIPS_Point* shownSize, ImageFormat* imageFormat, TWIPS_Point* bmSize, vector<char>* objData);
        virtual Led_DIB* ConstructDIBFromData (TWIPS_Point shownSize, ImageFormat imageFormat, TWIPS_Point bmSize, size_t nBytes, const void* data);
#if qPlatform_Windows
        virtual Led_DIB* ConstructDIBFromEMFHelper (TWIPS_Point shownSize, TWIPS_Point bmSize, const HENHMETAFILE hMF);
#endif

    protected:
        virtual void ApplyFontSpec (ReaderContext& readerContext, const RTFIO::ControlWord& cw);

        // utility functions to help reading/low level scanning rtf
    protected:
        nonvirtual unsigned char GetNextRTFHexByte () const;
        nonvirtual string        ReadInGroupAndSave ();
        nonvirtual void          SkipToEndOfCurrentGroup ();
        nonvirtual void          ScanForwardFor (const char* setOfChars); // stop when next char is a member of 'setOfChars'
        // (nb: doesn't look at current/prev chars - just FOLLOWING ones
        // (at eof returns no error, but does return)
        nonvirtual bool SearchForwardFor (const char* searchFor, size_t maxCharsToExamine = size_t (-1));

    protected:
        nonvirtual Color LookupColor (ReaderContext& readerContext, size_t index) const;

    public:
        nonvirtual RTFInfo& GetRTFInfo () const;

    private:
        RTFInfo* fRTFInfo;
        bool     fOwnRTFInfo;

#if qPlatform_Windows
    private:
        FontSpecification::FontSize fCachedFontSize; // Keep these as a speedtweek - SetPointSize() expensive on WIN32
        long                        fCachedFontSizeTMHeight;
#endif
    };

    /*
    @CLASS:         StyledTextIOReader_RTF::ReaderContext
    @DESCRIPTION:   <p>This context object is used during the process of reading RTF text from a source, and
                internalizing it into internal format. It is used to keep track of various document context information,
                like character sets, etc.</p>
    */
    class StyledTextIOReader_RTF::ReaderContext {
    public:
        ReaderContext (StyledTextIOReader_RTF& reader);
        ~ReaderContext ();

    public:
        nonvirtual StyledTextIOReader_RTF& GetReader () const;

    private:
        StyledTextIOReader_RTF& fReader;

    public:
        CodePage fDocumentCharacterSet;
        int      fDefaultFontNumber{-1}; // -1 ==> none specified
        bool     fStartedBodyYet{false};

    public:
        nonvirtual CodePage GetCurrentInputCharSetEncoding () const;
        nonvirtual void     UseInputCharSetEncoding (CodePage codePage);

    private:
        CodePage fCurrentInputCharSetEncoding_;
        char     fMultiByteInputCharBuf[2];

    public:
        size_t fUnicodeUCValue{1}; // support for \u and \uc RTF tags
        size_t fSkipNextNChars_UC{0};

    public:
        nonvirtual void PutRawCharToDestination (char c);

    public:
        size_t fHiddenTextStart{static_cast<size_t> (-1)}; // -1 => NOT in a hidden text block, and otherwise its the start of the block.

    public:
        /*
         *  Support for RTF destinations.
         */
        class Destination_;
        class SinkStreamDestination;

    public:
        nonvirtual Destination_& GetDestination () const;
        virtual void             SetDestination (Destination_* destination);

    private:
        Destination_* fCurrentDestination{nullptr};

    public:
        unique_ptr<Destination_> fDefaultDestination;

    public:
        class GroupContext;

    public:
        nonvirtual GroupContext* GetCurrentGroupContext () const; // can be nullptr
        nonvirtual GroupContext* GetParentGroupContext () const;  // can be nullptr
    private:
        GroupContext* fCurrentGroup{nullptr};

        friend class GroupContext;

    public:
        RTFIO::FontTable*  fFontTable{nullptr};
        RTFIO::ColorTable* fColorTable{nullptr};
    };

    /**
     */
    class StyledTextIOReader_RTF::ReaderContext::Destination_ {
    protected:
        Destination_ () = default;

    public:
        virtual ~Destination_ () = default;

    public:
        /**
         */
        struct Context {
            Context () = default;

            IncrementalFontSpecification fFontSpec{};
            StandardTabStopList          fTabStops{StandardTabStopList{RTFInfo::GetStaticDefaultTabStopWidth ()}};
            Justification                fJustification{eLeftJustify};
            TWIPS                        fSpaceBefore{TWIPS{0}};
            TWIPS                        fSpaceAfter{TWIPS{0}};
            CoordinateType               fSpaceBetweenLines{1000};
            bool                         fSpaceBetweenLinesMult{true};
            ListStyle                    fListStyle{eListStyle_None};
            unsigned char                fListIndentLevel{0};
            TWIPS                        fFirstIndent{TWIPS{0}};
            TWIPS                        fLeftMargin{TWIPS{0}};
            TWIPS                        fRightMargin{TWIPS{0}};
            bool                         fTextHidden{false};
        };

    public:
        virtual void AppendText (const Led_tChar* text, size_t nTChars) = 0;
#if qStroika_Frameworks_Led_SupportGDI
        virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) = 0;
#endif
        virtual void AppendSoftLineBreak ()                                 = 0;
        virtual void EndParagraph ()                                        = 0;
        virtual void UseFont (const IncrementalFontSpecification& fontSpec) = 0;
        virtual void SetJustification (Justification justification)         = 0;
        virtual void SetTabStops (const StandardTabStopList& tabStops)      = 0;
        virtual void SetFirstIndent (TWIPS tx)                              = 0;
        virtual void SetLeftMargin (TWIPS lhs)                              = 0;
        virtual void SetRightMargin (TWIPS rhs)                             = 0;
        virtual void SetSpaceBefore (TWIPS tx)                              = 0;
        virtual void SetSpaceAfter (TWIPS tx)                               = 0;
        virtual void SetSpaceBetweenLines (CoordinateType tx)               = 0;
        virtual void SetSpaceBetweenLinesMult (bool multipleLineSpacing)    = 0;
        virtual void SetTextHidden (bool hidden)                            = 0;
        virtual void SetInTable (bool inTable)                              = 0;
        virtual void EndRow (bool forceEmit = false)                        = 0;
        virtual void EndCell (bool forceEmit = false)                       = 0;
        virtual void SetListStyle (ListStyle listStyle)                     = 0;
        virtual void SetListIndentLevel (unsigned char indentLevel)         = 0;
        virtual void SetTableBorderColor (Color c)                          = 0;
        virtual void SetCellX (TWIPS cellx)                                 = 0;
        virtual void SetCellBackColor (const Color& c)                      = 0;
        virtual void Call_trowd ()                                          = 0;
        virtual void Set_trleft (TWIPS t)                                   = 0;
        virtual void SetDefaultCellMarginsForRow_top (TWIPS t)              = 0;
        virtual void SetDefaultCellMarginsForRow_left (TWIPS t)             = 0;
        virtual void SetDefaultCellMarginsForRow_bottom (TWIPS t)           = 0;
        virtual void SetDefaultCellMarginsForRow_right (TWIPS t)            = 0;
        virtual void SetDefaultCellSpacingForRow_top (TWIPS t)              = 0;
        virtual void SetDefaultCellSpacingForRow_left (TWIPS t)             = 0;
        virtual void SetDefaultCellSpacingForRow_bottom (TWIPS t)           = 0;
        virtual void SetDefaultCellSpacingForRow_right (TWIPS t)            = 0;

    public:
        virtual void Flush () = 0;
        virtual void Done ()  = 0;

    public:
        virtual Context GetContext () const           = 0;
        virtual void    SetContext (const Context& c) = 0;
    };

    /*
    @CLASS:         StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination
    @BASES:         @'StyledTextIOReader_RTF::ReaderContext::Destination_'
    @DESCRIPTION:
    */
    class StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination : public StyledTextIOReader_RTF::ReaderContext::Destination_ {
    public:
        using SinkStream = StyledTextIOReader::SinkStream;

    public:
        SinkStreamDestination (StyledTextIOReader_RTF& reader);
        ~SinkStreamDestination ();

    public:
        virtual void AppendText (const Led_tChar* text, size_t nTChars) override;
#if qStroika_Frameworks_Led_SupportGDI
        virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) override;
#endif
        virtual void AppendSoftLineBreak () override;
        virtual void EndParagraph () override;
        virtual void UseFont (const IncrementalFontSpecification& fontSpec) override;
        virtual void SetJustification (Justification justification) override;
        virtual void SetTabStops (const StandardTabStopList& tabStops) override;
        virtual void SetFirstIndent (TWIPS tx) override;
        virtual void SetLeftMargin (TWIPS lhs) override;
        virtual void SetRightMargin (TWIPS rhs) override;
        virtual void SetSpaceBefore (TWIPS tx) override;
        virtual void SetSpaceAfter (TWIPS tx) override;
        virtual void SetSpaceBetweenLines (CoordinateType tx) override;
        virtual void SetSpaceBetweenLinesMult (bool multipleLineSpacing) override;
        virtual void SetTextHidden (bool hidden) override;
        virtual void SetInTable (bool inTable) override;
        virtual void EndRow (bool forceEmit = false) override;
        virtual void EndCell (bool forceEmit = false) override;
        virtual void SetListStyle (ListStyle listStyle) override;
        virtual void SetListIndentLevel (unsigned char indentLevel) override;
        virtual void SetTableBorderColor (Color c) override;
        virtual void SetCellX (TWIPS cellx) override;
        virtual void SetCellBackColor (const Color& c) override;
        virtual void Call_trowd () override;
        virtual void Set_trleft (TWIPS t) override;
        virtual void SetDefaultCellMarginsForRow_top (TWIPS t) override;
        virtual void SetDefaultCellMarginsForRow_left (TWIPS t) override;
        virtual void SetDefaultCellMarginsForRow_bottom (TWIPS t) override;
        virtual void SetDefaultCellMarginsForRow_right (TWIPS t) override;
        virtual void SetDefaultCellSpacingForRow_top (TWIPS t) override;
        virtual void SetDefaultCellSpacingForRow_left (TWIPS t) override;
        virtual void SetDefaultCellSpacingForRow_bottom (TWIPS t) override;
        virtual void SetDefaultCellSpacingForRow_right (TWIPS t) override;

    private:
        nonvirtual void AssureTableOpen ();
        nonvirtual void DoStartRow ();
        nonvirtual void DoStartCell ();
        nonvirtual void DoEndTable ();

    public:
        virtual void Flush () override;
        virtual void Done () override;

    public:
        virtual Context GetContext () const override;
        virtual void    SetContext (const Context& c) override;

    private:
        nonvirtual void ApplyContext (const Context& c);

    public:
        nonvirtual size_t current_offset () const;
        nonvirtual void   InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner);

    private:
        nonvirtual void AppendText_ (const Led_tChar* text, size_t nTChars);
        nonvirtual void AboutToChange () const;
        nonvirtual void FlushSetContextCalls () const;
        nonvirtual void FlushParaEndings () const;

    private:
        SinkStream&             fSinkStream;
        RTFInfo&                fRTFInfo;
        StyledTextIOReader_RTF& fReader;
        Context                 fCurrentContext{};
        Context                 fNewContext{};
        mutable bool            fNewContextPending{false};
        Led_tChar               fSmallBuffer[8 * 1024]; // buffer is simply a performance hack...
        size_t                  fTCharsInSmallBuffer{0};
        mutable bool            fParaEndedFlag{false};

        // RETHINK - MAYBE DON'T NEED???
        mutable bool fParaEndBeforeNewContext{false}; // SPR#0968 - if we get BOTH delayed paraend and delayed setcontext call, AND
        // finally have to flush - then we need to preserve the ordering with a flag
    private:
        bool   fInTable{false};
        bool   fTableOpen{false};
        size_t fTableNextRowNum{0};
        bool   fTableInRow{false}; // true if we've started a row; false if row just ended and not sure if there will be another
        size_t fTableNextCellNum{0};
        bool   fTableInCell{false}; // ditto - except for cells

    private:
        struct CellInfo {
            CellInfo ();
            TWIPS  f_cellx;
            Color  f_clcbpat;
            size_t fColSpan;
        };
        CellInfo fNextCellInfo; // place to store up info (like clcbpat) til we get the terminating \cellx
    private:
        struct RowInfo {
            RowInfo ();
            TWIPS f_trrh;
            TWIPS f_trleft;
            TWIPS_Rect fDefaultCellMargins; // Not REALLY a rect - just a handy way to store 4 values... and OK since its private - not part of API
            TWIPS_Rect       fDefaultCellSpacing; // ''
            vector<CellInfo> fCellInfosForThisRow;
        };
        RowInfo fThisRow;
    };

    /*
    @CLASS:         StyledTextIOReader_RTF::ReaderContext::GroupContext
    @DESCRIPTION:   <p>Helper class to implement nested scopes where we save/restore font info while reading</p>
    */
    class StyledTextIOReader_RTF::ReaderContext::GroupContext {
    public:
        GroupContext (ReaderContext& readerContext);
        ~GroupContext ();

    private:
        GroupContext (const GroupContext&)            = delete;
        GroupContext& operator= (const GroupContext&) = delete;

    private:
        ReaderContext& fReaderContext;
        GroupContext*  fParentGroup;

    public:
        size_t                fCurrentGroupStartIdx;
        CodePage              fCurrentCodePage;
        CodePage              fCCHSCodePage;
        Destination_::Context fDestinationContext;

    private:
        friend class StyledTextIOReader_RTF::ReaderContext;
    };

    /*
    @CLASS:         StyledTextIOWriter_RTF
    @BASES:         @'StyledTextIOWriter'
    @DESCRIPTION:
    */
    class StyledTextIOWriter_RTF : public StyledTextIOWriter {
    public:
        StyledTextIOWriter_RTF (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo* rtfInfo = nullptr);
        ~StyledTextIOWriter_RTF ();

    public:
        virtual void Write () override;

    public:
        nonvirtual CodePage GetCurrentOutputCharSetEncoding () const;
        nonvirtual void     UseOutputCharSetEncoding (CodePage codePage);

    private:
        CodePage fCurrentOutputCharSetEncoding;

    public:
        nonvirtual const vector<pair<string, wchar_t>>& GetCharactersSavedByName () const;
        nonvirtual void SetCharactersSavedByName (const vector<pair<string, wchar_t>>& charactersSavedByName);

    private:
        vector<pair<string, wchar_t>> fCharactersSavedByName;
        map<string, wchar_t>          fCharactersSavedByName_Name2Char;
        map<wchar_t, string>          fCharactersSavedByName_Char2Name;

    protected:
        class WriterContext;

    public:
        using Table = StyledTextIOWriter::SrcStream::Table;

    protected:
        virtual void    WriteHeader (WriterContext& writerContext);
        virtual void    WriteBody (WriterContext& writerContext);
        virtual void    WriteBodyCharacter (WriterContext& writerContext, Led_tChar c);
        nonvirtual void WritePlainUnicodeCharCharacterHelper (wchar_t c);
        nonvirtual void WriteHexCharHelper (unsigned char c);
        virtual void    WriteStartParagraph (WriterContext& writerContext);
        virtual void    WriteTable (WriterContext& writerContext, Table* table);
#if qStroika_Frameworks_Led_SupportGDI
        virtual bool PossiblyWriteUnknownRTFEmbedding (WriterContext& writerContext, SimpleEmbeddedObjectStyleMarker* embedding);
        virtual bool PossiblyWriteOLERTFEmbedding (WriterContext& writerContext, SimpleEmbeddedObjectStyleMarker* embedding);
        virtual bool PossiblyWritePICTEmbedding (WriterContext& writerContext, SimpleEmbeddedObjectStyleMarker* embedding);
        virtual void WritePrivatLedEmbedding (WriterContext& writerContext, SimpleEmbeddedObjectStyleMarker* embedding);
#endif
        virtual void    WriteTag (const char* tagStr);
        virtual void    WriteTagNValue (const char* tagStr, int value);
        virtual void    WriteHexCharDataBlock (size_t nBytes, const void* resultData);
        nonvirtual void WriteRTFHexByte (unsigned char theByte);

    protected:
        virtual void WriteDocCharset ();
        virtual void WriteFontTable (WriterContext& writerContext);
        virtual void WriteFontTablesEntry (const RTFIO::FontTableEntry& entry);
        virtual void WriteColorTable (WriterContext& writerContext);
        virtual void WriteListTable ();
        virtual void WriteListTablesEntry (const RTFIO::ListTableEntry& entry);
        virtual void WriteListOverrideTablesEntry (const RTFIO::ListOverrideTableEntry& oEntry);
        virtual void WriteGenerator ();

    protected:
        virtual void EmitBodyFontInfoChange (WriterContext& writerContext, const FontSpecification& newOne);
        virtual void EmitBodyFontInfoChange (WriterContext& writerContext, const FontSpecification& newOne, const FontSpecification& oldOne);

    protected:
        virtual void AssureColorTableBuilt (WriterContext& writerContext);
        virtual void AssureFontTableBuilt (WriterContext& writerContext);
        virtual void AssureStyleRunSummaryBuilt (WriterContext& writerContext);
        virtual void AssureListTableBuilt (WriterContext& writerContext);

    private:
        RTFInfo* fRTFInfo{nullptr};

    protected:
        RTFIO::FontTable*               fFontTable{nullptr};
        RTFIO::ColorTable*              fColorTable{nullptr};
        RTFIO::ListTables*              fListTable{nullptr};
        CodePage                        fDocumentCharacterSet;
        vector<StyledInfoSummaryRecord> fStyleRunSummary;
        const Led_tChar                 fSoftLineBreakChar;
        DiscontiguousRun<bool>          fHidableTextRuns;
    };

    /*
    @CLASS:         StyledTextIOWriter_RTF::WriterContext
    @DESCRIPTION:
    */
    class StyledTextIOWriter_RTF::WriterContext {
    public:
        WriterContext (StyledTextIOWriter_RTF& writer);
        WriterContext (WriterContext& parentContext, SrcStream& srcStream);

    public:
        nonvirtual StyledTextIOWriter_RTF& GetWriter () const;

    private:
        StyledTextIOWriter_RTF& fWriter;

    public:
        nonvirtual SrcStream&  GetSrcStream () const;
        nonvirtual SinkStream& GetSinkStream () const;

    public:
        using Table = StyledTextIOWriter::SrcStream::Table;

    public:
        virtual size_t GetCurSrcOffset () const;
#if qStroika_Frameworks_Led_SupportGDI
        virtual SimpleEmbeddedObjectStyleMarker* GetCurSimpleEmbeddedObjectStyleMarker () const;
#endif
        virtual Table* GetCurRTFTable () const;

    public:
        bool                               fInTable;
        StyledTextIOWriter_RTF::SrcStream& fSrcStream;
        StyledInfoSummaryRecord            fLastEmittedISR;
        size_t                             fNextStyleChangeAt;
        size_t                             fIthStyleRun;
        size_t                             fNextHidableTextChangeAt;
        bool                               fHidableTextRegionOpen;
        size_t                             fIthHidableTextRun;
        size_t                             fCharsToSkip;
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
//  class   RTFIO::StringNControlWordAtom
#if !qFriendDeclarationsDontWorkWithNestedClassesBug && !qUseMapForControlWordMap
    inline bool operator< (const RTFIO::StringNControlWordAtom& lhs, const RTFIO::StringNControlWordAtom& rhs)
    {
        return lhs.first < rhs.first;
    }
#endif

    //  class   RTFIO::ControlWord
    inline RTFIO::ControlWord::ControlWord ()
        : fWord (eMinControlAtom)
        , fHasArg (false)
        , fValue (0)
    {
    }

    //  class   RTFInfo
    inline TWIPS RTFInfo::GetStaticDefaultTabStopWidth ()
    {
        return TWIPS (720); //  default to 1/2 inch - RTF spec default
    }
    inline TWIPS RTFInfo::GetDefaultTabStop () const
    {
        return fDefaultTabStop;
    }
    inline TWIPS_Point RTFInfo::GetStaticDefaultPaperSize ()
    {
        // From RTFSpec 1.4
        return TWIPS_Point (TWIPS (11 * 1440), TWIPS (static_cast<long> (8.5 * 1440)));
    }
    inline void RTFInfo::GetStaticDefaultMargins (TWIPS* t, TWIPS* l, TWIPS* b, TWIPS* r)
    {
        RequireNotNull (t);
        RequireNotNull (l);
        RequireNotNull (b);
        RequireNotNull (r);
        *t = TWIPS (1440);
        *l = TWIPS (1800);
        *b = TWIPS (1440);
        *r = TWIPS (1800);
    }
    inline RTFInfo::RTFInfo ()
        : fDefaultTabStop (GetStaticDefaultTabStopWidth ())
        , fDefaultPaperSize (GetStaticDefaultPaperSize ())
        , fDefaultMarginTop (0)
        , fDefaultMarginLeft (0)
        , fDefaultMarginBottom (0)
        , fDefaultMarginRight (0)
    {
        GetStaticDefaultMargins (&fDefaultMarginTop, &fDefaultMarginLeft, &fDefaultMarginBottom, &fDefaultMarginRight);
    }
    inline TWIPS RTFInfo::GetEffectiveDrawingWidth () const
    {
        TWIPS subtract = TWIPS (fDefaultMarginLeft + fDefaultMarginRight);
        Ensure (fDefaultPaperSize.h > subtract);
        return TWIPS (fDefaultPaperSize.h - subtract);
    }

    //  class   StyledTextIOReader_RTF
    inline Led_tChar StyledTextIOReader_RTF::GetDefaultUnsupportedCharacterChar () const
    {
        return fDefaultUnsupportedCharacterChar;
    }

    //  class   StyledTextIOReader_RTF::ReaderContext
    inline StyledTextIOReader_RTF& StyledTextIOReader_RTF::ReaderContext::GetReader () const
    {
        return fReader;
    }
    inline CodePage StyledTextIOReader_RTF::ReaderContext::GetCurrentInputCharSetEncoding () const
    {
        return fCurrentInputCharSetEncoding_;
    }
    inline StyledTextIOReader_RTF::ReaderContext::Destination_& StyledTextIOReader_RTF::ReaderContext::GetDestination () const
    {
        EnsureNotNull (fCurrentDestination);
        return (*fCurrentDestination);
    }
    inline void StyledTextIOReader_RTF::ReaderContext::SetDestination (Destination_* destination)
    {
        fCurrentDestination = destination;
    }
    inline StyledTextIOReader_RTF::ReaderContext::GroupContext* StyledTextIOReader_RTF::ReaderContext::GetCurrentGroupContext () const
    {
        return fCurrentGroup; // Can be nullptr
    }
    inline StyledTextIOReader_RTF::ReaderContext::GroupContext* StyledTextIOReader_RTF::ReaderContext::GetParentGroupContext () const
    {
        // Can return nullptr
        if (fCurrentGroup != nullptr) {
            return fCurrentGroup->fParentGroup;
        }
        return nullptr;
    }

    //  class   StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination
    //NOTHING INLINE

    //  class   StyledTextIOReader_RTF
    /*
    @METHOD:        StyledTextIOReader_RTF::GetPlainFont
    @DESCRIPTION:   <p>Get the font which is used for RTF \plain directives. This can be set
        via @'StyledTextIOReader_RTF::SetPlainFont'</p>
    */
    inline FontSpecification StyledTextIOReader_RTF::GetPlainFont () const
    {
        return fPlainFont;
    }
    /*
    @METHOD:        StyledTextIOReader_RTF::SetPlainFont
    @DESCRIPTION:   <p>Set the font which is used for RTF \plain directives. See
        @'StyledTextIOReader_RTF::GetPlainFont'</p>
    */
    inline void StyledTextIOReader_RTF::SetPlainFont (const FontSpecification& fsp)
    {
        fPlainFont = fsp;
    }
    inline void StyledTextIOReader_RTF::CheckIfAboutToStartBody (ReaderContext& readerContext)
    {
        if (not readerContext.fStartedBodyYet) {
            AboutToStartBody (readerContext);
            Assert (readerContext.fStartedBodyYet);
        }
    }
    /*
    @METHOD:        StyledTextIOReader_RTF::LookupColor
    @DESCRIPTION:   <p>Lookup the given font index in the color table. Deal moderately gracefully with error conditions.</p>
    */
    inline Color StyledTextIOReader_RTF::LookupColor (ReaderContext& readerContext, size_t index) const
    {
        if (readerContext.fColorTable == nullptr) {
            HandleBadlyFormattedInput (); // Cannot have a \cfN without having specified a color table
            return Color::kBlack;
        }
        else {
            try {
                return (readerContext.fColorTable->LookupColor (static_cast<size_t> (index)));
            }
            catch (...) {
                return Color::kBlack;
            }
        }
    }

    //  class   StyledTextIOWriter_RTF::WriterContext
    inline StyledTextIOWriter_RTF::WriterContext::WriterContext (StyledTextIOWriter_RTF& writer)
        : fWriter (writer)
        , fInTable (false)
        , fSrcStream (writer.GetSrcStream ())
        , fLastEmittedISR (IncrementalFontSpecification (), 0)
        , fNextStyleChangeAt (0)
        , fIthStyleRun (0)
        , fNextHidableTextChangeAt (size_t (-1))
        , fHidableTextRegionOpen (false)
        , fIthHidableTextRun (0)
        , fCharsToSkip (0)
    {
    }
    inline StyledTextIOWriter_RTF::WriterContext::WriterContext (WriterContext& parentContext, SrcStream& srcStream)
        : fWriter (parentContext.fWriter)
        , fInTable (true)
        , fSrcStream (srcStream)
        , fLastEmittedISR (IncrementalFontSpecification (), 0)
        , fNextStyleChangeAt (0)
        , fIthStyleRun (0)
        , fNextHidableTextChangeAt (size_t (-1))
        , fHidableTextRegionOpen (false)
        , fIthHidableTextRun (0)
        , fCharsToSkip (0)
    {
    }
    inline StyledTextIOWriter_RTF& StyledTextIOWriter_RTF::WriterContext::GetWriter () const
    {
        return fWriter;
    }
    inline StyledTextIOWriter_RTF::SrcStream& StyledTextIOWriter_RTF::WriterContext::GetSrcStream () const
    {
        return fSrcStream;
    }
    inline StyledTextIOWriter_RTF::SinkStream& StyledTextIOWriter_RTF::WriterContext::GetSinkStream () const
    {
        return fWriter.GetSinkStream ();
    }

    //  class   StyledTextIOWriter_RTF
    inline CodePage StyledTextIOWriter_RTF::GetCurrentOutputCharSetEncoding () const
    {
        return fCurrentOutputCharSetEncoding;
    }

}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_RTF_h_*/
