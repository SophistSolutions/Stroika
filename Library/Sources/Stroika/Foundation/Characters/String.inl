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
			inline	void	String::CopyTo (Character* buf, size_t nCharsInBuf)
				{
// THIS ASSUMES Peek() always returns non-null and Nul-terminated string - VERIFY IF THAT ASSUMPTION IS TRUE AND DOCUMENT IN Peek() API
					Require (nCharsInBuf > GetLength ());
					RequireNotNull (buf);
					size_t	nChars	=	GetLength () + 1;
					(void)::memcpy (buf, fRep->Peek (), nChars*sizeof (Character));
				}
			inline	void	String::CopyTo (wchar_t* buf, size_t nCharsInBuf)
				{
// THIS ASSUMES Peek() always returns non-null and Nul-terminated string - VERIFY IF THAT ASSUMPTION IS TRUE AND DOCUMENT IN Peek() API
					Require (nCharsInBuf > GetLength ());
					RequireNotNull (buf);
					size_t	nChars	=	GetLength () + 1;
					(void)::memcpy (buf, fRep->Peek (), nChars*sizeof (Character));
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
			template	<>
				inline	void	String::As (wstring* into) const
					{
						RequireNotNull (into);
						size_t	n	=	GetLength ();
						const Character* cp	=	Peek ();
						Assert (sizeof (Character) == sizeof (wchar_t));		// going to want to clean this up!!!	--LGP 2011-09-01
						const wchar_t* wcp	=	(const wchar_t*)cp;
						into->assign (wcp, wcp + n);
					}
			template	<>
				inline	wstring	String::As () const
					{
						wstring	r;
						As (&r);
						return r;
					}
			template	<>
				inline	const wchar_t*	String::As () const
					{
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!	-- LGP 2011-09-01
						return (const wchar_t*)Peek ();
					}
			template	<>
				inline	string	String::AsUTF8 () const
					{
						string	r;
						AsUTF8 (&r);
						return r;
					}
			template	<>
				inline	string	String::AsASCII () const
					{
						string	r;
						AsASCII (&r);
						return r;
					}





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
