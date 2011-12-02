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

			template	<>
				inline	size_t	C_String::Length (const char* p)
					{
						RequireNotNull (p);
						return ::strlen (p);
					}
			template	<>
				inline	size_t	C_String::Length (const wchar_t* p)
					{
						RequireNotNull (p);
						return ::wcslen (p);
					}


			//	class	String::_Rep
            inline	String::_Rep::_Rep ()
				{
				}
            inline	String::_Rep::~_Rep ()
				{
				}
			inline	void	String::_Rep::CopyTo (Character* bufFrom, Character* bufTo) const
				{
					RequireNotNull (bufFrom);
					Require (bufFrom + GetLength () >= bufTo);
					size_t	nChars	=	GetLength ();
					(void)::memcpy (bufFrom, Peek (), nChars*sizeof (Character));
				}
			inline	void	String::_Rep::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
				{
					RequireNotNull (bufFrom);
					Require (bufFrom + GetLength () >= bufTo);
					size_t	nChars	=	GetLength ();
					(void)::memcpy (bufFrom, Peek (), nChars*sizeof (Character));
				}



			//	class	String
			inline	String::String (const String& from)
				: fRep_ (from.fRep_)
				{
				}
            inline	String&	String::operator= (const String& newString)
				{
					fRep_ = newString.fRep_;
					return (*this);
				}
            inline	String::~String ()
				{
				}
			inline	String::_Rep*	String::Clone_ (const _Rep& rep)
				{
					return (rep.Clone ());
				}
			inline	void	String::CopyTo (Character* bufFrom, Character* bufTo) const
				{
					RequireNotNull (bufFrom);
					Require (bufFrom + GetLength () >= bufTo);
					fRep_->CopyTo (bufFrom, bufTo);
				}
			inline	void	String::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
				{
					RequireNotNull (bufFrom);
					Require (bufFrom + GetLength () >= bufTo);
					fRep_->CopyTo (bufFrom, bufTo);
				}
            inline	size_t	String::GetLength () const
				{
					return (fRep_->GetLength ());
				}
			inline	bool	String::empty () const
				{
					return fRep_->GetLength () == 0;
				}
			inline	void	String::clear ()
				{
					*this = String ();
				}
            inline	Character	String::operator[] (size_t i) const
				{
					Require (i >= 0);
					Require (i < GetLength ());
					return (fRep_->GetAt (i));
				}
			template	<>
				inline	void	String::As (wstring* into) const
					{
						RequireNotNull (into);
						size_t	n	=	GetLength ();
						const Character* cp	=	fRep_->Peek ();
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
						return (const wchar_t*)fRep_->Peek ();
					}
			template	<>
				inline	string	String::AsUTF8 () const
					{
						string	r;
						AsUTF8 (&r);
						return r;
					}
			inline	std::string	String::AsUTF8 () const
				{
					return AsUTF8<std::string> ();
				}
			inline	void		String::AsUTF8 (std::string* into) const
				{
					AsUTF8<std::string> (into);
				}
			template	<>
				inline	string	String::AsASCII () const
					{
						string	r;
						AsASCII (&r);
						return r;
					}
			inline	std::string	String::AsASCII () const
				{
					return AsASCII<std::string> ();
				}
			inline	void	String::AsASCII (std::string* into) const
				{
					AsASCII<std::string> (into);
				}
			inline	size_t	String::length () const
				{
					return GetLength ();
				}
			inline	size_t	String::size () const
				{
					return GetLength ();
				}
			inline	const wchar_t*	String::data () const
				{
					return As<const wchar_t*> ();
				}
			inline	size_t String::find (wchar_t c) const
				{
					return IndexOf (c);
				}
			inline	size_t String::rfind (wchar_t c) const
				{
					return RIndexOf (c);
				}
			inline	int	String::Compare (const String& rhs, CompareOptions co) const
				{
					return fRep_->Compare (*rhs.fRep_.GetPointer (), co);
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


			//	class	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly
			inline	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
				: String (s)
				{
				}
            inline	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
				{
					String::operator= (s);
					return (*this);
				}


			//	class	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
			inline	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
				: String (s)
				{
				}
            inline	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
				{
					String::operator= (s);
					return (*this);
				}
		}
	}
}


#endif // _Stroika_Foundation_Characters_String_inl_
