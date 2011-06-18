/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/Cryptography/Base64.h"
#include	"Stroika/Foundation/Cryptography/MD5.h"
#include	"Stroika/Foundation/Debug/Assertions.h"


using	namespace	Stroika::Foundation;



namespace	{
	void	_ASSERT_HANDLER_(const char* fileName, int lineNum)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}




namespace	{
	void	DoRegressionTests_ ()
		{
			{
				char	Src1[] = "This is a very good test of a very good test";
				char	Val1[] = "08c8888b86d6300ade93a10095a9083a";
				Assert (Cryptography::ComputeMD5Digest ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1)) == Val1);
			}

			{
				char	Src1[] = "This is a good test\r\n"
								"We eat wiggly worms.\r\n"
								"\r\n"
								"That is a very good thing.****^^^#$#AS\r\n";
				char	Val1[] = "VGhpcyBpcyBhIGdvb2QgdGVzdA0KV2UgZWF0IHdpZ2dseSB3b3Jtcy4NCg0KVGhhdCBpcyBhIHZl\r\ncnkgZ29vZCB0aGluZy4qKioqXl5eIyQjQVMNCg==";
				vector<Byte> Val1Bytes = vector<Byte> ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1));


				//string tmp = Cryptography::EncodeBase64 (vector<Byte> ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1)));
				Assert (Cryptography::EncodeBase64 (Val1Bytes) == Val1);
				Assert (Cryptography::DecodeBase64 (Cryptography::EncodeBase64 (Val1Bytes)) == Val1Bytes);
			}

			{
				char	Src1[]	=	"{\\rtf1 \\ansi {\\fonttbl {\\f0 \\fnil \\fcharset163 Times New Roman;}}{\\colortbl \\red0\\green0\\blue0;}\r\n"
		"{\\*\\listtable{\\list \\listtemplateid12382 {\\listlevel \\levelnfc23 \\leveljc0 \\levelfollow0 \\levelstartat1 \\levelindent0 {\\leveltext \\levelnfc23 \\leveltemplateid17421 \\'01\\u8226  ?;}\\f0 \\fi-360 \\li720 \\jclisttab \\tx720 }\\listid292 }}\r\n"
		"{\\*\\listoverridetable{\\listoverride \\listid292 \\listoverridecount0 \\ls1 }}\r\n"
		"{\\*\\generator Sophist Solutions, Inc. Led RTF IO Engine - 3.1b2x;}\\pard \\plain \\f0 \\fs24 \\cf0 Had hay fever today. Not terrible, but several times. And I think a bit yesterda\r\n"
		"y.}"
		;
				char	Val1[] = "e1xydGYxIFxhbnNpIHtcZm9udHRibCB7XGYwIFxmbmlsIFxmY2hhcnNldDE2MyBUaW1lcyBOZXcg\r\n"
		"Um9tYW47fX17XGNvbG9ydGJsIFxyZWQwXGdyZWVuMFxibHVlMDt9DQp7XCpcbGlzdHRhYmxle1xs\r\n"
		"aXN0IFxsaXN0dGVtcGxhdGVpZDEyMzgyIHtcbGlzdGxldmVsIFxsZXZlbG5mYzIzIFxsZXZlbGpj\r\n"
		"MCBcbGV2ZWxmb2xsb3cwIFxsZXZlbHN0YXJ0YXQxIFxsZXZlbGluZGVudDAge1xsZXZlbHRleHQg\r\n"
		"XGxldmVsbmZjMjMgXGxldmVsdGVtcGxhdGVpZDE3NDIxIFwnMDFcdTgyMjYgID87fVxmMCBcZmkt\r\n"
		"MzYwIFxsaTcyMCBcamNsaXN0dGFiIFx0eDcyMCB9XGxpc3RpZDI5MiB9fQ0Ke1wqXGxpc3RvdmVy\r\n"
		"cmlkZXRhYmxle1xsaXN0b3ZlcnJpZGUgXGxpc3RpZDI5MiBcbGlzdG92ZXJyaWRlY291bnQwIFxs\r\n"
		"czEgfX0NCntcKlxnZW5lcmF0b3IgU29waGlzdCBTb2x1dGlvbnMsIEluYy4gTGVkIFJURiBJTyBF\r\n"
		"bmdpbmUgLSAzLjFiMng7fVxwYXJkIFxwbGFpbiBcZjAgXGZzMjQgXGNmMCBIYWQgaGF5IGZldmVy\r\n"
		"IHRvZGF5LiBOb3QgdGVycmlibGUsIGJ1dCBzZXZlcmFsIHRpbWVzLiBBbmQgSSB0aGluayBhIGJp\r\n"
		"dCB5ZXN0ZXJkYQ0KeS59";
				vector<Byte> Val1Bytes = vector<Byte> ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1));

				string tmp = Cryptography::EncodeBase64 (vector<Byte> ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1)));
				Assert (Cryptography::EncodeBase64 (vector<Byte> ((const Byte*)Src1, (const Byte*)Src1 + ::strlen(Src1))) == Val1);

				Assert (Cryptography::DecodeBase64 (Cryptography::EncodeBase64 (Val1Bytes)) == Val1Bytes);

				vector<Byte>	x = Cryptography::DecodeBase64 (Val1);

				Assert (Cryptography::EncodeBase64 (x) == Val1);
			}

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

