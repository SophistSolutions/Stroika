/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<sstream>

#include	"../../Characters/StringUtils.h"
#include	"../BadFormatException.h"

#include	"Reader.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;


/*
 * TODO:
 *		(1)	Must add support for quoted characters like '"' etc - which cause trouble inside json. Also - quoted utf-8 characters.
 */





namespace	{
	
	/*
	 * Language Docs:
	 *		http://json.org/
	 */

	/*
	 * Parse strategy:
	 *		o	Pre-pass to map all input to UNICODE - and then just handle a sequence of unicode strings
	 *		o	Classic resursive decent parser.
	 *		o	Inline lexical analysis (cuz very simple)
	 */

	enum	ReadState_ {
		eNeutral_,
		eInObject_,
		eInArray_,
		eInNumber_,
		eInString_,
	};


	wstring	ReadTextStream_ (istream& in)
		{
			streamoff	start	=	in.tellg ();
			in.seekg (0, ios_base::end);
			streamoff	end		=	in.tellg ();
			Assert (start <= end);
			if (end - start > numeric_limits<ptrdiff_t>::max ()) {
				Execution::DoThrow (Execution::StringException (L"stream too large"));
			}
			size_t	bufLen	=	static_cast<size_t> (end - start);
			Memory::SmallStackBuffer<Byte>	buf (bufLen);
			in.seekg (start, ios_base::beg);
			in.read (reinterpret_cast<char*> (buf.begin ()), bufLen);
			size_t readLen = static_cast<size_t> (in.gcount ());
			Assert (readLen <= bufLen);
			const char*	startOfBuf	=	reinterpret_cast<const char*> (static_cast<const Byte*> (buf));
			return Characters::MapUNICODETextWithMaybeBOMTowstring (startOfBuf, startOfBuf + readLen);
		}
	wstring	ReadTextStream_ (wistream& in)
		{
			streamoff	start	=	in.tellg ();
			in.seekg (0, ios_base::end);
			streamoff	end		=	in.tellg ();
			Assert (start <= end);
			if (end - start > numeric_limits<ptrdiff_t>::max ()) {
				Execution::DoThrow (Execution::StringException (L"stream too large"));
			}
			size_t	bufLen	=	static_cast<size_t> (end - start);
			Memory::SmallStackBuffer<wchar_t>	buf (bufLen);
			in.seekg (start, ios_base::beg);
			in.read (reinterpret_cast<wchar_t*> (buf.begin ()), bufLen);
			size_t readLen = static_cast<size_t> (in.gcount ());
			Assert (readLen <= bufLen);
			const wchar_t*	startOfBuf	=	reinterpret_cast<const wchar_t*> (static_cast<const wchar_t*> (buf));
			return wstring (startOfBuf, startOfBuf + readLen);
		}


	wstring	Prepass2UNICODE_ (const wstring& in)
		{
// even given unicode, we could have \u characters - mapem!!!
// NYI
			return in;
		}
	wstring	Prepass2UNICODE_ (wistream& in)
		{
			return Prepass2UNICODE_ (ReadTextStream_ (in));
		}
	wstring	Prepass2UNICODE_ (istream& in)
		{
			return Prepass2UNICODE_ (ReadTextStream_ (in));
		}



	/*
	 * Utilities to treat string iterator/end ptr as a 'stream pointer' - and get next char
	 */
	bool	IsAtEOF_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i <= end);
			return *i == end;
		}
	wchar_t	NextChar_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (not IsAtEOF_ (i, end));
			wchar_t	c	=	*(*i);
			(*i)++;
			return c;
		}

	Memory::VariantValue	Reader_value_ (wstring::const_iterator* i, wstring::const_iterator end);


	// 'in' is positioned to the start of string, and we read, leaving in possitioned just after end of string
	Memory::VariantValue	Reader_String_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i < end);
			wchar_t	c	=	NextChar_ (i, end);
			if (c != '\"') {
				Execution::DoThrow (BadFormatException (L"JSON: Expected quoted string"));
			}
			// accumulate chars, and check for close-quote
			wstring	result;
			while (true) {
				if (IsAtEOF_ (i, end)) {
					Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for close quote)"));
				}
				c = NextChar_ (i, end);
				if (c == '\"') {
					return Memory::VariantValue (result);
				}
				Containers::ReserveSpeedTweekAdd1 (result);
				result += c;	// must handle other character quoting (besides \u which was preflighted)
			}
		}



	// 'in' is positioned to the start of number, and we read, leaving in possitioned just after end of number
	Memory::VariantValue	Reader_Number_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i < end);

			bool	containsDot	=	false;
			// ACCUMULATE STRING, and then call builtin number parsing functions...
			// This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
			wstring	tmp;
			while (not IsAtEOF_ (i, end)) {
				wchar_t	c = NextChar_ (i, end);
				if (iswdigit (c) or c == '.' or c == 'e' or c == 'E' or c == '+' or c == '-') {
					Containers::ReserveSpeedTweekAdd1 (tmp);
					tmp += c;
					if (c == '.') {
						containsDot = true;
					}
				}
				else {
					// any other character signals end of number (not a syntax error)
					// but if we read anything at all, backup - don't consume next character - not part of number
					if (!tmp.empty ()) {
						(*i)--;
					}
					break;
				}
			}
			// if got not valid characters, THATS not a valid # - so error
			if (tmp.empty ()) {
				Execution::DoThrow (BadFormatException (L"JSON: no valid number found"));
			}
			// COULD do better about types (int32 versus int64 etc)
			if (containsDot) {
				return Memory::VariantValue (Characters::String2Float (tmp));
			}
			else {
				return Memory::VariantValue (Characters::String2Int (tmp));
			}
		}

	Memory::VariantValue	Reader_Object_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i < end);
			map<wstring,Memory::VariantValue>	result;

			if (NextChar_ (i, end) != '{') {
				Execution::DoThrow (BadFormatException (L"JSON: Expected '{'"));
			}
			// accumulate elements, and check for close-array
			enum LookingFor { eName, eValue, eColon, eComma };
			LookingFor	lf = eName;

			wstring	curName;
			while (true) {
				if (IsAtEOF_ (i, end)) {
					Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for '}')"));
				}
				if (**i == '}') {
					if (lf == eName or lf == eComma) {
						NextChar_ (i, end);		// skip char
						return Memory::VariantValue (result);
					}
					else {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected '}' reading object"));
					}
				}
				else if (iswspace (**i)) {
					NextChar_ (i, end);		// skip char
				}
				else if (**i == ',') {
					if (lf == eComma) {
						NextChar_ (i, end);		// skip char
						lf = eName;				// next elt
					}
					else {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected ',' reading object"));
					}
				}
				else if (**i == ':') {
					if (lf == eColon) {
						NextChar_ (i, end);		// skip char
						lf = eValue;			// next elt
					}
					else {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected ':' reading object"));
					}
				}
				else {
					if (lf == eName) {
						curName = Reader_String_ (i, end).As<wstring> ();
						lf = eColon;
					}
					else if (lf == eValue) {
						Memory::VariantValue	v	 = Reader_value_ (i, end);
						result.insert (map<wstring,Memory::VariantValue>::value_type (curName, v));
						curName.clear ();
						lf = eComma;
					}
					else {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected character looking for colon or comma reading object"));
					}
				}
			}
		}

	Memory::VariantValue	Reader_Array_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i < end);
			vector<Memory::VariantValue>	result;

			if (NextChar_ (i, end) != '[') {
				Execution::DoThrow (BadFormatException (L"JSON: Expected '['"));
			}
			// accumulate elements, and check for close-array
			bool	lookingForElt	=	true;
			while (true) {
				if (IsAtEOF_ (i, end)) {
					Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for ']')"));
				}
				if (**i == ']') {
					if (lookingForElt) {
						// allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
					}
					NextChar_ (i, end);		// skip char
					return Memory::VariantValue (result);
				}
				else if (**i == ',') {
					if (lookingForElt) {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected second ',' in reading array"));
					}
					else {
						lookingForElt = true;
					}
					NextChar_ (i, end);		// skip char
				}
				else if (iswspace (**i)) {
					NextChar_ (i, end);		// skip char
				}
				else {
					// not looking at whitespace, in midst of array, and array not terminated, so better be looking at a value
					if (lookingForElt) {
						Containers::ReserveSpeedTweekAdd1 (result);
						result.push_back (Reader_value_ (i, end));
						lookingForElt = false;
					}
					else {
						Execution::DoThrow (BadFormatException (L"JSON: Unexpected character (missing ',' ?) in reading array"));
					}
				}
			}
		}

	Memory::VariantValue	Reader_SpecialToken_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			Require (i != NULL);
			Require (*i < end);
			switch (**i) {
				case	'f': {
					if (*i + 5 < end and
						*((*i)+1) == 'a' and
						*((*i)+2) == 'l' and
						*((*i)+3) == 's' and
						*((*i)+4) == 'e'
						)
					{
						(*i) += 5;
						return Memory::VariantValue (false);
					}
				}
				break;
				case	't': {
					if (*i + 4 < end and
						*((*i)+1) == 'r' and
						*((*i)+2) == 'u' and
						*((*i)+3) == 'e'
						)
					{
						(*i) += 4;
						return Memory::VariantValue (true);
					}
				}
				break;
				case	'n': {
					if (*i + 4 < end and
						*((*i)+1) == 'u' and
						*((*i)+2) == 'l' and
						*((*i)+3) == 'l'
						)
					{
						(*i) += 4;
						return Memory::VariantValue ();
					}
				}
				break;
			}
			Execution::DoThrow (BadFormatException (L"JSON: Unrecognized token"));
		}

	Memory::VariantValue	Reader_value_ (wstring::const_iterator* i, wstring::const_iterator end)
		{
			// Skip initial whitespace, and look for any value:
			//		string
			//		number
			//		object
			//		array
			//		true
			//		false
			//		null
			for (; *i < end; ++i) {
				switch (**i) {
					case	'\"':		return Reader_String_ (i, end);
					
					case	'0':
					case	'1':
					case	'2':
					case	'3':
					case	'4':
					case	'5':
					case	'6':
					case	'7':
					case	'8':
					case	'9':
					case	'-':		return Reader_Number_ (i, end);

					case	'{':		return Reader_Object_ (i, end);
					case	'[':		return Reader_Array_ (i, end);
					
					case	't':
					case	'f':
					case	'n':		return Reader_SpecialToken_ (i, end);

					default: {
						if (iswspace (**i)) {
							// ignore
						}
						else {
							Execution::DoThrow (BadFormatException (L"JSON: Unexpected character looking for start of value"));
						}
					}
				}

			}
			// if we get here - nothing found
			Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF looking for value"));
		}
}







/*
 ********************************************************************************
 ******************************* Streams::JSON::Reader **************************
 ********************************************************************************
 */
Memory::VariantValue	Streams::JSON::Reader (istream& in)
{
	wstring	tmp	=	Prepass2UNICODE_ (in);
	wstring::const_iterator i = tmp.begin ();
	return Reader_value_ (&i, tmp.end ());
}

Memory::VariantValue	Streams::JSON::Reader (wistream& in)
{
	wstring	tmp	=	Prepass2UNICODE_ (in);
	wstring::const_iterator i = tmp.begin ();
	return Reader_value_ (&i, tmp.end ());
}
