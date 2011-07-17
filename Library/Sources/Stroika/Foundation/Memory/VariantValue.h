/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_VariantValue_h_
#define	_Stroika_Foundation_Memory_VariantValue_h_	1

#include	"../StroikaPreComp.h"

#include	<map>
#include	<vector>

#include	"../Configuration/Common.h"
#include	"../Time/DateUtils.h"

#include	"RefCntPtr.h"


// MIMICS (eventually - not totally yet) COM "VARIANT" type - not so much for COM integration, but
// cuz its handy having such a thing (e.g. as gateway API to XML reader/writer code or JSON (my original motivaiton to write this now)
//		-- LGP 2011-07-16


// NOT SURE THIS LOGICALLY BELONGS IN "MEMORY" module but wasnt sure where else to put it.

// ALSO, BASED on R4LLib "Value" type - but with Enumeration/ID/BLOB removed. 
//When we add ATOM support to Storika (parameterized based on R4LLIb Enumeration)
// we can add that back here - possibly?
// Also - must add back BLOB type!!!


// ADD MORE INT TYPES (maybe int32/int64, or maybe long long and regular int?)


// NB: This code is ALMOST the same as R4LLib::DTypes::Value - but not close enuf and not re-usable enuf to be used at all there.
// THATS too bad!!! - any way to make this extensible?

// WARN: DESIGN FLAW - BECAUSE OF INTERNAL USE OF REFCNTPTR, ONE CAN CREATE CIRCULAR REFERNCES, WHICH WOULD INFINITE LOOP ON PRINT (and other such).
// SHOULD FIND A BETTER WAY TO BREAK THIS DOWN/PREVENT (maybe copy in array/map case?). Anyhow - it would only be in rare, or user-error cases
//

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {

			using	Memory::RefCntPtr;
			using	Time::Date;
			using	Time::DateTime;
	
			// Simple refcounted copying value objects
			class	VariantValue {
				public:
					enum Type {
						eNull,
						eBoolean,
						eInteger,
						eFloat,
						eEnumeration,
						eDate,
						eDateTime,
						eString,
						eArray,
						eMap,			// string to variant
					};

				public:
					VariantValue ();
					VariantValue (bool val);
					VariantValue (int val);
					VariantValue (float val);
					VariantValue (const Date& val);
					VariantValue (const DateTime& val);
					VariantValue (const wstring& val);
					VariantValue (const wchar_t* val);
					VariantValue (const map<wstring,VariantValue>& val);
					VariantValue (const vector<VariantValue>& val);

				private:
					VariantValue (const string& val);		// not implemneted - declared to avoid confusing automatic type conversions - use wstring/wchar_t*
					VariantValue (const char* val);			// ""

				public:
					nonvirtual	Type	GetType () const;
					nonvirtual	bool	empty () const;

				public:
					nonvirtual operator bool () const;
					nonvirtual operator int () const;
					nonvirtual operator float () const;
					nonvirtual operator Date () const;
					nonvirtual operator DateTime () const;
					nonvirtual operator wstring () const;
					nonvirtual operator map<wstring,VariantValue> () const;
					nonvirtual operator vector<VariantValue> () const;

				public:
					// bad name - historical - not sure whats better??? RETHINK - LGP 2011-07-16
					nonvirtual	wstring	FormatXML () const;

				private:
					struct	ValRep;
					RefCntPtr<ValRep>	fVal;

				private:
					template	<typename T, Type t>	struct	TValRep;
			};
			bool	operator== (const VariantValue& lhs, const VariantValue& rhs);
			bool	operator!= (const VariantValue& lhs, const VariantValue& rhs);

		}
	}
}
#endif	/*_Stroika_Foundation_Memory_VariantValue_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"VariantValue.inl"
