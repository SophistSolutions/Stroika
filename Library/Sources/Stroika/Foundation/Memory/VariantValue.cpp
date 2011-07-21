/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<sstream>

#include	"../Characters/StringUtils.h"

#include	"VariantValue.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;





/*
 ********************************************************************************
 ******************************** VariantValue **********************************
 ********************************************************************************
 */
VariantValue::VariantValue ()
	: fVal ()
{
}

VariantValue::VariantValue (bool val)
	: fVal (DEBUG_NEW TValRep<bool,eBoolean> (val))
{
}

VariantValue::VariantValue (int val)
	: fVal (DEBUG_NEW TValRep<int,eInteger> (val))
{
}

VariantValue::VariantValue (float val)
	: fVal (DEBUG_NEW TValRep<float,eFloat> (val))
{
}

VariantValue::VariantValue (const Date& val)
	: fVal (DEBUG_NEW TValRep<Date,eDate> (val))
{
}

VariantValue::VariantValue (const DateTime& val)
	: fVal (DEBUG_NEW TValRep<DateTime,eDateTime> (val))
{
}

VariantValue::VariantValue (const wchar_t* val)
	: fVal (DEBUG_NEW TValRep<wstring,eString> (val))
{
}

VariantValue::VariantValue (const wstring& val)
	: fVal (DEBUG_NEW TValRep<wstring,eString> (val))
{
}

VariantValue::VariantValue (const map<wstring,VariantValue>& val)
	: fVal (DEBUG_NEW TValRep<map<wstring,VariantValue>,eMap> (val))
{
}

VariantValue::VariantValue (const vector<VariantValue>& val)
	: fVal (DEBUG_NEW TValRep<vector<VariantValue>,eArray> (val))
{
}

bool	VariantValue::empty () const
{
	if (fVal.IsNull ()) {
		return true;
	}
	switch (fVal->GetType ()) {
		case	eBoolean:
		case	eInteger: {
			// cannot be empty
			return false;
		}
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNil (v);
			return _isnan (v->fVal) != 0;
		}
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.empty ();
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.empty ();
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.empty ();
		}
		case	eMap: {
			TValRep<map<wstring,VariantValue>,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue>,eMap>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.empty ();
		}
		case	eArray: {
			TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.empty ();
		}
		default: {
			return wstring (*this).empty ();
		}
	}
}

VariantValue::operator bool () const
{
	if (fVal.IsNull ()) {
		return false;
	}
	switch (fVal->GetType ()) {
		case	eBoolean: {
			TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eString: {
			wstring	tmp	=	wstring (*this);
			//return tmp != L"false";	// no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
			return tmp == L"true";	// no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
		}
		case	eInteger: {
			int	tmp	=	int (*this);
			return tmp != 0;
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return false;
		}
	}
}

VariantValue::operator int () const
{
	if (fVal.IsNull ()) {
		return 0;
	}
	switch (fVal->GetType ()) {
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNil (v);
			return static_cast<int> (v->fVal);
		}
		case	eInteger: {
			TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return Characters::String2Int (v->fVal);
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return 0;
		}
	}
}

VariantValue::operator float () const
{
	if (fVal.IsNull ()) {
		return 0.0f;
	}
	switch (fVal->GetType ()) {
		case	eInteger: {
			TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
			AssertNotNil (v);
			return static_cast<float> (v->fVal);
		}
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			// NB. this may return NAN if string not a well-formed number (including empty string case)
			return Characters::String2Float (v->fVal);
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return 0.0f;
		}
	}
}

VariantValue::operator Date () const
{
	if (fVal.IsNull ()) {
		return Date ();
	}
	switch (fVal->GetType ()) {
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.GetDate ();
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return Date (v->fVal, Date::eXML);
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return Date ();
		}
	}
}

VariantValue::operator DateTime () const
{
	if (fVal.IsNull ()) {
		return DateTime ();
	}
	switch (fVal->GetType ()) {
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNil (v);
			return DateTime (v->fVal);
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return DateTime (v->fVal, DateTime::eXML);
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return DateTime ();
		}
	}
}

VariantValue::operator wstring () const
{
	if (fVal.IsNull ()) {
		return wstring ();
	}
	switch (fVal->GetType ()) {
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.Format ();
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.Format ();
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eBoolean: {
			TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal? L"true": L"false";
		}
		case	eInteger: {
			TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
			AssertNotNil (v);
			return Characters::Format (L"%d", v->fVal);
		}
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNil (v);
			return Characters::Float2String (v->fVal);
		}
		case	eMap: {
			TValRep<map<wstring,VariantValue>,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue>,eMap>*> (fVal.get ());
			AssertNotNil (v);
			wstringstream tmp;
			tmp << L"map[";
			for (map<wstring,VariantValue>::const_iterator i = v->fVal.begin (); i != v->fVal.end (); ++i) {
				if (i != v->fVal.begin ()) {
					tmp << L", ";
				}
				tmp << i->first << L" -> " << static_cast<wstring> (i->second);
			}
			tmp << L"]";
			return tmp.str ();
		}
		case	eArray: {
			TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
			AssertNotNil (v);
			wstringstream tmp;
			tmp << L"[";
			for (vector<VariantValue>::const_iterator i = v->fVal.begin (); i != v->fVal.end (); ++i) {
				if (i != v->fVal.begin ()) {
					tmp << L", ";
				}
				tmp << static_cast<wstring> (*i);
			}
			tmp << L"]";
			return tmp.str ();
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return wstring ();
		}
	}
}

VariantValue::operator map<wstring,VariantValue> () const
{
	if (fVal.IsNull ()) {
		return map<wstring,VariantValue> ();
	}
	switch (fVal->GetType ()) {
		case	eMap: {
			TValRep<map<wstring,VariantValue> ,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue> ,eMap>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return map<wstring,VariantValue> ();
		}
	}
}

VariantValue::operator vector<VariantValue> () const
{
	if (fVal.IsNull ()) {
		return vector<VariantValue> ();
	}
	switch (fVal->GetType ()) {
		case	eArray: {
			TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		default: {
			Assert (false);	// bad type (or not yet supported)
			return vector<VariantValue> ();
		}
	}
}

wstring	VariantValue::FormatXML () const
{
	if (fVal.IsNull ()) {
		return wstring ();
	}
	switch (fVal->GetType ()) {
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.Format4XML ();
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal.Format4XML ();
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal;
		}
		case	eBoolean: {
			TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
			AssertNotNil (v);
			return v->fVal? L"true": L"false";
		}
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNil (v);
			return Characters::Float2String (v->fVal);
		}
		default: {
			return wstring (*this);
		}
	}
}

bool	Memory::operator== (const VariantValue& lhs, const VariantValue& rhs)
{
	VariantValue::Type	lt	=	lhs.GetType ();
	VariantValue::Type	rt	=	rhs.GetType ();
	if (lt != rt) {
		// specail case - comparing a string with a bool or bool with a string
		if (lt == VariantValue::eBoolean and rt == VariantValue::eString or
			lt == VariantValue::eString and rt == VariantValue::eBoolean
			) {
			// compare as STRING  - in case someone compares true with 'FRED' (we want that to come out as a FALSE compare result)
			return	static_cast<wstring> (lhs) == static_cast<wstring> (rhs);
		}
		return false;
	}
	switch (lt) {
		case	VariantValue::eNull:		return true;
		case	VariantValue::eBoolean:		return static_cast<bool> (lhs) == static_cast<bool> (rhs);
		case	VariantValue::eInteger:		return static_cast<int> (lhs) == static_cast<int> (rhs);
		case	VariantValue::eFloat:		return static_cast<float> (lhs) == static_cast<float> (rhs);
		case	VariantValue::eDate:		return static_cast<Date> (lhs) == static_cast<Date> (rhs);
		case	VariantValue::eDateTime:	return static_cast<DateTime> (lhs) == static_cast<DateTime> (rhs);
		case	VariantValue::eString:		return static_cast<wstring> (lhs) == static_cast<wstring> (rhs);
		case	VariantValue::eArray: {
			// same iff all elts same
			vector<VariantValue>	lhsV	=	lhs;
			vector<VariantValue>	rhsV	=	rhs;
			if (lhsV.size () != rhsV.size ()) {
				return false;
			}
			for (size_t i = 0; i < lhsV.size (); ++i) {
				if (lhsV[i] != rhsV[i]) {
					return false;
				}
			}
			return true;
		}
		case	VariantValue::eMap: {
			// same iff all elts same
			map<wstring,VariantValue>	lhsM	=	lhs;
			map<wstring,VariantValue>	rhsM	=	rhs;
			if (lhsM.size () != rhsM.size ()) {
				return false;
			}
			map<wstring,VariantValue>::const_iterator li = lhsM.begin ();
			map<wstring,VariantValue>::const_iterator ri = rhsM.begin ();
			for (; li != lhsM.end (); ++li, ++ri) {
				if (*li != *ri) {
					return false;
				}
			}
			Ensure (li == lhsM.end ());
			Ensure (ri == rhsM.end ());
			return true;
		}
		default:	AssertNotReached ();	return false;
	}
}
