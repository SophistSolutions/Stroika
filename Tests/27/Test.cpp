/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Reader/Writers(JSON/XML)
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchange/BadFormatException.h"
#include    "Stroika/Foundation/DataExchange/INI/Reader.h"
#include    "Stroika/Foundation/DataExchange/INI/Writer.h"
#include    "Stroika/Foundation/DataExchange/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/DataExchange/XML/Reader.h"
#include    "Stroika/Foundation/DataExchange/XML/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Streams/BasicBinaryInputStream.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "Stroika/Foundation/Math/Common.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Characters::String;
using   DataExchange::VariantValue;
using   Memory::Byte;



///// @todo ADD SEPEARET MODULE TO TEST VARIANTVALUE!!!



/*
 * Validating JSON parse results:
 *      http://json.parser.online.fr/
 */








namespace {
    namespace INI_ONLY_ {

        void    DoBasicReader1_ ()
        {
            stringstream    tmp;
            tmp << "NAME=\"Ubuntu\"" << endl;
            tmp << "VERSION=\"13.10, Saucy Salamander\"" << endl;
            tmp << "ID=ubuntu" << endl;
            tmp << "ID_LIKE=debian" << endl;
            tmp << "PRETTY_NAME=\"Ubuntu 13.10\"" << endl;
            tmp << "VERSION_ID=\"13.10\"" << endl;
            tmp << "HOME_URL=\"http://www.ubuntu.com/\"" << endl;
            tmp << "SUPPORT_URL=\"http://help.ubuntu.com/\"" << endl;
            tmp << "BUG_REPORT_URL=\"http://bugs.launchpad.net/ubuntu/\"" << endl;
            INI::Profile p = INI::Reader ().ReadProfile (tmp);
            VerifyTestResult (p.fNamedSections.empty ());
            VerifyTestResult (p.fUnnamedSection.fProperties.LookupValue (L"NAME") == L"Ubuntu");
            VerifyTestResult (p.fUnnamedSection.fProperties.LookupValue (L"SUPPORT_URL") == L"http://help.ubuntu.com/");
        }

        void    DoAll_ ()
        {
            DoBasicReader1_ ();
        }
    }
}



namespace   {
    namespace JSON_ONLY_ {
        /*
         *  This section is for using the direct - JSON-only APIs, and verifying the results look like good JSON
         */


        namespace Test_01_BasicWriterTests_ {
            void    CheckMatchesExpected_WRITER_ (const VariantValue& v, const string& expected)
            {
                Streams::BasicBinaryOutputStream    out;
                DataExchange::JSON::Writer ().Write (v, out);
                string x = out.As<string> ();
                for (string::size_type i = 0; i < min (x.length (), expected.length ()); ++i) {
                    if (x[i] != expected[i]) {
                        VerifyTestResult (false);
                    }
                }
                VerifyTestResult (out.As<string> () == expected);
            }
            void    DoIt ()
            {
                {
                    VariantValue    v1 = L"hello world";
                    CheckMatchesExpected_WRITER_ (v1, "\"hello world\"\n");
                }
                {
                    VariantValue    v1 =    3;
                    CheckMatchesExpected_WRITER_ (v1, "3\n");
                }
                {
                    VariantValue    v1 =    4.7;
                    CheckMatchesExpected_WRITER_ (v1, "4.7\n");
                }
                {
                    VariantValue    v1 =    L"\"";
                    CheckMatchesExpected_WRITER_ (v1, "\"\\\"\"\n");
                }
                {
                    // array
                    vector<VariantValue>    v;
                    v.push_back (3);
                    v.push_back (7);
                    v.push_back (L"cookie");
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_WRITER_ (v1, "[\n    3,\n    7,\n    \"cookie\"\n]\n");
                }
                {
                    // object
                    map<wstring, VariantValue>   v;
                    v[L"Arg1"] = 32;
                    v[L"Arg2"] = L"Cookies";
                    v[L"Arg3"] = Containers::Sequence<VariantValue> ({ 19 });
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_WRITER_ (v1, "{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}\n");
                }
            }
        }


        namespace Test_02_BasicReaderTests_ {
            void    CheckMatchesExpected_READER_ (const string& v, const VariantValue& expected)
            {
                stringstream    tmp;
                tmp << v;
                VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                VerifyTestResult (v1 == expected);
            }

            void    DoIt ()
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
                    v[L"Arg3"] = Containers::Sequence<VariantValue> ({ 19 });
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_READER_ ("{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}", v1);
                }
                {
                    // Bug found in another JSON reader (sent me by Ryan - 2011-07-27)
                    const   string  kExample    =   "{\"nav_items\":[{\"main_link\":{\"href\":\"/about/index.html\",\"text\":\"Who We Are\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/company-management.html\",\"text\":\"Management\"}},{\"header\":{\"href\":\"/about/mission-statement.html\",\"text\":\"Mission\"}},{\"header\":{\"href\":\"/about/company-history.html\",\"text\":\" History\"}},{\"header\":{\"href\":\"/about/headquarters.html\",\"text\":\"Corporate Headquarters\"}},{\"header\":{\"href\":\"/about/diversity.html\",\"text\":\"Diversity\"}},{\"header\":{\"href\":\"/about/supplier-diversity.html\",\"text\":\"Supplier Diversity\"}}]}]},{\"main_link\":{\"href\":\"http://investor.compuware.com\",\"text\":\"Investor Relations\"}},{\"main_link\":{\"href\":\"/about/newsroom.html\",\"text\":\"News Room\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/analyst-reports\",\"text\":\"Analyst Reports\"}},{\"header\":{\"href\":\"/about/awards-recognition.html\",\"text\":\"Awards and Recognition\"}},{\"header\":{\"href\":\"/about/blogs.html\",\"text\":\"Blog Home\"}},{\"header\":{\"href\":\"/about/press-analyst-contacts.html\",\"text\":\"Contact Us\"}},{\"header\":{\"href\":\"/about/customers.html\",\"text\":\"Customers\"}},{\"header\":{\"href\":\"/about/press-mentions\",\"text\":\"Press Mentions\"}},{\"header\":{\"href\":\"/about/press-releases\",\"text\":\"Press Releases\"}},{\"header\":{\"href\":\"/about/press-resources.html\",\"text\":\"Press Resources\"}}]}]},{\"main_link\":{\"href\":\"#top\",\"text\":\"Sponsorships\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"/about/lemans-sponsorship.html\",\"text\":\"Le Mans\"}},{\"header\":{\"href\":\"/about/nhl-sponsorship.html\",\"text\":\"NHL\"}},{}]}]},{\"main_link\":{\"href\":\"/about/community-involvement.html\",\"text\":\"Community Involvement\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"http://communityclicks.compuware.com\",\"text\":\"Community Clicks Blog\"}},{\"header\":{\"href\":\"javascript:securenav('/forms/grant-eligibility-form.html')\",\"text\":\"Grant Eligibility Form\"}},{}]}]},{\"main_link\":{\"href\":\"/government/\",\"text\":\"Government\"}}]}";
                    stringstream    tmp;
                    tmp << kExample;
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (v1.GetType () == VariantValue::Type::eMap);
                }

            }
        }


        namespace Test_03_CheckCanReadFromSmallBadSrc_ {
            void    VerifyThisStringFailsToParse_ (const string& s)
            {
                stringstream    tmp;
                tmp << s;
                try {
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (false);   // should get exception
                }
                catch (const DataExchange::BadFormatException&) {
                    // GOOD
                }
                catch (...) {
                    VerifyTestResult (false);   // should get BadFormatException
                }
            }
            void    DoIt ()
            {
                VerifyThisStringFailsToParse_ ("n");
                VerifyThisStringFailsToParse_ ("'");
                VerifyThisStringFailsToParse_ ("\"");
                VerifyThisStringFailsToParse_ ("[");
                VerifyThisStringFailsToParse_ ("}");
                VerifyThisStringFailsToParse_ ("]");
            }
        }


        namespace Test_04_CheckStringQuoting_ {

            void    CheckRoundtrip_encode_decode_unchanged (const VariantValue& v)
            {
                string  encodedRep;
                {
                    Streams::BasicBinaryOutputStream    out;
                    DataExchange::JSON::Writer ().Write (v, out);
                    encodedRep = out.As<string> ();
                }
                {
                    stringstream    tmp;
                    tmp << encodedRep;
                    VariantValue    vOut = DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (vOut == v);
                }
            }

            void    DoIt ()
            {
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"cookie"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"c:\\"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"'"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\""));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\\u20a9")); //  ₩
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\u20a9"));  //  ₩
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\"apple\""));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long long int>::min ()));
#if 1
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long long int>::max ()));
#endif
            }
        }


        namespace Test_05_ParseRegressionTest_1_ {
            void    DoIt ()
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
                    VariantValue v = DataExchange::JSON::Reader ().Read (Streams::ExternallyOwnedMemoryBinaryInputStream (reinterpret_cast<const Byte*> (std::begin (kJSONExample_)), reinterpret_cast<const Byte*> (std::begin (kJSONExample_)) + strlen (kJSONExample_)));
                    map<wstring, VariantValue>  mv  =   v.As<map<wstring, VariantValue>> ();
                    VerifyTestResult (mv[L"Automated Backups"].GetType () == VariantValue::Type::eMap);
                    map<wstring, VariantValue>  outputMap   =   v.As<map<wstring, VariantValue>> ()[L"Output"].As<map<wstring, VariantValue>> ();
                    outputMap[L"MaxFiles"] = 123456789;
                    mv[L"Output"] = outputMap;
                    v = mv;

                    string  jsonExampleWithUpdatedMaxFilesReference;
                    {
                        Streams::BasicBinaryOutputStream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        jsonExampleWithUpdatedMaxFilesReference = tmpStrm.As<string> ();
                    }
                    {
                        // Verify change of locale has no effect on results
                        locale  prevLocale  =   locale::global (locale ("C"));
                        Streams::BasicBinaryOutputStream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                        locale::global (prevLocale);
                    }
                    {
                        // Verify change of locale has no effect on results
                        locale  prevLocale  =   locale::global (Configuration::FindNamedLocale (L"en", L"us"));
                        Streams::BasicBinaryOutputStream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                        locale::global (prevLocale);
                    }

                }
            }
        }


        namespace Test_06_ParseRegressionTest_2_ {
            void    DoIt ()
            {
                auto f = [] () {
                    map<wstring, VariantValue>  mv;
                    mv[L"MaxFiles"] = VariantValue (405);
                    VariantValue    v   =    VariantValue (mv);

                    string  encoded;
                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        encoded = tmpStrm.str ();
                    }
                    stringstream    tnmStrStrm (encoded);
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tnmStrStrm);
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


        namespace Test_07_ParserTestReadWriteBasictypes_ {
            void    DoIt ()
            {
                using   namespace   Time;
                auto f = [] (VariantValue v) {
                    string  encoded;
                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        encoded = tmpStrm.str ();
                    }
                    stringstream    tnmStrStrm (encoded);
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tnmStrStrm);
                    // JSON reader comes back with strings - because date/datetime are not native types
                    if (v.GetType () == VariantValue::Type::eDate and v1.GetType () == VariantValue::Type::eString) {
                        v1 = VariantValue (v1.As<Time::Date> ());
                    }
                    if (v.GetType () == VariantValue::Type::eDateTime and v1.GetType () == VariantValue::Type::eString) {
                        v1 = VariantValue (v1.As<Time::DateTime> ());
                    }
                    if (v.GetType () == VariantValue::Type::eFloat) {
                        VerifyTestResult (Math::NearlyEquals (v1.As<double> (), v.As<double> (), 0.001));
                    }
                    else {
                        VerifyTestResult (v1 == v);
                    }
                };
                auto    doAll = [f] () {
                    f (VariantValue (405));
                    f (VariantValue (4405));
                    f (VariantValue (44905));
                    f (VariantValue (405.1));
                    f (VariantValue (4405.2));
                    f (VariantValue (44905.3));
                    f (VariantValue (L"'"));
                    f (VariantValue (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12)));
                    f (VariantValue (DateTime (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12), TimeOfDay (432))));

                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (VariantValue (44905.3), tmpStrm);
                        string tmp = tmpStrm.str ();
                        VerifyTestResult (tmp.find (",") == string::npos);
                    }
                };
                {
                    doAll ();
                    locale  prevLocale  =   locale::global (Configuration::FindNamedLocale (L"en", L"us"));
                    doAll ();
                    locale::global (prevLocale);
                }
            }
        }


        namespace Test_08_ReadEmptyStreamShouldNotFail_ {
            void    DoIt ()
            {
                try {
                    VariantValue    vOut    =   DataExchange::JSON::Reader ().Read (Streams::BasicBinaryInputStream (nullptr, nullptr));
                    VerifyTestResult (false);
                }
                catch (const DataExchange::BadFormatException&) {
                    // Good - this should fail
                }
            }
        }


        void    DoAll_ ()
        {
            Test_01_BasicWriterTests_::DoIt ();
            Test_02_BasicReaderTests_::DoIt ();
            Test_03_CheckCanReadFromSmallBadSrc_::DoIt ();
            Test_04_CheckStringQuoting_::DoIt ();
            Test_05_ParseRegressionTest_1_::DoIt ();
            Test_06_ParseRegressionTest_2_::DoIt ();
            Test_07_ParserTestReadWriteBasictypes_::DoIt ();
            Test_08_ReadEmptyStreamShouldNotFail_::DoIt ();
        }
    }
}







namespace   {
    namespace XML_ONLY_ {
        /*
         *  This section is for using the direct - XML-only APIs, and verifying the results look like good XML
         */



        namespace Test_01_FirstPlayingAroundTest_ {
            void    DoIt ()
            {
                {
                    DataExchange::XML::Writer w;
                    VariantValue    v   =   VariantValue (44905.3);
                    Streams::BasicBinaryOutputStream    out;
                    w.Write (v, out);
                    string x = out.As<string> ();
                    int breakhere = 1;
                }
                {
                    DataExchange::XML::Writer w;
                    map<wstring, VariantValue>  mv;
                    mv[L"MaxFiles"] = VariantValue (405);
                    VariantValue    v   =    VariantValue (mv);
                    Streams::BasicBinaryOutputStream    out;
                    w.Write (v, out);
                    string x = out.As<string> ();
                    int breakhere = 1;
                }
            }
        }



        void    DoAll_ ()
        {
            Test_01_FirstPlayingAroundTest_::DoIt ();
        }
    }
}










namespace   {
    namespace GENERIC_SERIALIZE_DESERIALIZE_ {
        /*
         *  This section is for generic tests - that will be applied to BOTH the default JSON reader/writers
         *  and the default XML readers/writers (with no mods)
         */

        void    DoAll_ ()
        {



        }
    }
}



namespace {
    /// @TODO MOVE ELSEWHERE
    template <typename T>
    void Test3_VariantValue_Helper_MinMax_ ()
    {
        {
            VariantValue v = numeric_limits<T>::lowest ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::min ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::max ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
    }
    void    Test3_VariantValue ()
    {
        using Characters::String;
        {
            VariantValue v;
            VerifyTestResult (v.empty ());
            v = String (L"hi");
            VerifyTestResult (v == L"hi");
        }
        Test3_VariantValue_Helper_MinMax_<int> ();
        Test3_VariantValue_Helper_MinMax_<unsigned int> ();
        Test3_VariantValue_Helper_MinMax_<long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long> ();
        Test3_VariantValue_Helper_MinMax_<long long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long long> ();
        Test3_VariantValue_Helper_MinMax_<float> ();
        Test3_VariantValue_Helper_MinMax_<double> ();
        Test3_VariantValue_Helper_MinMax_<long double> ();
    }

}



namespace   {
    void    DoRegressionTests_ ()
    {
        INI_ONLY_::DoAll_ ();
        JSON_ONLY_::DoAll_();
        XML_ONLY_::DoAll_();
        GENERIC_SERIALIZE_DESERIALIZE_::DoAll_();

        Test3_VariantValue ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

