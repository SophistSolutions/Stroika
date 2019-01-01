/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <fstream>
#include <iostream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/StringException.h"
#include "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include "Stroika/Foundation/Streams/iostream/Utilities.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

using Execution::StringException;

const wchar_t kOpenCodeTag[]  = L"<%";
const wchar_t kCloseCodeTag[] = L"%>";
const wchar_t kMagicWriteChar = '='; // if first char after open tag - treat as synonym for fResponse.write ()

class CompilerApp {
public:
    CompilerApp ()
        : fInputFile ()
        , fOutputFile ()
        , fFormGeneratorName (SDKSTR ("Form_Generator"))
    {
    }

public:
    nonvirtual void Run (int argc, const SDKChar* argv[])
    {
        if (ParseArgs_ (argc, argv)) {
            fstream in;
            in.open (fInputFile.c_str (), ios_base::in);
            if (!in) {
                cerr << "Error opening input file- aborting" << endl;
                return;
            }
            fstream out;
            out.open (fOutputFile.c_str (), ios_base::out);
            if (!out) {
                cerr << "Error opening output file- aborting" << endl;
                return;
            }
            ProcessFile_ (in, out);
        }
    }

private:
    nonvirtual void Usage_ ()
    {
        cerr << "Usage: HTMLViewCompiler [options]? InputHTMLFile OutputCFile\n";
        cerr << "\twhere [options]can be:\n";
        cerr << "\t\t-help to print out this message\n";
        cerr << "\t\t-n X set FormGenerator functioname to X\n";
    }

private:
    nonvirtual bool ParseArgs_ (int argc, const SDKChar* argv[])
    {
        int  fileCount   = 0;
        bool gettingName = false;
        for (int i = 1; i < argc; ++i) {
            const SDKChar* argi = argv[i];
            if (argi != NULL) {
                if (argi[0] == '-' or argi[0] == '/') {
                    size_t  optCharIdx = 1;
                    SDKChar optChar    = argi[optCharIdx];
                    if (optChar == '-' and argi[0] == '-') {
                        optCharIdx++;
                        optChar = argi[optCharIdx];
                    }
                    switch (optChar) {
                        case 'h':
                        case '?': {
                            Usage_ ();
                            return false;
                        } break;
                        case 'n':
                            gettingName = true;
                            break;
                        default: {
                            cerr << "Unknown argument '" << String::FromSDKString (argi).AsNarrowSDKString () << "'\n";
                            Usage_ ();
                            return false;
                        } break;
                    }
                }
                else if (gettingName) {
                    fFormGeneratorName = argi;
                    gettingName        = false;
                    break;
                }
                else {
                    switch (fileCount) {
                        case 0:
                            fInputFile = argi;
                            break;
                        case 1:
                            fOutputFile = argi;
                            break;
                        default:
                            cerr << "Too many files" << endl;
                            Usage_ ();
                            return false;
                    }
                    fileCount++;
                }
            }
        }
        if (fileCount < 2) {
            cerr << "Not enuf files specified." << endl;
            Usage_ ();
            return false;
        }
        return true;
    }

private:
    nonvirtual void ProcessFile_ (istream& in, ostream& out)
    {
        wstring orig = Streams::iostream::ReadTextStream (in);

        out << "/*Auto-Generated C++ file from the Source HTML file '" << SDKString2NarrowSDK (fInputFile) << "'*/" << endl;
        out << "void    " << SDKString2NarrowSDK (fFormGeneratorName) << " ()" << endl;
        out << "{" << endl;
        {
            bool inCode           = false;
            bool inMagicWriteMode = false;
            for (wstring::const_iterator i = orig.begin (); i != orig.end ();) {

                if (static_cast<size_t> (orig.end () - i) >= ::wcslen (kOpenCodeTag) and wstring (i, i + ::wcslen (kOpenCodeTag)) == kOpenCodeTag) {
                    if (inCode) {
                        cerr << "ERROR: Open inside context of open" << endl;
                        return;
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
                        return;
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
                        out << WideStringToNarrowSDKString (wstring (i, next));
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
                (void)::snprintf (buf, NEltsOf (buf), "\" L\"\\x%x\" L\"", *i);
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
    SDKString fInputFile;
    SDKString fOutputFile;
    SDKString fFormGeneratorName;
};

#if defined(__TCHAR_DEFINED)
int _tmain (int argc, const _TCHAR* argv[])
#else
int main (int argc, const char* argv[])
#endif
{
    CompilerApp app;
    app.Run (argc, argv);
    return 0;
}
