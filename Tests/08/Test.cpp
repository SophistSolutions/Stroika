/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::JSON
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/STL/VectorUtils.h"
#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchangeFormat/BadFormatException.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"
#include    "Stroika/Foundation/Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"
#include    "Stroika/Foundation/Streams/iostream/TextInputStreamFromIStreamAdapter.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;

using   Memory::VariantValue;

using   Streams::iostream::BinaryInputStreamFromIStreamAdapter;


/*
 * Validating JSON parse results:
 *      http://json.parser.online.fr/
 */







namespace   {
    void    CheckMatchesExpected_WRITER_ (const VariantValue& v, const string& expected)
    {
        Streams::BasicBinaryOutputStream    out;
        DataExchangeFormat::JSON::PrettyPrint (v, out);
        string x = out.As<string> ();
        for (string::size_type i = 0; i < min (x.length (), expected.length ()); ++i) {
            if (x[i] != expected[i]) {
                VerifyTestResult (false);
            }
        }
        VerifyTestResult (out.As<string> () == expected);
    }

    void    DoRegressionTests_Writer_ ()
    {
        {
            VariantValue    v1 = L"hello world";
            CheckMatchesExpected_WRITER_ (v1, "\"hello world\"");
        }
        {
            VariantValue    v1 =    3;
            CheckMatchesExpected_WRITER_ (v1, "3");
        }
        {
            VariantValue    v1 =    4.7;
            CheckMatchesExpected_WRITER_ (v1, "4.7");
        }
        {
            VariantValue    v1 =    L"\"";
            CheckMatchesExpected_WRITER_ (v1, "\"\\\"\"");
        }
        {
            // array
            vector<VariantValue>    v;
            v.push_back (3);
            v.push_back (7);
            v.push_back (L"cookie");
            VariantValue    v1 =    v;
            CheckMatchesExpected_WRITER_ (v1, "[\n    3,\n    7,\n    \"cookie\"\n]");
        }
        {
            // object
            map<wstring, VariantValue>   v;
            v[L"Arg1"] = 32;
            v[L"Arg2"] = L"Cookies";
            v[L"Arg3"] = Containers::STL::mkV<VariantValue> (19);
            VariantValue    v1 =    v;
            CheckMatchesExpected_WRITER_ (v1, "{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}");
        }
    }
}




namespace   {
    void    CheckMatchesExpected_READER_ (const string& v, const VariantValue& expected)
    {
        stringstream    tmp;
        tmp << v;
        VariantValue    v1  =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tmp));
        VerifyTestResult (v1.GetType () == expected.GetType ());
        VerifyTestResult (v1 == expected);
    }

    void    DoRegressionTests_Reader_ ()
    {
        {
            VariantValue    v1 = L"hello world";
            CheckMatchesExpected_READER_ ("\"hello world\"", v1);
        }
        {
            VariantValue    v1 =    3;
            CheckMatchesExpected_READER_ ("3", v1);
        }
        {
            VariantValue    v1 =    L"\uFDD0";
            CheckMatchesExpected_READER_ ("\"\\uFDD0\"", v1);
        }
        {
            VariantValue    v1 =    4.7;
            CheckMatchesExpected_READER_ ("4.7", v1);
        }
        {
            // array
            vector<VariantValue>    v;
            v.push_back (3);
            v.push_back (7);
            v.push_back (L"cookie");
            VariantValue    v1 =    v;
            CheckMatchesExpected_READER_ ("[\n    3,\n    7,\n    \"cookie\"\n]", v1);
        }
        {
            // object
            map<wstring, VariantValue>   v;
            v[L"Arg1"] = 32;
            v[L"Arg2"] = L"Cookies";
            v[L"Arg3"] = Containers::STL::mkV<VariantValue> (19);
            VariantValue    v1 =    v;
            CheckMatchesExpected_READER_ ("{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}", v1);
        }
        {
            // Bug found in another JSON reader (sent me by Ryan - 2011-07-27)
            const   string  kExample    =   "{\"nav_items\":[{\"main_link\":{\"href\":\"/about/index.html\",\"text\":\"Who We Are\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/company-management.html\",\"text\":\"Management\"}},{\"header\":{\"href\":\"/about/mission-statement.html\",\"text\":\"Mission\"}},{\"header\":{\"href\":\"/about/company-history.html\",\"text\":\" History\"}},{\"header\":{\"href\":\"/about/headquarters.html\",\"text\":\"Corporate Headquarters\"}},{\"header\":{\"href\":\"/about/diversity.html\",\"text\":\"Diversity\"}},{\"header\":{\"href\":\"/about/supplier-diversity.html\",\"text\":\"Supplier Diversity\"}}]}]},{\"main_link\":{\"href\":\"http://investor.compuware.com\",\"text\":\"Investor Relations\"}},{\"main_link\":{\"href\":\"/about/newsroom.html\",\"text\":\"News Room\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/analyst-reports\",\"text\":\"Analyst Reports\"}},{\"header\":{\"href\":\"/about/awards-recognition.html\",\"text\":\"Awards and Recognition\"}},{\"header\":{\"href\":\"/about/blogs.html\",\"text\":\"Blog Home\"}},{\"header\":{\"href\":\"/about/press-analyst-contacts.html\",\"text\":\"Contact Us\"}},{\"header\":{\"href\":\"/about/customers.html\",\"text\":\"Customers\"}},{\"header\":{\"href\":\"/about/press-mentions\",\"text\":\"Press Mentions\"}},{\"header\":{\"href\":\"/about/press-releases\",\"text\":\"Press Releases\"}},{\"header\":{\"href\":\"/about/press-resources.html\",\"text\":\"Press Resources\"}}]}]},{\"main_link\":{\"href\":\"#top\",\"text\":\"Sponsorships\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"/about/lemans-sponsorship.html\",\"text\":\"Le Mans\"}},{\"header\":{\"href\":\"/about/nhl-sponsorship.html\",\"text\":\"NHL\"}},{}]}]},{\"main_link\":{\"href\":\"/about/community-involvement.html\",\"text\":\"Community Involvement\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"http://communityclicks.compuware.com\",\"text\":\"Community Clicks Blog\"}},{\"header\":{\"href\":\"javascript:securenav('/forms/grant-eligibility-form.html')\",\"text\":\"Grant Eligibility Form\"}},{}]}]},{\"main_link\":{\"href\":\"/government/\",\"text\":\"Government\"}}]}";
            stringstream    tmp;
            tmp << kExample;
            VariantValue    v1  =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tmp));
            VerifyTestResult (v1.GetType () == VariantValue::Type::eMap);
        }

    }
}


namespace   {
    void    VerifyThisStringFailsToParse_ (const string& s)
    {
        stringstream    tmp;
        tmp << s;
        try {
            VariantValue    v1  =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tmp));
            VerifyTestResult (false);   // should get exception
        }
        catch (const DataExchangeFormat::BadFormatException&) {
            // GOOD
        }
        catch (...) {
            VerifyTestResult (false);   // should get BadFormatException
        }
    }
    void    CheckCanReadFromSmallBadSrc_ ()
    {
        VerifyThisStringFailsToParse_ ("n");
        VerifyThisStringFailsToParse_ ("'");
        VerifyThisStringFailsToParse_ ("\"");
        VerifyThisStringFailsToParse_ ("[");
        VerifyThisStringFailsToParse_ ("}");
        VerifyThisStringFailsToParse_ ("]");
    }
}




namespace   {

    void    CheckRoundtrip_encode_decode_unchanged (const VariantValue& v)
    {
        string  encodedRep;
        {
            Streams::BasicBinaryOutputStream    out;
            DataExchangeFormat::JSON::PrettyPrint (v, out);
            encodedRep = out.As<string> ();
        }
        {
            stringstream    tmp;
            tmp << encodedRep;
            VariantValue    vOut    =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tmp));
            VerifyTestResult (vOut.GetType () == v.GetType ());
            VerifyTestResult (vOut == v);
        }
    }

    void    CheckStringQuoting_ ()
    {
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"cookie"));
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"c:\\"));
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"'"));
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\""));
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\\u20a9")); //  ₩
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\u20a9"));  //  ₩
        CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\"apple\""));
    }
}



namespace {
    void    ParseRegressionTest_1_ ()
    {
        {
            const char  kJSONExample_[] =
                "{"
                "    \"Automated Backups\" : {"
                "        \"From\" : {"
                "            \"CurrentHRWildcard\" : true,"
                "            \"PrintName\" : \"{Current HR}\""
                "        },"
                "        \"LastRanAt\" : {"
                "            \"ID-ca22f72c-9ff5-4082-82d0-d9763c64ddd6\" : \"2013-03-03T13:53:05-05:00\""
                "        },"
                "        \"Operation\" : 0,"
                "        \"Output\" : {"
                "            \"AttachmentPolicy\" : 2,"
                "            \"Format\" : \"application/x-healthframe-snapshotphr-3\","
                "            \"MaxFiles\" : 0,"
                "            \"NamePolicy\" : 1,"
                "            \"Password\" : \"\""
                "        },"
                "        \"PolicyName\" : \"Automated Backups\","
                "        \"Schedule\" : 2,"
                "        \"To\" : {"
                "            \"DefaultBackupDirectory\" : true,"
                "            \"PrintName\" : \"{Default Backup Directory}\""
                "        }"
                "    }"
                "}"
                ;
            Memory::VariantValue v = DataExchangeFormat::JSON::Reader (Streams::ExternallyOwnedMemoryBinaryInputStream (reinterpret_cast<const Byte*> (StartOfArray (kJSONExample_)), reinterpret_cast<const Byte*> (StartOfArray (kJSONExample_)) + strlen (kJSONExample_)));
            map<wstring, VariantValue>  mv  =   v.As<map<wstring, VariantValue>> ();
            VerifyTestResult (mv[L"Automated Backups"].GetType () == Memory::VariantValue::Type::eMap);
            map<wstring, VariantValue>  outputMap   =   v.As<map<wstring, VariantValue>> ()[L"Output"].As<map<wstring, VariantValue>> ();
            outputMap[L"MaxFiles"] = 123456789;
            mv[L"Output"] = outputMap;
            v = mv;

            string  jsonExampleWithUpdatedMaxFilesReference;
            {
                Streams::BasicBinaryOutputStream    tmpStrm;
                DataExchangeFormat::JSON::PrettyPrint (v, tmpStrm);
                jsonExampleWithUpdatedMaxFilesReference = tmpStrm.As<string> ();
            }
            {
                // Verify change of locale has no effect on results
                locale  prevLocale  =   locale::global (locale ("C"));
                Streams::BasicBinaryOutputStream    tmpStrm;
                DataExchangeFormat::JSON::PrettyPrint (v, tmpStrm);
                VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                locale::global (prevLocale);
            }
            {
                // Verify change of locale has no effect on results
                locale  prevLocale  =   locale::global (Configuration::FindNamedLocale (L"en", L"us"));
                Streams::BasicBinaryOutputStream    tmpStrm;
                DataExchangeFormat::JSON::PrettyPrint (v, tmpStrm);
                VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                locale::global (prevLocale);
            }

        }
    }
}

namespace {
    void    ParseRegressionTest_2_ ()
    {
        auto f = [] () {
            map<wstring, VariantValue>  mv;
            mv[L"MaxFiles"] = Memory::VariantValue (405);
            Memory::VariantValue    v   =    Memory::VariantValue (mv);

            string  encoded;
            {
                stringstream    tmpStrm;
                DataExchangeFormat::JSON::PrettyPrint (v, tmpStrm);
                encoded = tmpStrm.str ();
            }
            stringstream    tnmStrStrm (encoded);
            VariantValue    v1  =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tnmStrStrm));
            VerifyTestResult (v1 == v);
        };
        f ();
        {
            locale  prevLocale  =   locale::global (Configuration::FindNamedLocale (L"en", L"us"));
            f ();
            locale::global (prevLocale);
        }
    }
}

namespace {
    void    Parser_TestReadWriteBasictypes_3_ ()
    {
        using   namespace   Time;
        auto f = [] (VariantValue v) {
            string  encoded;
            {
                stringstream    tmpStrm;
                DataExchangeFormat::JSON::PrettyPrint (v, tmpStrm);
                encoded = tmpStrm.str ();
            }
            stringstream    tnmStrStrm (encoded);
            VariantValue    v1  =   DataExchangeFormat::JSON::Reader (BinaryInputStreamFromIStreamAdapter (tnmStrStrm));
            // JSON reader comes back with strings - because date/datetime are not native types
            if (v.GetType () == VariantValue::Type::eDate and v1.GetType () == VariantValue::Type::eString) {
                v1 = VariantValue (v1.As<Time::Date> ());
            }
            if (v.GetType () == VariantValue::Type::eDateTime and v1.GetType () == VariantValue::Type::eString) {
                v1 = VariantValue (v1.As<Time::DateTime> ());
            }
            VerifyTestResult (v1 == v);
        };
        f (Memory::VariantValue (405));
        f (Memory::VariantValue (L"'"));
        f (Memory::VariantValue (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12)));
        f (Memory::VariantValue (DateTime (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12), TimeOfDay (432))));
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        DoRegressionTests_Writer_ ();
        DoRegressionTests_Reader_ ();
        CheckCanReadFromSmallBadSrc_ ();
        CheckStringQuoting_ ();
        ParseRegressionTest_1_ ();
        ParseRegressionTest_2_ ();
        Parser_TestReadWriteBasictypes_3_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

