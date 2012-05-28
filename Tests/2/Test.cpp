/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Containers/VectorUtils.h"
#include	"Stroika/Foundation/DataExchangeFormat/JSON/Reader.h"
#include	"Stroika/Foundation/DataExchangeFormat/JSON/Writer.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Memory/VariantValue.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;

using	Memory::VariantValue;




/*
 * Validating JSON parse results:
 *		http://json.parser.online.fr/
 */







namespace	{
	void	CheckMatchesExpected_WRITER_ (const VariantValue& v, const string& expected)
		{
			stringstream	out;
			DataExchangeFormat::JSON::PrettyPrint (v, out);
			string x = out.str ();
			for (string::size_type i = 0; i < min (x.length (), expected.length ()); ++i) {
				if (x[i] != expected[i]) {
					VerifyTestResult (false);
				}
			}
			VerifyTestResult (out.str () == expected);
		}

	void	DoRegressionTests_Writer_ ()
		{
			{
				VariantValue	v1 = L"hello world";
				CheckMatchesExpected_WRITER_ (v1, "\"hello world\"");
			}
			{
				VariantValue	v1 =	3;
				CheckMatchesExpected_WRITER_ (v1, "3");
			}
			{
				VariantValue	v1 =	4.7;
				CheckMatchesExpected_WRITER_ (v1, "4.7");
			}
			{
				// array
				vector<VariantValue>	v;
				v.push_back (3);
				v.push_back (7);
				v.push_back (L"cookie");
				VariantValue	v1 =	v;
				CheckMatchesExpected_WRITER_ (v1, "[\n    3,\n    7,\n    \"cookie\"\n]");
			}
			{
				// object
				map<wstring,VariantValue>	v;
				v[L"Arg1"] = 32;
				v[L"Arg2"] = L"Cookies";
				v[L"Arg3"] = Containers::mkV<VariantValue> (19);
				VariantValue	v1 =	v;
				CheckMatchesExpected_WRITER_ (v1, "{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}");
			}
		}
}




namespace	{
	void	CheckMatchesExpected_READER_ (const string& v, const VariantValue& expected)
		{
			stringstream	tmp;
			tmp << v;
			VariantValue	v1	=	DataExchangeFormat::JSON::Reader (tmp);
			VerifyTestResult (v1.GetType () == expected.GetType ());
			VerifyTestResult (v1 == expected);
		}

	void	DoRegressionTests_Reader_ ()
		{
			{
				VariantValue	v1 = L"hello world";
				CheckMatchesExpected_READER_ ("\"hello world\"", v1);
			}
			{
				VariantValue	v1 =	3;
				CheckMatchesExpected_READER_ ("3", v1);
			}
			{
				VariantValue	v1 =	4.7;
				CheckMatchesExpected_READER_ ("4.7", v1);
			}
			{
				// array
				vector<VariantValue>	v;
				v.push_back (3);
				v.push_back (7);
				v.push_back (L"cookie");
				VariantValue	v1 =	v;
				CheckMatchesExpected_READER_ ("[\n    3,\n    7,\n    \"cookie\"\n]", v1);
			}
			{
				// object
				map<wstring,VariantValue>	v;
				v[L"Arg1"] = 32;
				v[L"Arg2"] = L"Cookies";
				v[L"Arg3"] = Containers::mkV<VariantValue> (19);
				VariantValue	v1 =	v;
				CheckMatchesExpected_READER_ ("{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}", v1);
			}
			{
				// Bug found in another JSON reader (sent me by Ryan - 2011-07-27)
				const	string	kExample	=	"{\"nav_items\":[{\"main_link\":{\"href\":\"/about/index.html\",\"text\":\"Who We Are\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/company-management.html\",\"text\":\"Management\"}},{\"header\":{\"href\":\"/about/mission-statement.html\",\"text\":\"Mission\"}},{\"header\":{\"href\":\"/about/company-history.html\",\"text\":\" History\"}},{\"header\":{\"href\":\"/about/headquarters.html\",\"text\":\"Corporate Headquarters\"}},{\"header\":{\"href\":\"/about/diversity.html\",\"text\":\"Diversity\"}},{\"header\":{\"href\":\"/about/supplier-diversity.html\",\"text\":\"Supplier Diversity\"}}]}]},{\"main_link\":{\"href\":\"http://investor.compuware.com\",\"text\":\"Investor Relations\"}},{\"main_link\":{\"href\":\"/about/newsroom.html\",\"text\":\"News Room\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/analyst-reports\",\"text\":\"Analyst Reports\"}},{\"header\":{\"href\":\"/about/awards-recognition.html\",\"text\":\"Awards and Recognition\"}},{\"header\":{\"href\":\"/about/blogs.html\",\"text\":\"Blog Home\"}},{\"header\":{\"href\":\"/about/press-analyst-contacts.html\",\"text\":\"Contact Us\"}},{\"header\":{\"href\":\"/about/customers.html\",\"text\":\"Customers\"}},{\"header\":{\"href\":\"/about/press-mentions\",\"text\":\"Press Mentions\"}},{\"header\":{\"href\":\"/about/press-releases\",\"text\":\"Press Releases\"}},{\"header\":{\"href\":\"/about/press-resources.html\",\"text\":\"Press Resources\"}}]}]},{\"main_link\":{\"href\":\"#top\",\"text\":\"Sponsorships\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"/about/lemans-sponsorship.html\",\"text\":\"Le Mans\"}},{\"header\":{\"href\":\"/about/nhl-sponsorship.html\",\"text\":\"NHL\"}},{}]}]},{\"main_link\":{\"href\":\"/about/community-involvement.html\",\"text\":\"Community Involvement\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"http://communityclicks.compuware.com\",\"text\":\"Community Clicks Blog\"}},{\"header\":{\"href\":\"javascript:securenav('/forms/grant-eligibility-form.html')\",\"text\":\"Grant Eligibility Form\"}},{}]}]},{\"main_link\":{\"href\":\"/government/\",\"text\":\"Government\"}}]}";
				stringstream	tmp;
				tmp << kExample;
				VariantValue	v1	=	DataExchangeFormat::JSON::Reader (tmp);
				VerifyTestResult (v1.GetType () == VariantValue::eMap);
			}

		}
}


namespace	{

	void	DoRegressionTests_ ()
		{
			DoRegressionTests_Writer_ ();
			DoRegressionTests_Reader_ ();
		}
}




#if qOnlyOneMain
extern  int TestJSON ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

