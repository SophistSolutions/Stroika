/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {
	namespace	Foundation {
		namespace	Characters {



			//	class	String::StringRep
            inline	String::StringRep::StringRep ()
				{
				}


			//	class	String
			inline	String::String (const String& from)
				: fRep (from.fRep)
				{
				}
            inline	String&	String::operator= (const String& newString)
				{
					fRep = newString.fRep;
					return (*this);
				}
            inline	String::~String ()
				{
				}
			inline	String::StringRep*	String::Clone_ (const StringRep& rep)
				{
					return (rep.Clone ());
				}
            inline	void	String::RemoveAt (size_t i)
				{
					RemoveAt (i, 1);
				}
            inline	const Character*	String::Peek () const
				{
					return (fRep->Peek ());
				}
            inline	size_t	String::GetLength () const
				{
					return (fRep->GetLength ());
				}
            inline	Character	String::operator[] (size_t i) const
				{
					Require (i >= 0);
					Require (i < GetLength ());
					return (fRep->GetAt (i));
				}
#if 0
            inline	void	String::SetRep_ (StringRep* rep)
				{
					fRep = Memory::CopyOnWrite<StringRep> (rep, &Clone_);
				}
            inline	const String::StringRep*	String::GetRep_ () const
				{
					return (fRep.GetPointer ());
				}
            inline	String::StringRep*			String::GetRep_ ()
				{
					return (fRep.GetPointer ());
				}
#endif



			inline	bool	operator!= (const String& lhs, const String& rhs)
				{
					return (bool (not (lhs == rhs)));
				}
            inline	bool	operator!= (const wchar_t* lhs, const String& rhs)
				{
					return (bool (not (lhs == rhs)));
				}
            inline	bool	operator!= (const String& lhs, const wchar_t* rhs)
				{
					return (bool (not (lhs == rhs)));
				}
            inline	bool	operator> (const String& lhs, const String& rhs)
				{
					return (bool (not (lhs <= rhs)));
				}
            inline	bool	operator> (const wchar_t* lhs, const String& rhs)
				{
					return (bool (not (lhs <= rhs)));
				}
            inline	bool	operator> (const String& lhs, const wchar_t* rhs)
				{
					return (bool (not (lhs <= rhs)));
				}
            inline	bool	operator>= (const String& lhs, const String& rhs)
				{
					return (bool (not (lhs < rhs)));
				}
            inline	bool	operator>= (const wchar_t* lhs, const String& rhs)
				{
					return (bool (not (lhs < rhs)));
				}
            inline	bool	operator>= (const String& lhs, const wchar_t* rhs)
				{
					return (bool (not (lhs < rhs)));
				}


			//	class	String_CharArray
			inline	String_CharArray::String_CharArray (const String_CharArray& s)
				: String (s)
				{
				}
            inline	String_CharArray& String_CharArray::operator= (const String_CharArray& s)
				{
					String::operator= (s);
					return (*this);
				}


			//	class	String_BufferedCharArray
			inline	String_BufferedCharArray::String_BufferedCharArray (const String_BufferedCharArray& s)
				: String (s)
				{
				}
            inline	String_BufferedCharArray& String_BufferedCharArray::operator= (const String_BufferedCharArray& s)
				{	
					String::operator= (s);	
					return (*this);
				}


			//	class	String_ReadOnlyChar
			inline	String_ReadOnlyChar::String_ReadOnlyChar (const String_ReadOnlyChar& s)
				: String (s)
				{
				}
            inline	String_ReadOnlyChar& String_ReadOnlyChar::operator= (const String_ReadOnlyChar& s)
				{
					String::operator= (s);
					return (*this);
				}
		}
	}
}


#endif // _Stroika_Foundation_Characters_String_inl_
