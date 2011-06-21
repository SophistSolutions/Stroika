/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
/*
 * Note - used PUBLIC DOMAIN http://sourceforge.net/projects/libb64/files/libb64/libb64/libb64-1.2.src.zip/download
 * code as a starting point.
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
 *	IMPLEMENTATION NOTES:
 *
 *		The public domain (private) code in this file - is designed to operate in a STREAM mode. I've preserved that
 *	internally, evne though my current API doesnt work that way, because we will soon support a STREAM based API here,
 *	and that will fit perfectly.
 *		-- LGP 2011-06-21
 */






/*
 ********************************************************************************
 *************************** Encoding::DecodeBase64 *****************************
 ********************************************************************************
 */
namespace	{
	enum base64_decodestep {
		step_a, step_b, step_c, step_d
	};
	struct base64_decodestate {
		base64_decodestep	step;
		char				plainchar;
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
	int base64_decode_block (const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
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
	enum base64_encodestep {
		step_A, step_B, step_C
	};
	struct base64_encodestate {
		base64_encodestep step;
		char result;
		int stepcount;
		LineBreak lb;
	};

	void base64_init_encodestate(base64_encodestate* state_in)
		{
			state_in->step = step_A;
			state_in->result = 0;
			state_in->stepcount = 0;
		}

	char base64_encode_value(char value_in)
		{
			const char	BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			if (value_in > 63) return '=';
			return BASE64_CHARS[(int)value_in];
		}

	int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
		{
			const int CHARS_PER_LINE = 76;
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
						switch (state_in->lb) {
							case	eLF_LB: *codechar++ = '\n'; break;
							case	eCRLF_LB: *codechar++ = '\r'; *codechar++ = '\n'; break;
						}
						state_in->stepcount = 0;
					}
				}
			}
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
	
			return codechar - code_out;
		}
}

string	Cryptography::EncodeBase64 (const Byte* start, const Byte* end, LineBreak lb)
{
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
}

string	Cryptography::EncodeBase64 (const vector<Byte>& b, LineBreak lb)
{
	return EncodeBase64 (Containers::Start (b), Containers::End (b), lb);
}

