/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<windows.h>
#include	<atlenc.h>

#include	"../Containers/Basics.h"
#include	"../Debug/Assertions.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"Base64.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Cryptography;
using	namespace	Stroika::Foundation::Memory;





/*
 ********************************************************************************
 *************************** Encoding::DecodeBase64 *****************************
 ********************************************************************************
 */

// http://freecode-freecode.blogspot.com/2008/02/base64c.html
#ifndef	qSupportFREEDCODEENECODEDECODE
#define	qSupportFREEDCODEENECODEDECODE	1
#endif
#if qSupportFREEDCODEENECODEDECODE
namespace	{
/**
 * characters used for Base64 encoding
 */  
const char *BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * encode three bytes using base64 (RFC 3548)
 *
 * @param triple three bytes that should be encoded
 * @param result buffer of four characters where the result is stored
 */  
void _base64_encode_triple(unsigned char triple[3], char result[4])
 {
    int tripleValue, i;

    tripleValue = triple[0];
    tripleValue *= 256;
    tripleValue += triple[1];
    tripleValue *= 256;
    tripleValue += triple[2];

    for (i=0; i<4; i++)
    {
 result[3-i] = BASE64_CHARS[tripleValue%64];
 tripleValue /= 64;
    }
} 

/**
 * encode an array of bytes using Base64 (RFC 3548)
 *
 * @param source the source buffer
 * @param sourcelen the length of the source buffer
 * @param target the target buffer
 * @param targetlen the length of the target buffer
 * @return 1 on success, 0 otherwise
 */  
int base64_encode_(unsigned char *source, size_t sourcelen, char *target, size_t targetlen)
 {
    /* check if the result will fit in the target buffer */
    if ((sourcelen+2)/3*4 > targetlen-1)
 return 0;

    /* encode all full triples */
    while (sourcelen >= 3)
    {
 _base64_encode_triple(source, target);
 sourcelen -= 3;
 source += 3;
 target += 4;
    }

    /* encode the last one or two characters */
    if (sourcelen > 0)
    {
 unsigned char temp[3];
 memset(temp, 0, sizeof(temp));
 memcpy(temp, source, sourcelen);
 _base64_encode_triple(temp, target);
 target[3] = '=';
 if (sourcelen == 1)
     target[2] = '=';

 target += 4;
    }

    /* terminate the string */
    target[0] = 0;

    return 1;
} 

/**
 * determine the value of a base64 encoding character
 *
 * @param base64char the character of which the value is searched
 * @return the value in case of success (0-63), -1 on failure
 */  
int _base64_char_value(char base64char)
 {
    if (base64char >= 'A' && base64char <= 'Z')
 return base64char-'A';
    if (base64char >= 'a' && base64char <= 'z')
 return base64char-'a'+26;
    if (base64char >= '0' && base64char <= '9')
 return base64char-'0'+2*26;
    if (base64char == '+')
 return 2*26+10;
    if (base64char == '/')
 return 2*26+11;
    return -1;
} 

/**
 * decode a 4 char base64 encoded byte triple
 *
 * @param quadruple the 4 characters that should be decoded
 * @param result the decoded data
 * @return lenth of the result (1, 2 or 3), 0 on failure
 */  
int _base64_decode_triple(char quadruple[4], unsigned char *result)
 {
    int i, triple_value, bytes_to_decode = 3, only_equals_yet = 1;
    int char_value[4];

    for (i=0; i<4; i++)
 char_value[i] = _base64_char_value(quadruple[i]);

    /* check if the characters are valid */
    for (i=3; i>=0; i--)
    {
 if (char_value[i]<0)
 {
     if (only_equals_yet && quadruple[i]=='=')
     {
  /* we will ignore this character anyway, make it something
   * that does not break our calculations */
  char_value[i]=0;
  bytes_to_decode--;
  continue;
     }
     return 0;
 }
 /* after we got a real character, no other '=' are allowed anymore */
 only_equals_yet = 0;
    }

    /* if we got "====" as input, bytes_to_decode is -1 */
    if (bytes_to_decode < 0)
 bytes_to_decode = 0;

    /* make one big value out of the partial values */
    triple_value = char_value[0];
    triple_value *= 64;
    triple_value += char_value[1];
    triple_value *= 64;
    triple_value += char_value[2];
    triple_value *= 64;
    triple_value += char_value[3];

    /* break the big value into bytes */
    for (i=bytes_to_decode; i<3; i++)
 triple_value /= 256;
    for (i=bytes_to_decode-1; i>=0; i--)
    {
 result[i] = triple_value%256;
 triple_value /= 256;
    }

    return bytes_to_decode;
} 

/**
 * decode base64 encoded data
 *
 * @param source the encoded data (zero terminated)
 * @param target pointer to the target buffer
 * @param targetlen length of the target buffer
 * @return length of converted data on success, -1 otherwise
 */  
size_t base64_decode_X(char *source, unsigned char *target, size_t targetlen)
 {
    char *src, *tmpptr;
    char quadruple[4], tmpresult[3];
    int i, tmplen = 3;
    size_t converted = 0;

    /* concatinate '===' to the source to handle unpadded base64 data */
    src = (char *)malloc(strlen(source)+5);
    if (src == NULL)
 return -1;
    strcpy(src, source);
    strcat(src, "====");
    tmpptr = src;

    /* convert as long as we get a full result */
    while (tmplen == 3)
    {
 /* get 4 characters to convert */
 for (i=0; i<4; i++)
 {
     /* skip invalid characters - we won't reach the end */
     while (*tmpptr != '=' && _base64_char_value(*tmpptr)<0)
  tmpptr++;

     quadruple[i] = *(tmpptr++);
 }

 /* convert the characters */
 tmplen = _base64_decode_triple(quadruple, (unsigned char*)tmpresult);

 /* check if the fit in the result buffer */
 if (targetlen < tmplen)
 {
     free(src);
     return -1;
 }

 /* put the partial result in the result buffer */
 memcpy(target, tmpresult, tmplen);
 target += tmplen;
 targetlen -= tmplen;
 converted += tmplen;
    }

    free(src);
    return converted;
} 




	vector<Byte> base64_decode_FRECODE_(const string& s)
		{
			size_t dataSize1= s.length ();
			SmallStackBuffer<Byte>	buf1 (dataSize1);
			size_t r = base64_decode_X((char*)s.c_str (), buf1.begin (), dataSize1);
			Assert (r <= dataSize1);
			return vector<Byte> (buf1.begin (), buf1.begin () + r);


		}
}
#endif

#ifndef	qSupportNyffeneggerENCODEDECODE
#define	qSupportNyffeneggerENCODEDECODE	1
#endif
#if		qSupportNyffeneggerENCODEDECODE
namespace	{
// FROM http://www.adp-gmbh.ch/cpp/common/base64.html
/* 
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

	std::string base64_encode_(unsigned char const* bytes_to_encode, unsigned int in_len) {
	  std::string ret;
	  int i = 0;
	  int j = 0;
	  unsigned char char_array_3[3];
	  unsigned char char_array_4[4];


	  int rowLen = 0;


	  while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
		  char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		  char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		  char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		  char_array_4[3] = char_array_3[2] & 0x3f;

		  for(i = 0; (i <4) ; i++)
			ret += base64_chars[char_array_4[i]];

	rowLen += 4;
		  i = 0;
	if ((rowLen %76) == 0) {
		ret += "\r\n";
		rowLen=0;
	}
		}
	  }

	  if (i)
	  {
		for(j = i; j < 3; j++)
		  char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		  ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
		  ret += '=';

	  }

	  return ret;

	}
}
#endif

namespace	{
	vector<Byte>	DecodeBase64_ATLVERSION (const string& s)
	{
		int						dataSize1	=	::Base64DecodeGetRequiredLength (s.length ());
		SmallStackBuffer<Byte>	buf1 (dataSize1);
		if (Base64Decode (s.c_str (), s.length (), buf1, &dataSize1)) {
			return vector<Byte> (buf1.begin (), buf1.begin () + dataSize1);
		}
		return vector<Byte> ();
	}
}

vector<Byte>	Cryptography::DecodeBase64 (const string& s)
{
	Assert (DecodeBase64_ATLVERSION (s) == base64_decode_FRECODE_ (s));
	return base64_decode_FRECODE_ (s);
}








/*
 ********************************************************************************
 *********************** Cryptography::EncodeBase64 *****************************
 ********************************************************************************
 */
namespace	{
	string	EncodeBase64_ATL_REFERENCE_ (const vector<Byte>& b, LineBreak lb)
		{
			size_t	totalSize		=	b.size ();
			if (totalSize != 0) {
				SmallStackBuffer<char>	relBuf (0);
				int						relEncodedSize	=	ATL::Base64EncodeGetRequiredLength (totalSize);
				relBuf.GrowToSize (relEncodedSize);
				Verify (ATL::Base64Encode (Containers::Start (b), totalSize, relBuf, &relEncodedSize));
				relBuf[relEncodedSize] = '\0';
				if (lb == eCRLF_LB) {
					return (static_cast<const char*> (relBuf));
				}
				else {
					Assert (lb == eLF_LB);
					string	result;
					result.reserve (relEncodedSize);
					for (int i = 0; i < relEncodedSize; ++i) {
						if (relBuf[i] == '\r') {
							// 
							result.push_back ('\n');
							++i;	// skip LF
						}
						else {
							result.push_back (relBuf[i]);
						}
					}
					return result;
				}
			}
			return string ();
		}
}

string	Cryptography::EncodeBase64 (const vector<Byte>& b, LineBreak lb)
{
	Assert (EncodeBase64_ATL_REFERENCE_ (b, lb) == base64_encode_ (Containers::Start (b), b.size ()));
	return base64_encode_ (Containers::Start (b), b.size ());
}

