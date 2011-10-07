/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<sstream>

#include	"../Characters/Format.h"
#include	"../Math/Common.h"

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
			AssertNotNull (v);
			return isnan (v->fVal) != 0;
		}
		case	eDate: {
			TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.empty ();
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.empty ();
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.empty ();
		}
		case	eMap: {
			TValRep<map<wstring,VariantValue>,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue>,eMap>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.empty ();
		}
		case	eArray: {
			TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.empty ();
		}
		default: {
			return As<wstring> ().empty ();
		}
	}
}

template	<>
	bool	VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return false;
			}
			switch (fVal->GetType ()) {
				case	eBoolean: {
					TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eString: {
					//return tmp != L"false";			// no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
					return As<wstring> () == L"true";	// no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
				}
				case	eInteger: {
					return As<int> () != 0;
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return false;
				}
			}
		}

template	<>
	int VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return 0;
			}
			switch (fVal->GetType ()) {
				case	eFloat: {
					TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
					AssertNotNull (v);
					return static_cast<int> (v->fVal);
				}
				case	eInteger: {
					TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eString: {
					TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
					AssertNotNull (v);
					return Characters::String2Int (v->fVal);
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return 0;
				}
			}
		}

template	<>
	float VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return 0.0f;
			}
			switch (fVal->GetType ()) {
				case	eInteger: {
					TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
					AssertNotNull (v);
					return static_cast<float> (v->fVal);
				}
				case	eFloat: {
					TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eString: {
					TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
					AssertNotNull (v);
					// NB. this may return NAN if string not a well-formed number (including empty string case)
					return Characters::String2Float (v->fVal);
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return 0.0f;
				}
			}
		}

template	<>
	Date VariantValue::As() const
		{
			if (fVal.IsNull ()) {
				return Date ();
			}
			switch (fVal->GetType ()) {
				case	eDate: {
					TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eDateTime: {
					TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal.GetDate ();
				}
				case	eString: {
					TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
					AssertNotNull (v);
					return Date::Parse (v->fVal, Date::eXML_PF);
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return Date ();
				}
			}
		}

template	<>
	DateTime VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return DateTime ();
			}
			switch (fVal->GetType ()) {
				case	eDate: {
					TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
					AssertNotNull (v);
					return DateTime (v->fVal);
				}
				case	eDateTime: {
					TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eString: {
					TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
					AssertNotNull (v);
					return DateTime::Parse (v->fVal, DateTime::eXML_PF);
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return DateTime ();
				}
			}
		}

template	<>
	wstring	VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return wstring ();
			}
			switch (fVal->GetType ()) {
				case	eDate: {
					TValRep<Date,eDate>*	v	=	dynamic_cast<TValRep<Date,eDate>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal.Format ();
				}
				case	eDateTime: {
					TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal.Format ();
				}
				case	eString: {
					TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				case	eBoolean: {
					TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal? L"true": L"false";
				}
				case	eInteger: {
					TValRep<int,eInteger>*	v	=	dynamic_cast<TValRep<int,eInteger>*> (fVal.get ());
					AssertNotNull (v);
					return Characters::Format (L"%d", v->fVal);
				}
				case	eFloat: {
					TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
					AssertNotNull (v);
					return Characters::Float2String (v->fVal);
				}
				case	eMap: {
					TValRep<map<wstring,VariantValue>,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue>,eMap>*> (fVal.get ());
					AssertNotNull (v);
					wstringstream tmp;
					tmp << L"map[";
					for (map<wstring,VariantValue>::const_iterator i = v->fVal.begin (); i != v->fVal.end (); ++i) {
						if (i != v->fVal.begin ()) {
							tmp << L", ";
						}
						tmp << i->first << L" -> " << i->second.As<wstring> ();
					}
					tmp << L"]";
					return tmp.str ();
				}
				case	eArray: {
					TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
					AssertNotNull (v);
					wstringstream tmp;
					tmp << L"[";
					for (vector<VariantValue>::const_iterator i = v->fVal.begin (); i != v->fVal.end (); ++i) {
						if (i != v->fVal.begin ()) {
							tmp << L", ";
						}
						tmp << i->As<wstring> ();
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

template	<>
	map<wstring,VariantValue>	VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return map<wstring,VariantValue> ();
			}
			switch (fVal->GetType ()) {
				case	eMap: {
					TValRep<map<wstring,VariantValue> ,eMap>*	v	=	dynamic_cast<TValRep<map<wstring,VariantValue> ,eMap>*> (fVal.get ());
					AssertNotNull (v);
					return v->fVal;
				}
				default: {
					Assert (false);	// bad type (or not yet supported)
					return map<wstring,VariantValue> ();
				}
			}
		}

template	<>
	vector<VariantValue> VariantValue::As () const
		{
			if (fVal.IsNull ()) {
				return vector<VariantValue> ();
			}
			switch (fVal->GetType ()) {
				case	eArray: {
					TValRep<vector<VariantValue>,eArray>*	v	=	dynamic_cast<TValRep<vector<VariantValue>,eArray>*> (fVal.get ());
					AssertNotNull (v);
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
			AssertNotNull (v);
			return v->fVal.Format (Date::eXML_PF);
		}
		case	eDateTime: {
			TValRep<DateTime,eDateTime>*	v	=	dynamic_cast<TValRep<DateTime,eDateTime>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal.Format (DateTime::eXML_PF);
		}
		case	eString: {
			TValRep<wstring,eString>*	v	=	dynamic_cast<TValRep<wstring,eString>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal;
		}
		case	eBoolean: {
			TValRep<bool,eBoolean>*	v	=	dynamic_cast<TValRep<bool,eBoolean>*> (fVal.get ());
			AssertNotNull (v);
			return v->fVal? L"true": L"false";
		}
		case	eFloat: {
			TValRep<float,eFloat>*	v	=	dynamic_cast<TValRep<float,eFloat>*> (fVal.get ());
			AssertNotNull (v);
			return Characters::Float2String (v->fVal);
		}
		default: {
			return As<wstring> ();
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
			return	lhs.As<wstring> () == rhs.As<wstring> ();
		}
		return false;
	}
	switch (lt) {
		case	VariantValue::eNull:		return true;
		case	VariantValue::eBoolean:		return lhs.As<bool> () == rhs.As<bool> ();
		case	VariantValue::eInteger:		return lhs.As<int> () == rhs.As<int> ();
		case	VariantValue::eFloat:		return lhs.As<float> () == rhs.As<float> ();
		case	VariantValue::eDate:		return lhs.As<Date> () == rhs.As<Date> ();
		case	VariantValue::eDateTime:	return lhs.As<DateTime> () == rhs.As<DateTime> ();
		case	VariantValue::eString:		return lhs.As<wstring> () == rhs.As<wstring> ();
		case	VariantValue::eArray: {
			// same iff all elts same
			vector<VariantValue>	lhsV	=	lhs.As<vector<VariantValue>> ();
			vector<VariantValue>	rhsV	=	rhs.As<vector<VariantValue>> ();
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
			map<wstring,VariantValue>	lhsM	=	lhs.As<map<wstring,VariantValue>> ();
			map<wstring,VariantValue>	rhsM	=	rhs.As<map<wstring,VariantValue>> ();
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
