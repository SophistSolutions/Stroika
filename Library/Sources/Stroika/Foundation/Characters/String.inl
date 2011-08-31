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

                private:
                    wchar_t*	fStorage;
                    size_t	    fLength;

                    virtual	size_t	CalcAllocSize (size_t requested);
            };


            /*
             * Subclasses from StringRep_CharArray instead of StringRep as a convenience (inheriting implementation).
             * In nearly all cases, such inheritance is a bad idea, but here it is justified because people
             * never directly manipulate the Reps, only the envelope classes, which have the conceptually
             * proper derivation. And the code savings is significant, since they differ only in
             * their buffering schemes.
             * Of course, not all subclasses of StringRep can do this, and if it ever posed a problem here it
             * could be modified without having any effect on peoples code.
             *
             */
            class	StringRep_BufferedCharArray : public StringRep_CharArray {
                public:
                    StringRep_BufferedCharArray (const Character* arrayOfCharacters, size_t nBytes);

                    virtual		StringRep*	Clone () const override;

                private:
                    virtual		size_t	CalcAllocSize (size_t requested) override;
            };

            /*
             * Subclasses from StringRep_CharArray instead of StringRep as a convenience (inheriting implementation).
             * In nearly all cases, such inheritance is a bad idea, but here it is justified because people
             * never directly manipulate the Reps, only the envelope classes, which have the conceptually
             * proper derivation. This class differs from StringRep_CharArray only in that it normally does
             * not allocate any memory, simply pointing at the memory the user provides. However, it must
             * have all the capabilities of the full StringRep_CharArray, so that if forced to modify itself,
             * it can do so in a safe manner. This could be implemented through ownership instead of inheritance,
             * but that would be less efficient in a place where efficiency counts.
             * Of course, not all subclasses of StringRep can do this, and if it ever posed a problem here it
             * could be modified without having any effect on peoples code.
             *
             * One interesting thing to note is that we do not override the Clone method here. This is
             * justified, in that the Clone method only requires that we return a StringRep*, not an exact
             * duplicate. And it is a good idea since we only call Clone when we wish to apply modifications,
             * but StringRep_ReadOnlyChar is a class that dislikes applying modifications.
             */
            class	StringRep_ReadOnlyChar : public StringRep_CharArray {
                public:
                    StringRep_ReadOnlyChar (const Character* arrayOfCharacters, size_t nBytes);
                    ~StringRep_ReadOnlyChar ();

                    virtual		void	RemoveAll () override;

                    virtual		void	SetAt (Character item, size_t index) override;
                    virtual		void	InsertAt (Character item, size_t index) override;
                    virtual		void	RemoveAt (size_t index, size_t amountToRemove) override;

                    virtual		void	SetLength (size_t newLength) override;

                private:
                    bool	fAllocedMem;

                    // called before calling any method that modifies the string, to ensure that
                    // we do not munge memory we did not alloc
                    nonvirtual	void	AssureMemAllocated ();
            };




            class	StringRep_Substring : public String::StringRep {
                public:
                    StringRep_Substring (const Memory::Shared<StringRep>& baseString, size_t from, size_t length);


                    virtual		StringRep*	Clone () const override;

                    virtual		size_t	GetLength () const;
                    virtual		bool	Contains (Character item) const override;
                    virtual		void	RemoveAll () override;

                    virtual		Character	GetAt (size_t index) const override;
                    virtual		void		SetAt (Character item, size_t index) override;
                    virtual		void		InsertAt (Character item, size_t index) override;
                    virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

                    virtual		void	SetLength (size_t newLength) override;

                    virtual		const Character*	Peek () const override;

                private:
                    Memory::Shared<StringRep>	fBase;

                    size_t	fFrom;
                    size_t	fLength;
            };


            class	StringRep_Catenate : public String::StringRep {
                public:
                    StringRep_Catenate (const String& lhs, const String& rhs);

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

                private:
                    String			fLeft;
                    String			fRight;
                    size_t	fLength;

                    nonvirtual	void	Normalize ();
            };

            inline	String::StringRep::StringRep ()
            {
            }


			inline	String::String (const String& from)
				: fRep (from.fRep)
            {
            }


            inline	String::String (const std::wstring& r)
                : fRep (&Clone_, 0)
            {
                fRep = new StringRep_CharArray ((const Character*)r.c_str (), r.length ());
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
                fRep = rep;
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
