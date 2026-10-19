/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::Led
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/Common.h"

#include "Stroika/Frameworks/Led/SimpleTextStore.h"
#include "Stroika/Frameworks/Led/StandardStyledTextImager.h"
#include "Stroika/Frameworks/Led/StandardStyledTextInteractor.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_HTML.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_RTF.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using Characters::String;

#if qStroika_HasComponent_googletest
namespace {
    // c a f e-acute, space, euro sign - as code points, since C++ source stays ASCII
    constexpr char32_t kCafeEuro_[] = {'c', 'a', 'f', 0xE9, ' ', 0x20AC};

    String TextOf_ (const TextStore& ts)
    {
        vector<Led_tChar> buf (ts.GetLength ());
        ts.CopyOut (0, buf.size (), buf.data ());
        return String{span<const Led_tChar>{buf}};
    }

    template <typename READER>
    String Read_ (const string& src)
    {
        SimpleTextStore                ts;
        shared_ptr<StyleDatabaseRep>   styles = Memory::MakeSharedPtr<StyleDatabaseRep> (ts);
        StyledTextIOSrcStream_Memory   source{src.data (), src.size ()};
        StandardStyledTextIOSinkStream sink{&ts, styles};
        READER                         reader{&source, &sink};
        reader.Read ();
        sink.Flush ();
        return TextOf_ (ts);
    }
}

namespace {
    /*
     *  RTF names its code page (\ansi, \mac, \ansicpgN), and gives the characters outside ASCII as \'xx bytes in it.
     */
    GTEST_TEST (Frameworks_Led, RTF_ReadsCodePageBytes_)
    {
        Debug::TraceContextBumper ctx{"RTF_ReadsCodePageBytes_"};
        EXPECT_EQ (Read_<StyledTextIOReader_RTF> (R"({\rtf1\ansi caf\'e9 \'80})"), String{span{kCafeEuro_}});
        EXPECT_EQ (Read_<StyledTextIOReader_RTF> (R"({\rtf1\ansi\ansicpg1252 caf\'e9 \'80})"), String{span{kCafeEuro_}});
        EXPECT_EQ (Read_<StyledTextIOReader_RTF> (R"({\rtf1\mac caf\'8e \'db})"), String{span{kCafeEuro_}});
    }

    /*
     *  Text written as RTF reads back as itself; each character outside ASCII is written as its code point, and then - for a
     *  reader that does not know those - its byte in the document's code page (\ansi: Windows-1252).
     */
    GTEST_TEST (Frameworks_Led, RTF_RoundTrip_)
    {
        Debug::TraceContextBumper    ctx{"RTF_RoundTrip_"};
        const String                 text{span{kCafeEuro_}};
        const wstring                textChars = text.As<wstring> ();
        SimpleTextStore              ts;
        shared_ptr<StyleDatabaseRep> styles = Memory::MakeSharedPtr<StyleDatabaseRep> (ts);
        ts.Replace (0, 0, textChars.data (), textChars.size ());
        StandardStyledTextIOSrcStream       source{&ts, styles};
        StyledTextIOWriterSinkStream_Memory sink;
        StyledTextIOWriter_RTF              writer{&source, &sink};
        writer.Write ();
        const string rtf{static_cast<const char*> (sink.PeekAtData ()), sink.GetLength ()};
        EXPECT_NE (rtf.find (R"(\'e9)"), string::npos) << rtf;
        EXPECT_NE (rtf.find (R"(\'80)"), string::npos) << rtf;
        EXPECT_EQ (Read_<StyledTextIOReader_RTF> (rtf), text) << rtf;
    }

    /*
     *  HTML's text - here with no charset named, so Windows-1252 - and its character entities.
     */
    GTEST_TEST (Frameworks_Led, HTML_ReadsText_)
    {
        Debug::TraceContextBumper ctx{"HTML_ReadsText_"};
        string                    html = "<html><body>caf";
        html += static_cast<char> (0xE9);
        html += " &euro;</body></html>";
        EXPECT_EQ (Read_<StyledTextIOReader_HTML> (html), String{span{kCafeEuro_}});
        EXPECT_EQ (Read_<StyledTextIOReader_HTML> ("<html><body>caf&eacute; &#8364;</body></html>"), String{span{kCafeEuro_}});
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
