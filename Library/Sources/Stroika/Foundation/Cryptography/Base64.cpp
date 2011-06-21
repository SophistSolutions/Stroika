/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
/*
 * Portions of code from http://freecode-freecode.blogspot.com/2008/02/base64c.html (not copyrighted).
 */
/*
 * Portions of code from http://www.adp-gmbh.ch/cpp/common/base64.html
 *
 *  Copyright (C) 2004-2008 René Nyffenegger
 * 
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

#include	"../StroikaPreComp.h"

#include	"../Containers/Common.h"
#include	"../Debug/Assertions.h"
#include	"../Execution/Exceptions.h"
#include	"../Memory/SmallStackBuffer.h"
#include	"../Streams/BadFormatException.h"

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
namespace	{
	const char	BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

#if 1
//PUBLIC DOMAIN IMPL OF DECODE libb64
namespace	{
	enum base64_decodestep {
		step_a, step_b, step_c, step_d
	};
	struct base64_decodestate {
		base64_decodestep step;
		char plainchar;
	};
	void base64_init_decodestate(base64_decodestate* state_in)
		{
			state_in->step = step_a;
			state_in->plainchar = 0;
		}
	int base64_decode_value(char value_in)
		{
			static const char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
			static const char decoding_size = sizeof(decoding);
			value_in -= 43;
			if (value_in < 0 || value_in > decoding_size) return -1;
			return decoding[(int)value_in];
		}
	int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
		{
			const char* codechar = code_in;
			char* plainchar = plaintext_out;
			char fragment;
	
			*plainchar = state_in->plainchar;
	
			switch (state_in->step)
			{
				while (1)
				{
			case step_a:
					do {
						if (codechar == code_in+length_in)
						{
							state_in->step = step_a;
							state_in->plainchar = *plainchar;
							return plainchar - plaintext_out;
						}
						fragment = (char)base64_decode_value(*codechar++);
					} while (fragment < 0);
					*plainchar    = (fragment & 0x03f) << 2;
			case step_b:
					do {
						if (codechar == code_in+length_in)
						{
							state_in->step = step_b;
							state_in->plainchar = *plainchar;
							return plainchar - plaintext_out;
						}
						fragment = (char)base64_decode_value(*codechar++);
					} while (fragment < 0);
					*plainchar++ |= (fragment & 0x030) >> 4;
					*plainchar    = (fragment & 0x00f) << 4;
			case step_c:
					do {
						if (codechar == code_in+length_in)
						{
							state_in->step = step_c;
							state_in->plainchar = *plainchar;
							return plainchar - plaintext_out;
						}
						fragment = (char)base64_decode_value(*codechar++);
					} while (fragment < 0);
					*plainchar++ |= (fragment & 0x03c) >> 2;
					*plainchar    = (fragment & 0x003) << 6;
			case step_d:
					do {
						if (codechar == code_in+length_in)
						{
							state_in->step = step_d;
							state_in->plainchar = *plainchar;
							return plainchar - plaintext_out;
						}
						fragment = (char)base64_decode_value(*codechar++);
					} while (fragment < 0);
					*plainchar++   |= (fragment & 0x03f);
				}
			}
			/* control should not reach here */
			return plainchar - plaintext_out;
		}
}
#endif


vector<Byte>	Cryptography::DecodeBase64 (const string& s)
{
	size_t dataSize1 = s.length ();
	SmallStackBuffer<Byte>	buf1 (dataSize1);	// MUCH more than big enuf
	base64_decodestate _state;
	base64_init_decodestate(&_state);
	int r = base64_decode_block(s.c_str (), s.length (), (char*)buf1.begin (), &_state);
	Assert (r <= dataSize1);
	return vector<Byte> (buf1.begin (), buf1.begin () + r);
}








/*
 ********************************************************************************
 *********************** Cryptography::EncodeBase64 *****************************
 ********************************************************************************
 */
namespace	{
	std::string base64_encode_ (unsigned char const* bytes_to_encode, size_t in_len, LineBreak lb)
	{
		string ret;
		ret.reserve ((in_len / 4) + 3);
		int i = 0;
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

				for (i = 0; (i <4) ; i++) {
					ret += BASE64_CHARS[char_array_4[i]];
				}

				rowLen += 4;
				i = 0;
				if ((rowLen %76) == 0) {
					switch (lb) {
						case	eLF_LB: ret += "\n";; break;
						case	eCRLF_LB: ret += "\r\n"; break;
					}
					rowLen=0;
				}
			}
		}

		if (i != 0) {
			for (int j = i; j < 3; j++) {
				char_array_3[j] = '\0';
			}

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (int j = 0; (j < i + 1); j++) {
				ret += BASE64_CHARS[char_array_4[j]];
			}

			while ((i++ < 3)) {
				ret += '=';
			}
		}

		return ret;
	}
}




namespace	{
	// FROM PUBLC DOMAIN LIB64 ....
	typedef enum
	{
		step_A, step_B, step_C
	} base64_encodestep;

	typedef struct
	{
		base64_encodestep step;
		char result;
		int stepcount;


LineBreak lb;
	} base64_encodestate;

//	const int CHARS_PER_LINE = 72;
	const int CHARS_PER_LINE = 76;

	void base64_init_encodestate(base64_encodestate* state_in)
	{
		state_in->step = step_A;
		state_in->result = 0;
		state_in->stepcount = 0;
	}

	char base64_encode_value(char value_in)
	{
		static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		if (value_in > 63) return '=';
		return encoding[(int)value_in];
	}

	int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
	{
		const char* plainchar = plaintext_in;
		const char* const plaintextend = plaintext_in + length_in;
		char* codechar = code_out;
		char result;
		char fragment;
	
		result = state_in->result;
	
		switch (state_in->step)
		{
			while (1)
			{
		case step_A:
				if (plainchar == plaintextend)
				{
					state_in->result = result;
					state_in->step = step_A;
					return codechar - code_out;
				}
				fragment = *plainchar++;
				result = (fragment & 0x0fc) >> 2;
				*codechar++ = base64_encode_value(result);
				result = (fragment & 0x003) << 4;
		case step_B:
				if (plainchar == plaintextend)
				{
					state_in->result = result;
					state_in->step = step_B;
					return codechar - code_out;
				}
				fragment = *plainchar++;
				result |= (fragment & 0x0f0) >> 4;
				*codechar++ = base64_encode_value(result);
				result = (fragment & 0x00f) << 2;
		case step_C:
				if (plainchar == plaintextend)
				{
					state_in->result = result;
					state_in->step = step_C;
					return codechar - code_out;
				}
				fragment = *plainchar++;
				result |= (fragment & 0x0c0) >> 6;
				*codechar++ = base64_encode_value(result);
				result  = (fragment & 0x03f) >> 0;
				*codechar++ = base64_encode_value(result);
			
				++(state_in->stepcount);
				if (state_in->stepcount == CHARS_PER_LINE/4)
				{
//					*codechar++ = '\n';
					switch (state_in->lb) {
						case	eLF_LB: *codechar++ = '\n'; break;
						case	eCRLF_LB: *codechar++ = '\r'; *codechar++ = '\n'; break;
					}
					state_in->stepcount = 0;
				}
			}
		}
		/* control should not reach here */
		return codechar - code_out;
	}

	int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
	{
		char* codechar = code_out;
	
		switch (state_in->step)
		{
		case step_B:
			*codechar++ = base64_encode_value(state_in->result);
			*codechar++ = '=';
			*codechar++ = '=';
			break;
		case step_C:
			*codechar++ = base64_encode_value(state_in->result);
			*codechar++ = '=';
			break;
		case step_A:
			break;
		}
#if 0
//		*codechar++ = '\n';
					switch (state_in->lb) {
						case	eLF_LB: *codechar++ = '\n'; break;
						case	eCRLF_LB: *codechar++ = '\r'; *codechar++ = '\n'; break;
					}
#endif
	
		return codechar - code_out;
	}

}

string	Cryptography::EncodeBase64 (const Byte* start, const Byte* end, LineBreak lb)
{
#if 1
	base64_encodestate _state;
	base64_init_encodestate(&_state);
	_state.lb = lb;
	int srcLen = end - start;
	int bufSize = 4 * srcLen;
	SmallStackBuffer<char>	data (bufSize);
	int r = 	 base64_encode_block((char*)start, (end-start), data.begin (), &_state);
	int extraBytes = base64_encode_blockend(data.begin () + r, &_state);
	int totalBytes = r + extraBytes;
	return string (data.begin (), data.begin () + totalBytes);
#else
	return base64_encode_ (start, end-start, lb);
#endif
}

string	Cryptography::EncodeBase64 (const vector<Byte>& b, LineBreak lb)
{
#if 1
	return EncodeBase64 (Containers::Start (b), Containers::End (b), lb);
#else
	return base64_encode_ (Containers::Start (b), b.size (), lb);
#endif
}

