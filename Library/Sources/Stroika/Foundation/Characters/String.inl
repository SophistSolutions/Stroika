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

			class	StringRep_CharArray : public String::StringRep {
				public:
					StringRep_CharArray (const Character* arrayOfCharacters, size_t nBytes);
					~StringRep_CharArray ();

					virtual		StringRep*	Clone () const override;

					virtual		size_t	GetLength () const override;
					virtual		bool	Contains (Character item) const override;
					virtual		void	RemoveAll () override;

					virtual		Character	GetAt (size_t index) const override;
					virtual		void		SetAt (Character item, size_t index) override;
					virtual		void		InsertAt (Character item, size_t index) override;
					virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

					virtual		void	SetLength (size_t newLength) override;

					virtual		const Character*	Peek () const override;

				protected:
					StringRep_CharArray ();
					nonvirtual	void	SetStorage (Character* storage, size_t length);

					virtual	size_t	CalcAllocChars (size_t requested);

				private:
					wchar_t*	fStorage;
					size_t	    fLength;
			};



            inline	String::StringRep::StringRep ()
				{
				}


			inline	String::String (const String& from)
				: fRep (from.fRep)
				{
				}
            inline	String::String (const std::wstring& r)
                : fRep (new StringRep_CharArray ((const Character*)r.c_str (), r.length ()), &Clone_)
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
            inline	void	String::SetRep (StringRep* rep)
				{
					fRep = SHARED<StringRep> (rep, &Clone_);
				}
            inline	const String::StringRep*	String::GetRep () const
				{
					return (fRep.GetPointer ());
				}
            inline	String::StringRep*			String::GetRep ()
				{
					return (fRep.GetPointer ());
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


			inline	String_CharArray::String_CharArray (const String_CharArray& s)	: String (s)	{}
            inline	String_CharArray& String_CharArray::operator= (const String_CharArray& s)	{	String::operator= (s);	return (*this); }


			inline	String_BufferedCharArray::String_BufferedCharArray (const String_BufferedCharArray& s)	: String (s)	{}
            inline	String_BufferedCharArray& String_BufferedCharArray::operator= (const String_BufferedCharArray& s)	{	String::operator= (s);	return (*this); }


			inline	String_ReadOnlyChar::String_ReadOnlyChar (const String_ReadOnlyChar& s)	: String (s)	{}
            inline	String_ReadOnlyChar& String_ReadOnlyChar::operator= (const String_ReadOnlyChar& s)	{	String::operator= (s);	return (*this); }
		}
	}
}


#endif // _Stroika_Foundation_Characters_String_inl_
