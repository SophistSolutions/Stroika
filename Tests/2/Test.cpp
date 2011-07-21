/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Containers/VectorUtils.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Memory/VariantValue.h"
#include	"Stroika/Foundation/Streams/JSON/Reader.h"
#include	"Stroika/Foundation/Streams/JSON/Writer.h"


using	namespace	Stroika::Foundation;

using	Memory::VariantValue;



namespace	{
	void	_ASSERT_HANDLER_(const char* fileName, int lineNum)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}





namespace	{
	void	CheckMatchesExpected_WRITER_ (const VariantValue& v, const string& expected)
		{
			stringstream	out;
			Streams::JSON::PrettyPrint (v, out);
			string x = out.str ();
			for (string::size_type i = 0; i < min (x.length (), expected.length ()); ++i) {
				if (x[i] != expected[i]) {
					AssertNotReached ();
				}
			}
			Assert (out.str () == expected);
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
				VariantValue	v1 =	4.7f;
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
			VariantValue	v1	=	Streams::JSON::Reader (tmp);
			Assert (v1.GetType () == expected.GetType ());
			Assert (v1 == expected);
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
				VariantValue	v1 =	4.7f;
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

		}
}


namespace	{

	void	DoRegressionTests_ ()
		{
			DoRegressionTests_Writer_ ();
			DoRegressionTests_Reader_ ();
		}
}




int main(int argc, const char* argv[])
{
#if		defined (_DEBUG)
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
	DoRegressionTests_ ();

	cout << "Succeeded" << endl;
	return EXIT_SUCCESS;
}
