/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <fstream>
#include <iostream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

constexpr wchar_t kOpenCodeTag[]  = L"<%";
constexpr wchar_t kCloseCodeTag[] = L"%>";
constexpr wchar_t kMagicWriteChar = '='; // if first char after open tag - treat as synonym for fResponse.write ()

namespace {
    using StandardCommandLineOptions::kHelp;
    const CommandLine::Option kFormGeneratorNameO_{.fSingleCharName   = 'n',
                                                   .fSupportsArgument = true,
                                                   .fHelpArgName      = "NAME"sv,
                                                   .fHelpOptionText   = "names the generated function (default Form_Generator)"sv};
    const CommandLine::Option kInputFileO_{.fRequired = true, .fHelpArgName = "INPUT-HTML-FILE"sv};
    const CommandLine::Option kOutputFileO_{.fRequired = true, .fHelpArgName = "OUTPUT-CPP-FILE"sv};
    const initializer_list<CommandLine::Option> kAllOptions_{kHelp, kFormGeneratorNameO_, kInputFileO_, kOutputFileO_};
}

class CompilerApp {
public:
    CompilerApp (const filesystem::path& inputFile, const filesystem::path& outputFile, const String& formGeneratorName)
        : fInputFile{inputFile}
        , fOutputFile{outputFile}
        , fFormGeneratorName{formGeneratorName}
    {
    }

public:
    /**
     *  false if it wrote no complete output: a file it could not open, or HTML whose code tags do not pair up
     */
    nonvirtual bool Run ()
    {
        fstream in;
        in.open (fInputFile, ios_base::in);
        if (!in) {
            cerr << "Error opening input file- aborting" << endl;
            return false;
        }
        fstream out;
        out.open (fOutputFile, ios_base::out);
        if (!out) {
            cerr << "Error opening output file- aborting" << endl;
            return false;
        }
        return ProcessFile_ (in, out);
    }

private:
    nonvirtual bool ProcessFile_ (istream& in, ostream& out)
    {
        wstring orig =
            Streams::BinaryToText::Reader::New (Streams::iostream::InputStreamFromStdIStream::New<std::byte> (in)).ReadAll ().As<wstring> ();

        out << "/*Auto-Generated C++ file from the Source HTML file '" << String{fInputFile} << "'*/" << endl;
        out << "void    " << fFormGeneratorName << " ()" << endl;
        out << "{" << endl;
        {
            bool inCode           = false;
            bool inMagicWriteMode = false;
            for (wstring::const_iterator i = orig.begin (); i != orig.end ();) {

                if (static_cast<size_t> (orig.end () - i) >= ::wcslen (kOpenCodeTag) and wstring (i, i + ::wcslen (kOpenCodeTag)) == kOpenCodeTag) {
                    if (inCode) {
                        cerr << "ERROR: Open inside context of open" << endl;
                        return false;
                    }
                    inCode = true;
                    i += ::wcslen (kOpenCodeTag);
                    if (i < orig.end () and *i == kMagicWriteChar) {
                        inMagicWriteMode = true;
                        out << "fResponse.write (";
                        i += 1; // skip magicwritechar
                    }
                    else {
                        inMagicWriteMode = false;
                    }
                }
                else if (static_cast<size_t> (orig.end () - i) >= ::wcslen (kCloseCodeTag) and wstring (i, i + ::wcslen (kCloseCodeTag)) == kCloseCodeTag) {
                    if (not inCode) {
                        cerr << "ERROR: Close CODE inside context of closed" << endl;
                        return false;
                    }
                    i += ::wcslen (kCloseCodeTag);
                    if (inMagicWriteMode) {
                        out << ");";
                    }
                    if (i < orig.end () and (*i == '\n' or *i == '\r')) {
                        out << endl;
                    }
                    inCode = false;
                }

                wstring::const_iterator next = FindNextInterestingThing_ (i, orig.end ());

                if (i == next) {
                    // Can only happen at EOF????
                    // THIS LOGIC IS WRONG - CLEANUP!!!! test case of empty code segment <%%>
                    ++i;
                }
                else {
                    if (inCode) {
                        out << String{wstring{i, next}};
                    }
                    else {
                        // Really long 'C' strings cause MSVC to barf - so break it up
                        const size_t kBigSize = 250;
                        if (next - i > kBigSize) {
                            // make sure we don't break in front of a backslash
                            size_t x = orig.find_first_not_of ('\\', i - orig.begin () + kBigSize);
                            if (x < static_cast<size_t> (next - orig.begin ())) {
                                Assert (x >= i - orig.begin () + kBigSize);
                                next = orig.begin () + x;
                            }
                        }

                        out << "\tfResponse.write (";
                        WriteCCodeString_ (out, wstring (i, next));
                        out << ");" << endl;
                    }

                    i = next;
                }
            }
        }
        out << "}" << endl;
        return true;
    }

private:
    nonvirtual void WriteCCodeString_ (ostream& out, const wstring& s)
    {
        out << "L\"";
        for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
            if (*i == '\\') {
                out << "\\\\";
            }
            else if (*i == '"') {
                out << "\\\"";
            }
            else if (isascii (*i) and ((*i == ' ') or isalnum (*i) or ispunct (*i))) {
                out << static_cast<char> (*i);
            }
            else if (*i == '\t') {
                out << "\\t";
            }
            else if (*i == '\r') {
                out << "\\r";
            }
            else if (*i == '\n') {
                out << "\\n";
            }
            else {
                char buf[1024];
                (void)::snprintf (buf, std::size (buf), "\" L\"\\x%x\" L\"", *i);
                out << buf;
            }
        }
        out << "\"";
    }

private:
    wstring::const_iterator FindNextInterestingThing_ (wstring::const_iterator start, wstring::const_iterator end) const
    {
        const wstring kOPEN     = kOpenCodeTag;
        const size_t  kOPEN_LEN = kOPEN.length ();
        Assert (kOPEN_LEN != 0);
        const wstring kCLOSE     = kCloseCodeTag;
        const size_t  kCLOSE_LEN = kCLOSE.length ();
        Assert (kCLOSE_LEN != 0);

        for (wstring::const_iterator i = start; i != end; ++i) {
            if (static_cast<size_t> (end - i) >= kOPEN_LEN and wstring (i, i + kOPEN_LEN) == kOPEN) {
                return i;
            }
            else if (static_cast<size_t> (end - i) >= kCLOSE_LEN and wstring (i, i + kCLOSE_LEN) == kCLOSE) {
                return i;
            }
            if (*i == '\r') {
                if (i < end and *(i + 1) == '\n') {
                    return i + 2;
                }
                else {
                    return i + 1;
                }
            }
            if (*i == '\n') {
                return i + 1;
            }
        }
        return end;
    }

private:
    filesystem::path fInputFile;
    filesystem::path fOutputFile;
    String           fFormGeneratorName;
};

int main (int argc, const char* argv[])
{
    CommandLine                       cmdLine{argc, argv};
    Debug::TraceContextBumper         ctx{"main", "argv={}"_f, cmdLine};
    CommandLine::ExpectedOptionValues args = cmdLine.ApplyOptionsQuietly (kAllOptions_);
    if (not args) {
        cerr << Characters::ToString (args.error ()) << endl;
        cerr << cmdLine.GenerateUsage (kAllOptions_);
        return EXIT_FAILURE;
    }
    if (args->Has (kHelp)) {
        cerr << cmdLine.GenerateUsage (kAllOptions_);
        return EXIT_SUCCESS;
    }
    CompilerApp app{ValueOf (args->GetArgument (kInputFileO_)).As<filesystem::path> (),
                    ValueOf (args->GetArgument (kOutputFileO_)).As<filesystem::path> (),
                    args->GetArgument (kFormGeneratorNameO_).value_or ("Form_Generator"sv)};
    return app.Run () ? EXIT_SUCCESS : EXIT_FAILURE;
}
