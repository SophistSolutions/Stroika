/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <algorithm>
#include <cctype>

#include "SyntaxColoring.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using Memory::SmallStackBuffer;

using FontChangeStyleMarker = SyntaxColoringMarkerOwner::FontChangeStyleMarker;
using ColoredStyleMarker    = SyntaxColoringMarkerOwner::ColoredStyleMarker;

/*
 ********************************************************************************
 ******************************** TrivialRGBSyntaxAnalyzer **********************
 ********************************************************************************
 */
void TrivialRGBSyntaxAnalyzer::AdjustLookBackRange (TextStore* /*ts*/, size_t* /*lookBackStart*/, size_t* /*lookBackTo*/) const
{
}

void TrivialRGBSyntaxAnalyzer::AddMarkers (TextStore* ts, TextInteractor* /*interactor*/, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const
{
    RequireNotNull (ts);
    Require (lookBackStart <= lookBackTo);

    size_t                      len = lookBackTo - lookBackStart;
    SmallStackBuffer<Led_tChar> buf (len);
    ts->CopyOut (lookBackStart, len, buf);
    for (size_t i = 0; i < len; ++i) {
        Led_tChar c = buf[i];
        if (c == 'r' or c == 'R') {
            Marker* m = new ColoredStyleMarker (Led_Color::kRed);
            ts->AddMarker (m, i + lookBackStart, 1, owner);
            if (appendNewMarkersToList != NULL) {
                appendNewMarkersToList->push_back (m);
            }
        }
        if (c == 'g' or c == 'G') {
            Marker* m = new ColoredStyleMarker (Led_Color::kGreen);
            ts->AddMarker (m, i + lookBackStart, 1, owner);
            if (appendNewMarkersToList != NULL) {
                appendNewMarkersToList->push_back (m);
            }
        }
        if (c == 'b' or c == 'B') {
            Marker* m = new ColoredStyleMarker (Led_Color::kBlue);
            ts->AddMarker (m, i + lookBackStart, 1, owner);
            if (appendNewMarkersToList != NULL) {
                appendNewMarkersToList->push_back (m);
            }
        }
    }
}

/*
 ********************************************************************************
 ************************* TableDrivenKeywordSyntaxAnalyzer *********************
 ********************************************************************************
 */
const Led_tChar* kCPlusPlusKeywordTable[] = {
    //nb: do to quirky lookup code, arrange table so no prefixing keyword comes before
    // longer one. Cuz we always match shortest one first!
    LED_TCHAR_OF ("#define"),
    LED_TCHAR_OF ("#ifdef"),
    LED_TCHAR_OF ("#ifndef"),
    LED_TCHAR_OF ("#if"),
    LED_TCHAR_OF ("#else"),
    LED_TCHAR_OF ("#elif"),
    LED_TCHAR_OF ("#endif"),
    LED_TCHAR_OF ("#include"),
    LED_TCHAR_OF ("#pragma"),
    LED_TCHAR_OF ("auto"),
    LED_TCHAR_OF ("break"),
    LED_TCHAR_OF ("bool"),
    LED_TCHAR_OF ("case"),
    LED_TCHAR_OF ("catch"),
    LED_TCHAR_OF ("char"),
    LED_TCHAR_OF ("const_cast"),
    LED_TCHAR_OF ("const"),
    LED_TCHAR_OF ("class"),
    LED_TCHAR_OF ("continue"),
    LED_TCHAR_OF ("default"),
    LED_TCHAR_OF ("delete"),
    LED_TCHAR_OF ("double"),
    LED_TCHAR_OF ("do"),
    LED_TCHAR_OF ("dynamic_cast"),
    LED_TCHAR_OF ("else"),
    LED_TCHAR_OF ("enum"),
    LED_TCHAR_OF ("explicit"),
    LED_TCHAR_OF ("extern"),
    LED_TCHAR_OF ("false"),
    LED_TCHAR_OF ("float"),
    LED_TCHAR_OF ("for"),
    LED_TCHAR_OF ("friend"),
    LED_TCHAR_OF ("goto"),
    LED_TCHAR_OF ("if"),
    LED_TCHAR_OF ("inline"),
    LED_TCHAR_OF ("int"),
    LED_TCHAR_OF ("inherited"),
    LED_TCHAR_OF ("long"),
    LED_TCHAR_OF ("main"),
    LED_TCHAR_OF ("mutable"),
    LED_TCHAR_OF ("namespace"),
    LED_TCHAR_OF ("new"),
    LED_TCHAR_OF ("nonvirtual"),
    LED_TCHAR_OF ("operator"),
    LED_TCHAR_OF ("public"),
    LED_TCHAR_OF ("private"),
    LED_TCHAR_OF ("protected"),
    LED_TCHAR_OF ("register"),
    LED_TCHAR_OF ("reinterpret_cast"),
    LED_TCHAR_OF ("return"),
    LED_TCHAR_OF ("signed"),
    LED_TCHAR_OF ("sizeof"),
    LED_TCHAR_OF ("short"),
    LED_TCHAR_OF ("struct"),
    LED_TCHAR_OF ("static"),
    LED_TCHAR_OF ("static_cast"),
    LED_TCHAR_OF ("switch"),
    LED_TCHAR_OF ("template"),
    LED_TCHAR_OF ("this"),
    LED_TCHAR_OF ("throw"),
    LED_TCHAR_OF ("true"),
    LED_TCHAR_OF ("try"),
    LED_TCHAR_OF ("typedef"),
    LED_TCHAR_OF ("typeid"),
    LED_TCHAR_OF ("typename"),
    LED_TCHAR_OF ("union"),
    LED_TCHAR_OF ("unsigned"),
    LED_TCHAR_OF ("using"),
    LED_TCHAR_OF ("virtual"),
    LED_TCHAR_OF ("volatile"),
    LED_TCHAR_OF ("void"),
    LED_TCHAR_OF ("while")};
TableDrivenKeywordSyntaxAnalyzer::KeywordTable TableDrivenKeywordSyntaxAnalyzer::kCPlusPlusKeywords (kCPlusPlusKeywordTable, NEltsOf (kCPlusPlusKeywordTable));

const Led_tChar* kVisualBasicKeywordTable[] = {
    //nb: do to quirky lookup code, arrange table so no prefixing keyword comes before
    // longer one. Cuz we always match shortest one first!
    LED_TCHAR_OF ("abs"),
    LED_TCHAR_OF ("add"),
    LED_TCHAR_OF ("+"),
    LED_TCHAR_OF ("and"),
    LED_TCHAR_OF ("array"),
    LED_TCHAR_OF ("asc"),
    LED_TCHAR_OF ("="),
    LED_TCHAR_OF ("AtEndOfLine"),
    LED_TCHAR_OF ("AtEndOfStream"),
    LED_TCHAR_OF ("Atn"),
    LED_TCHAR_OF ("Attributes"),
    LED_TCHAR_OF ("AvailableSpace"),
    LED_TCHAR_OF ("BuildPath"),
    LED_TCHAR_OF ("Call"),
    LED_TCHAR_OF ("Case"),
    LED_TCHAR_OF ("CBool"),
    LED_TCHAR_OF ("CByte"),
    LED_TCHAR_OF ("CCur"),
    LED_TCHAR_OF ("CDate"),
    LED_TCHAR_OF ("CDbl"),
    LED_TCHAR_OF ("Chr"),
    LED_TCHAR_OF ("CInt"),
    LED_TCHAR_OF ("Clear"),
    LED_TCHAR_OF ("CLng"),
    LED_TCHAR_OF ("Close"),
    LED_TCHAR_OF ("Column"),
    LED_TCHAR_OF ("CompareMode"),
    LED_TCHAR_OF ("&"),
    LED_TCHAR_OF ("Const"),
    LED_TCHAR_OF ("CopyFile"),
    LED_TCHAR_OF ("CopyFolder"),
    LED_TCHAR_OF ("Copy"),
    LED_TCHAR_OF ("Cos"),
    LED_TCHAR_OF ("Count"),
    LED_TCHAR_OF ("CreateFolder"),
    LED_TCHAR_OF ("CreateObject"),
    LED_TCHAR_OF ("CreateTextFile"),
    LED_TCHAR_OF ("CSng"),
    LED_TCHAR_OF ("CStr"),
    LED_TCHAR_OF ("DateAddFunction"),
    LED_TCHAR_OF ("DateCreated"),
    LED_TCHAR_OF ("DateDiff"),
    LED_TCHAR_OF ("DateLastAccessed"),
    LED_TCHAR_OF ("DateLastModified"),
    LED_TCHAR_OF ("DatePart"),
    LED_TCHAR_OF ("DateSerial"),
    LED_TCHAR_OF ("DateValue"),
    LED_TCHAR_OF ("Date"),
    LED_TCHAR_OF ("Day"),
    LED_TCHAR_OF ("DeleteFile"),
    LED_TCHAR_OF ("DeleteFolder"),
    LED_TCHAR_OF ("Delete"),
    LED_TCHAR_OF ("Description"),
    LED_TCHAR_OF ("Dictionary"),
    LED_TCHAR_OF ("Dim"),
    LED_TCHAR_OF ("Division"),
    LED_TCHAR_OF ("Do"),
    LED_TCHAR_OF ("DriveExists"),
    LED_TCHAR_OF ("DriveLetter"),
    LED_TCHAR_OF ("DriveType"),
    LED_TCHAR_OF ("Drives"),
    LED_TCHAR_OF ("Drive"),
    LED_TCHAR_OF ("Each"),
    LED_TCHAR_OF ("Empty"),
    LED_TCHAR_OF ("Eqv"),
    LED_TCHAR_OF ("Else"),
    LED_TCHAR_OF ("Err"),
    LED_TCHAR_OF ("Erase"),
    LED_TCHAR_OF ("Exists"),
    LED_TCHAR_OF ("Exit"),
    LED_TCHAR_OF ("Explicit"),
    LED_TCHAR_OF ("Exp"),
    LED_TCHAR_OF ("^"),
    LED_TCHAR_OF ("False"),
    LED_TCHAR_OF ("FileExists"),
    LED_TCHAR_OF ("FileSystemObject"),
    LED_TCHAR_OF ("FileSystem"),
    LED_TCHAR_OF ("Files"),
    LED_TCHAR_OF ("File"),
    LED_TCHAR_OF ("Filter"),
    LED_TCHAR_OF ("Fix"),
    LED_TCHAR_OF ("Folders"),
    LED_TCHAR_OF ("FolderExists"),
    LED_TCHAR_OF ("Folder"),
    LED_TCHAR_OF ("FormatCurrency"),
    LED_TCHAR_OF ("FormatDateTime"),
    LED_TCHAR_OF ("FormatNumber"),
    LED_TCHAR_OF ("FormatPercent"),
    LED_TCHAR_OF ("For"),
    LED_TCHAR_OF ("FreeSpace"),
    LED_TCHAR_OF ("Function"),
    LED_TCHAR_OF ("GetAbsolutePathName"),
    LED_TCHAR_OF ("GetBaseName"),
    LED_TCHAR_OF ("GetDriveName"),
    LED_TCHAR_OF ("GetDrive"),
    LED_TCHAR_OF ("GetExtensionName"),
    LED_TCHAR_OF ("GetFileName"),
    LED_TCHAR_OF ("GetFile"),
    LED_TCHAR_OF ("GetFolder"),
    LED_TCHAR_OF ("GetObject"),
    LED_TCHAR_OF ("GetParentFolderName"),
    LED_TCHAR_OF ("GetSpecialFolder"),
    LED_TCHAR_OF ("GetTempName"),
    LED_TCHAR_OF ("Hex"),
    LED_TCHAR_OF ("HelpContext"),
    LED_TCHAR_OF ("HelpFile"),
    LED_TCHAR_OF ("Hour"),
    LED_TCHAR_OF ("If"),
    LED_TCHAR_OF ("Imp"),
    LED_TCHAR_OF ("InputBox"),
    LED_TCHAR_OF ("InStrRev"),
    LED_TCHAR_OF ("InStr"),
    LED_TCHAR_OF ("Int"),
    LED_TCHAR_OF ("\\"),
    LED_TCHAR_OF ("IsArray"),
    LED_TCHAR_OF ("IsDate"),
    LED_TCHAR_OF ("IsEmpty"),
    LED_TCHAR_OF ("IsNull"),
    LED_TCHAR_OF ("IsNumeric"),
    LED_TCHAR_OF ("IsObject"),
    LED_TCHAR_OF ("IsReady"),
    LED_TCHAR_OF ("IsRootFolder"),
    LED_TCHAR_OF ("Is"),
    LED_TCHAR_OF ("Items"),
    LED_TCHAR_OF ("Item"),
    LED_TCHAR_OF ("Join"),
    LED_TCHAR_OF ("Keys"),
    LED_TCHAR_OF ("Key"),
    LED_TCHAR_OF ("LBound"),
    LED_TCHAR_OF ("LCase"),
    LED_TCHAR_OF ("Left"),
    LED_TCHAR_OF ("Len"),
    LED_TCHAR_OF ("Line"),
    LED_TCHAR_OF ("LoadPicture"),
    LED_TCHAR_OF ("Log"),
    LED_TCHAR_OF ("Loop"),
    LED_TCHAR_OF ("LTrim"),
    LED_TCHAR_OF ("Mid"),
    LED_TCHAR_OF ("Minute"),
    LED_TCHAR_OF ("Mod"),
    LED_TCHAR_OF ("MonthName"),
    LED_TCHAR_OF ("Month"),
    LED_TCHAR_OF ("MoveFile"),
    LED_TCHAR_OF ("MoveFolder"),
    LED_TCHAR_OF ("Move"),
    LED_TCHAR_OF ("MsgBox"),
    LED_TCHAR_OF ("*"),
    LED_TCHAR_OF ("Name"),
    LED_TCHAR_OF ("Next"),
    LED_TCHAR_OF ("-"),
    LED_TCHAR_OF ("Nothing"),
    LED_TCHAR_OF ("Not"),
    LED_TCHAR_OF ("Now"),
    LED_TCHAR_OF ("Null"),
    LED_TCHAR_OF ("Number"),
    LED_TCHAR_OF ("Oct"),
    LED_TCHAR_OF ("On"),
    LED_TCHAR_OF ("OpenAsTextStream"),
    LED_TCHAR_OF ("OpenTextFile"),
    LED_TCHAR_OF ("Option"),
    LED_TCHAR_OF ("Or"),
    LED_TCHAR_OF ("ParentFolder"),
    LED_TCHAR_OF ("Path"),
    LED_TCHAR_OF ("Private"),
    LED_TCHAR_OF ("Public"),
    LED_TCHAR_OF ("Raise"),
    LED_TCHAR_OF ("Randomize"),
    LED_TCHAR_OF ("ReadAll"),
    LED_TCHAR_OF ("ReadLine"),
    LED_TCHAR_OF ("Read"),
    LED_TCHAR_OF ("ReDim"),
    LED_TCHAR_OF ("RemoveAll"),
    LED_TCHAR_OF ("Remove"),
    LED_TCHAR_OF ("Rem"),
    LED_TCHAR_OF ("Replace"),
    LED_TCHAR_OF ("RGB"),
    LED_TCHAR_OF ("Right"),
    LED_TCHAR_OF ("Rnd"),
    LED_TCHAR_OF ("RootFolder"),
    LED_TCHAR_OF ("Round"),
    LED_TCHAR_OF ("RTrim"),
    LED_TCHAR_OF ("ScriptEngineBuildVersion"),
    LED_TCHAR_OF ("ScriptEngineMajorVersion"),
    LED_TCHAR_OF ("ScriptEngineMinorVersion"),
    LED_TCHAR_OF ("ScriptEngine"),
    LED_TCHAR_OF ("Second"),
    LED_TCHAR_OF ("Select"),
    LED_TCHAR_OF ("SerialNumber"),
    LED_TCHAR_OF ("Set"),
    LED_TCHAR_OF ("Sgn"),
    LED_TCHAR_OF ("ShareName"),
    LED_TCHAR_OF ("ShortName"),
    LED_TCHAR_OF ("ShortPath"),
    LED_TCHAR_OF ("Sin"),
    LED_TCHAR_OF ("Size"),
    LED_TCHAR_OF ("SkipLine"),
    LED_TCHAR_OF ("Skip"),
    LED_TCHAR_OF ("Source"),
    LED_TCHAR_OF ("Space"),
    LED_TCHAR_OF ("Split"),
    LED_TCHAR_OF ("Sqr"),
    LED_TCHAR_OF ("StrComp"),
    LED_TCHAR_OF ("String"),
    LED_TCHAR_OF ("StrReverse"),
    LED_TCHAR_OF ("SubFolders"),
    LED_TCHAR_OF ("Sub"),
    LED_TCHAR_OF ("-"),
    LED_TCHAR_OF ("Tan"),
    LED_TCHAR_OF ("TextStream"),
    LED_TCHAR_OF ("TimeSerial"),
    LED_TCHAR_OF ("TimeValue"),
    LED_TCHAR_OF ("Time"),
    LED_TCHAR_OF ("Then"),
    LED_TCHAR_OF ("TotalSize"),
    LED_TCHAR_OF ("Trim"),
    LED_TCHAR_OF ("True"),
    LED_TCHAR_OF ("TypeName"),
    LED_TCHAR_OF ("Type"),
    LED_TCHAR_OF ("UBound"),
    LED_TCHAR_OF ("UCase"),
    LED_TCHAR_OF ("VarType"),
    LED_TCHAR_OF ("VolumeName"),
    LED_TCHAR_OF ("WeekdayName"),
    LED_TCHAR_OF ("Weekday"),
    LED_TCHAR_OF ("While"),
    LED_TCHAR_OF ("Wend"),
    LED_TCHAR_OF ("WriteBlankLines"),
    LED_TCHAR_OF ("WriteLine"),
    LED_TCHAR_OF ("Write"),
    LED_TCHAR_OF ("Xor"),
    LED_TCHAR_OF ("Year"),
};
TableDrivenKeywordSyntaxAnalyzer::KeywordTable TableDrivenKeywordSyntaxAnalyzer::kVisualBasicKeywords (kVisualBasicKeywordTable, NEltsOf (kVisualBasicKeywordTable), Led_tStrniCmp);

TableDrivenKeywordSyntaxAnalyzer::TableDrivenKeywordSyntaxAnalyzer (const KeywordTable& keyTable)
    : inherited ()
    , fKeywordTable (keyTable)
{
}

void TableDrivenKeywordSyntaxAnalyzer::AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const
{
    RequireNotNull (ts);
    RequireNotNull (lookBackStart);
    RequireNotNull (lookBackTo);
    Require (*lookBackStart <= *lookBackTo);

    // Now adjust for scanning algorithm sluff
    // for our trivial keyword stuff, only look back at most one keyword length
    *lookBackStart = max (0, int(*lookBackStart) - int(fKeywordTable.MaxKeywordLength () - 1));
    *lookBackTo    = min (ts->GetEnd (), *lookBackTo + fKeywordTable.MaxKeywordLength () - 1);
    Ensure (*lookBackStart <= *lookBackTo);
}

void TableDrivenKeywordSyntaxAnalyzer::AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const
{
    RequireNotNull (ts);
    RequireNotNull (interactor);
    Require (lookBackStart <= lookBackTo);

    size_t                      len = lookBackTo - lookBackStart;
    SmallStackBuffer<Led_tChar> buf (len);
    ts->CopyOut (lookBackStart, len, buf);
    bool preceedingCharKWDelimiter = false;
    for (size_t i = 0; i < len; ++i) {
        size_t kwl = fKeywordTable.KeywordLength (&buf[i], len - i);
        if (kwl != 0) {
            // Check if preceeding and following characters look like delimiters
            {
                size_t pc = ts->FindPreviousCharacter (i + lookBackStart);
                if (pc < i + lookBackStart) {
                    Led_tChar c;
                    ts->CopyOut (pc, 1, &c);
                    if (not isspace (c) and not ispunct (c)) {
                        continue;
                    }
                }
                if (i + lookBackStart + kwl < ts->GetEnd ()) {
                    Led_tChar c;
                    ts->CopyOut (i + lookBackStart + kwl, 1, &c);
                    if (not isspace (c) and not ispunct (c)) {
                        continue;
                    }
                }
            }

            Marker* m = new ColoredStyleMarker (Led_Color::kBlue);
            ts->AddMarker (m, i + lookBackStart, kwl, owner);
            // could be far more efficient, and keep markers in sorted order, but this is easier
            if (appendNewMarkersToList != NULL) {
                appendNewMarkersToList->push_back (m);
            }
            i += kwl - 1;
            interactor->Refresh (m, TextInteractor::eDelayedUpdate);
        }
    }
}

/*
 ********************************************************************************
 ************************** SyntaxColoringMarkerOwner ***************************
 ********************************************************************************
 */
SyntaxColoringMarkerOwner::SyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer)
    : inherited ()
    , fInteractor (interactor)
    , fTextStore (textStore)
    , fSyntaxAnalyzer (syntaxAnalyzer)
{
    fTextStore.AddMarkerOwner (this);
}

SyntaxColoringMarkerOwner::~SyntaxColoringMarkerOwner ()
{
    fTextStore.RemoveMarkerOwner (this);
}

TextStore* SyntaxColoringMarkerOwner::PeekAtTextStore () const
{
    return &fTextStore;
}

void SyntaxColoringMarkerOwner::RecheckAll ()
{
    RecheckRange (fTextStore.GetStart (), fTextStore.GetEnd ());
}

/*
 ********************************************************************************
 ************************* SimpleSyntaxColoringMarkerOwner **********************
 ********************************************************************************
 */
SimpleSyntaxColoringMarkerOwner::SimpleSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer)
    : inherited (interactor, textStore, syntaxAnalyzer)
    , fMarkers ()
{
}

SimpleSyntaxColoringMarkerOwner::~SimpleSyntaxColoringMarkerOwner ()
{
    GetTextStore ().RemoveMarkers (&(*fMarkers.begin ()), fMarkers.size ());
    for (size_t i = 0; i < fMarkers.size (); ++i) {
        delete (fMarkers[i]);
    }
    fInteractor.Refresh ();
}

void SimpleSyntaxColoringMarkerOwner::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    RecheckRange (updateInfo.fReplaceFrom, updateInfo.fReplaceFrom + updateInfo.fTextLength);
}

void SimpleSyntaxColoringMarkerOwner::RecheckRange (size_t updateFrom, size_t updateTo)
{
    Require (updateFrom <= updateTo);

    /*
     *  We must compute a bounds which must be re-analyzed (based on what was updated).
     *  Then we must walk our marker array, and eliminate any inappropriate ones. And we must
     *  the create any NEW appropriate ones.
     *
     *  How far back you must search will depend on the nature of your lexical analysis.
     */
    size_t lookBackStart = updateFrom;
    size_t lookBackTo    = updateTo;
    Assert (lookBackStart <= lookBackTo);
    fSyntaxAnalyzer.AdjustLookBackRange (&fTextStore, &lookBackStart, &lookBackTo);
    Assert (lookBackStart <= lookBackTo);

    // eliminate any invalidated markers.
    {
        for (size_t i = 0; i < fMarkers.size ();) {
            Marker* m = fMarkers[i];
            AssertNotNull (m);
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress : 28182)
#endif
            if (
                (lookBackStart <= m->GetStart () and m->GetEnd () <= lookBackTo) or m->GetLength () == 0) {
                // This update is needed cuz Led's normal typing update might not extend as far as the effect of
                // this change in styled text (coloring) might.
                fInteractor.Refresh (m, TextInteractor::eDelayedUpdate);

                // NB: we only remove them here, and don't invalidate their region cuz they were zero sized.
                // Invaling wouldn't hurt, but just a waste of time.
                fTextStore.RemoveMarker (m);
                delete m;
                fMarkers.erase (fMarkers.begin () + i);
                continue;
            }
            ++i;
        }
    }

    fSyntaxAnalyzer.AddMarkers (&fTextStore, &fInteractor, this, lookBackStart, lookBackTo, &fMarkers);
}

/*
 ********************************************************************************
 *************************** WindowedSyntaxColoringMarkerOwner ******************
 ********************************************************************************
 */
WindowedSyntaxColoringMarkerOwner::WindowedSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer)
    : inherited (interactor, textStore, syntaxAnalyzer)
    , fMarkers ()
    , fDeletedLines (false)
    , fCachedWindowStart (0)
    , fCachedWindowEnd (0)
{
}

WindowedSyntaxColoringMarkerOwner::~WindowedSyntaxColoringMarkerOwner ()
{
    GetTextStore ().RemoveMarkers (&(*fMarkers.begin ()), fMarkers.size ());
    for (size_t i = 0; i < fMarkers.size (); ++i) {
        delete (fMarkers[i]);
    }
    fInteractor.Refresh ();
}

void WindowedSyntaxColoringMarkerOwner::RecheckScrolling ()
{
    // could be smarter and cache old window boundaries, but lets cheap out, and do the simple thing
    // for now. After all, this is just a demo...
    //RecheckAll ();
    size_t windowStart = fInteractor.GetMarkerPositionOfStartOfWindow ();
    size_t windowEnd   = fInteractor.GetMarkerPositionOfEndOfWindow ();
    // Any amount scrolled OFF screen we don't need to check cuz the RecheckRange () algorithm will
    // automatically blow anything away it can off screen. All we need todo is
    // see what NEW stuff is being displayed, and assure AT LEAST THAT gets checked for new
    // hilight markers.
    size_t checkStart = windowStart;
    size_t checkEnd   = windowEnd;
    if (fCachedWindowEnd >= checkEnd and fCachedWindowStart < checkEnd) {
        checkEnd = fCachedWindowStart;
    }
    if (fCachedWindowStart <= checkStart and fCachedWindowEnd > checkStart) {
        checkStart = fCachedWindowEnd;
    }
    if (checkStart < checkEnd) {
        RecheckRange (checkStart, checkEnd);
    }
}

void WindowedSyntaxColoringMarkerOwner::AboutToUpdateText (const UpdateInfo& updateInfo)
{
    inherited::AboutToUpdateText (updateInfo);
    // If we're deleting any NLs, set flag, so on later DidUpdate (), we will recheck all, cuz we must
    // also check newly scrolled onto screen rows.
    if (updateInfo.fTextModified) {
        size_t                      len = updateInfo.fReplaceTo - updateInfo.fReplaceFrom;
        SmallStackBuffer<Led_tChar> buf (len);
        fTextStore.CopyOut (updateInfo.fReplaceFrom, len, buf);
        for (size_t i = 0; i < len; ++i) {
            Led_tChar c = buf[i];
            if (c == '\n') {
                fDeletedLines = true;
                break;
            }
        }
    }
}

void WindowedSyntaxColoringMarkerOwner::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    if (fDeletedLines) {
        fDeletedLines = false;
        RecheckAll ();
    }
    else {
        RecheckRange (updateInfo.fReplaceFrom, updateInfo.fReplaceFrom + updateInfo.fTextLength);
    }
}

void WindowedSyntaxColoringMarkerOwner::RecheckRange (size_t updateFrom, size_t updateTo)
{
    Require (updateFrom <= updateTo);

    /*
     *  We must compute a bounds which must be re-analyzed (based on what was updated).
     *  Then we must walk our marker array, and eliminate any inappropriate ones. And we must
     *  the create any NEW appropriate ones.
     *
     *  How far back you must search will depend on the nature of your lexical analysis.
     */
    size_t windowStart = fInteractor.GetMarkerPositionOfStartOfWindow ();
    size_t windowEnd   = fInteractor.GetMarkerPositionOfEndOfWindow ();

    size_t lookBackStart = updateFrom;
    size_t lookBackTo    = updateTo;

    // never bother looking outside the window
    lookBackStart = max (lookBackStart, windowStart);
    lookBackTo    = min (lookBackTo, windowEnd);
    lookBackTo    = max (lookBackTo, lookBackStart); // assure lookBackStart <= lookBackTo

    Assert (lookBackStart <= lookBackTo);
    fSyntaxAnalyzer.AdjustLookBackRange (&fTextStore, &lookBackStart, &lookBackTo);
    Assert (lookBackStart <= lookBackTo);

    // eliminate any invalidated markers.
    {
        for (size_t i = 0; i < fMarkers.size ();) {
            Marker* m = fMarkers[i];
            AssertNotNull (m);
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress : 28182)
#endif
            if (
                (lookBackStart <= m->GetStart () and m->GetEnd () <= lookBackTo) or (m->GetEnd () <= windowStart or m->GetStart () >= windowEnd) or m->GetLength () == 0) {

                // This update is needed cuz Led's normal typing update might not extend as far as the effect of
                // this change in styled text (coloring) might.
                fInteractor.Refresh (m, TextInteractor::eDelayedUpdate);

                // NB: we only remove them here, and don't invalidate their region cuz they were zero sized or
                // outside the window. Invaling wouldn't hurt, but just a waste of time.
                fTextStore.RemoveMarker (m);
                delete m;
                fMarkers.erase (fMarkers.begin () + i);
                continue;
            }
            ++i;
        }
    }

    fSyntaxAnalyzer.AddMarkers (&fTextStore, &fInteractor, this, lookBackStart, lookBackTo, &fMarkers);

    fCachedWindowStart = windowStart;
    fCachedWindowEnd   = windowEnd;
}

/*
 ********************************************************************************
 ******************************** ColoredStyleMarker ****************************
 ********************************************************************************
 */
Led_FontSpecification ColoredStyleMarker::MakeFontSpec (const StyledTextImager* imager, const RunElement& /*runElement*/) const
{
    RequireNotNull (imager);
    Led_FontSpecification fsp = imager->GetDefaultFont ();
    fsp.SetTextColor (fColor);
    return fsp;
}
