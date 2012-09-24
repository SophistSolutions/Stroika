/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Memory/BlockAllocated.h"        // #include here because the CPP module depends on BlockAllocated, and this #include needed assure ModuleInit<> proper order


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            template    <>
            inline  size_t  C_String::Length (const char* p)
            {
                RequireNotNull (p);
                return ::strlen (p);
            }
            template    <>
            inline  size_t  C_String::Length (const wchar_t* p)
            {
                RequireNotNull (p);
                return ::wcslen (p);
            }
            template    <>
			inline	void	C_String::Copy (char* dest, const char* src, size_t nEltsInDest)
			{
				RequireNotNull (dest);
				RequireNotNull (src);
				Require (nEltsInDest >= 1);
				char*		destEnd	=	dest + nEltsInDest;
				char*		destI	=	dest;
				const char*	srcI	=	src;
				while ((*dest++ = *srcI++) != '\0') {
					Assert (dest <= destEnd);
					if (dest == destEnd) {
						*(dest-1) = '\0';
						return;
					}
				}
			}
            template    <>
			inline	void	C_String::Copy (wchar_t* dest, const wchar_t* src, size_t nEltsInDest)
			{
				RequireNotNull (dest);
				RequireNotNull (src);
				Require (nEltsInDest >= 1);
				wchar_t*		destEnd	=	dest + nEltsInDest;
				wchar_t*		destI	=	dest;
				const wchar_t*	srcI	=	src;
				while ((*dest++ = *srcI++) != '\0') {
					Assert (dest <= destEnd);
					if (dest == destEnd) {
						*(dest-1) = '\0';
						return;
					}
				}
			}


            //  class   String::_Rep
            inline  String::_Rep::_Rep ()
            {
            }
            inline  String::_Rep::~_Rep ()
            {
            }
            inline  void    String::_Rep::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars  =   GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }
            inline  void    String::_Rep::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars  =   GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }



            //  class   String
            inline  String::String (const String& from)
                : _fRep (from._fRep)
            {
            }
            inline  String& String::operator= (const String& newString)
            {
                _fRep = newString._fRep;
                return (*this);
            }
            inline  String::~String ()
            {
            }
            inline  String::_Rep*   String::Clone_ (const _Rep& rep)
            {
                return (rep.Clone ());
            }
            inline  void    String::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                _fRep->CopyTo (bufFrom, bufTo);
            }
            inline  void    String::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                _fRep->CopyTo (bufFrom, bufTo);
            }
            inline  size_t  String::GetLength () const
            {
                return (_fRep->GetLength ());
            }
            inline  bool    String::empty () const
            {
                return _fRep->GetLength () == 0;
            }
            inline  void    String::clear ()
            {
                *this = String ();
            }
            inline  void    String::InsertAt (Character c, size_t at)
            {
                InsertAt (&c, &c + 1, at);
            }
            inline  void    String::InsertAt (const String& s, size_t at)
            {
                // NB: I don't THINK we need be careful if s.fRep == this->fRep because when we first derefence this->fRep it will force a CLONE, so OUR fRep will be unique
                // And no need to worry about lifetime of 'p' because we don't allow changes to 's' from two different threads at a time, and the rep would rep if accessed from
                // another thread could only change that other envelopes copy
                pair<const Character*, const Character*> d   =   s._fRep->GetData ();
                InsertAt (d.first, d.second, at);
            }
            inline  void    String::InsertAt (const wchar_t* from, const wchar_t* to, size_t at)
            {
                InsertAt (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
            }
            inline  String& String::operator+= (Character appendage)
            {
                InsertAt (appendage, GetLength ());
                return (*this);
            }
            inline  String& String::operator+= (const String& appendage)
            {
                InsertAt (appendage, GetLength ());
                return (*this);
            }
            inline  Character   String::operator[] (size_t i) const
            {
                Require (i >= 0);
                Require (i < GetLength ());
                return (_fRep->GetAt (i));
            }
            template    <>
            inline  void    String::As (wstring* into) const
            {
                RequireNotNull (into);
                size_t  n   =   GetLength ();
                const Character* cp =   _fRep->Peek ();
                Assert (sizeof (Character) == sizeof (wchar_t));        // going to want to clean this up!!!    --LGP 2011-09-01
                const wchar_t* wcp  =   (const wchar_t*)cp;
                into->assign (wcp, wcp + n);
            }
            template    <>
            inline  wstring String::As () const
            {
                wstring r;
                As (&r);
                return r;
            }
            template    <>
            inline  const wchar_t*  String::As () const
            {
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const wchar_t*)_fRep->Peek ();
            }
            template    <>
            inline  const Character*    String::As () const
            {
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const Character*)_fRep->Peek ();
            }
            template    <>
            inline  string  String::AsUTF8 () const
            {
                string  r;
                AsUTF8 (&r);
                return r;
            }
            inline  std::string String::AsUTF8 () const
            {
                return AsUTF8<std::string> ();
            }
            inline  void        String::AsUTF8 (std::string* into) const
            {
                AsUTF8<std::string> (into);
            }
            template    <>
            inline  string  String::AsASCII () const
            {
                string  r;
                AsASCII (&r);
                return r;
            }
            inline  std::string String::AsASCII () const
            {
                return AsASCII<std::string> ();
            }
            inline  void    String::AsASCII (std::string* into) const
            {
                AsASCII<std::string> (into);
            }
            inline  size_t  String::length () const
            {
                return GetLength ();
            }
            inline  size_t  String::size () const
            {
                return GetLength ();
            }
            inline  const wchar_t*  String::data () const
            {
                return As<const wchar_t*> ();
            }
            inline  size_t String::find (wchar_t c) const
            {
                return IndexOf (c);
            }
            inline  size_t String::rfind (wchar_t c) const
            {
                return RIndexOf (c);
            }
            inline  int String::Compare (const String& rhs, CompareOptions co) const
            {
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return _fRep->Compare (d.first, d.second, co);
            }
            inline  int String::Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const
            {
                return _fRep->Compare (rhsStart, rhsEnd, co);
            }
            inline  int String::Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co) const
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return _fRep->Compare (reinterpret_cast<const Character*> (rhsStart), reinterpret_cast<const Character*> (rhsEnd), co);
            }



            inline  bool    operator== (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (true);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, String::eWithCase_CO) == 0;
            }
            inline  bool    operator== (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), String::eWithCase_CO) == 0;
            }
            inline  bool    operator== (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), String::eWithCase_CO) == 0;
            }
            inline  bool    operator< (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (false);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, String::eWithCase_CO) < 0;
            }
            inline  bool    operator< (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), String::eWithCase_CO) >= 0;
            }
            inline  bool    operator< (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), String::eWithCase_CO) < 0;
            }
            inline  bool    operator<= (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (false);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, String::eWithCase_CO) <= 0;
            }
            inline  bool    operator<= (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), String::eWithCase_CO) > 0;
            }
            inline  bool    operator<= (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), String::eWithCase_CO) <= 0;
            }
            inline  bool    operator!= (const String& lhs, const String& rhs)
            {
                return (bool (not (lhs == rhs)));
            }
            inline  bool    operator!= (const wchar_t* lhs, const String& rhs)
            {
                return (bool (not (lhs == rhs)));
            }
            inline  bool    operator!= (const String& lhs, const wchar_t* rhs)
            {
                return (bool (not (lhs == rhs)));
            }
            inline  bool    operator> (const String& lhs, const String& rhs)
            {
                return (bool (not (lhs <= rhs)));
            }
            inline  bool    operator> (const wchar_t* lhs, const String& rhs)
            {
                return (bool (not (lhs <= rhs)));
            }
            inline  bool    operator> (const String& lhs, const wchar_t* rhs)
            {
                return (bool (not (lhs <= rhs)));
            }
            inline  bool    operator>= (const String& lhs, const String& rhs)
            {
                return (bool (not (lhs < rhs)));
            }
            inline  bool    operator>= (const wchar_t* lhs, const String& rhs)
            {
                return (bool (not (lhs < rhs)));
            }
            inline  bool    operator>= (const String& lhs, const wchar_t* rhs)
            {
                return (bool (not (lhs < rhs)));
            }


#if 0
            //  class   String_CharArray
            inline  String_CharArray::String_CharArray (const String_CharArray& s)
                : String (s)
            {
            }
            inline  String_CharArray& String_CharArray::operator= (const String_CharArray& s)
            {
                String::operator= (s);
                return (*this);
            }
#endif




            //  class   String_BufferedArray
            inline  String_BufferedArray::String_BufferedArray (const String_BufferedArray& s)
                : String (s)
            {
            }
            inline  String_BufferedArray& String_BufferedArray::operator= (const String_BufferedArray& s)
            {
                String::operator= (s);
                return (*this);
            }



            //  class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
                : String (s)
            {
            }
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
            {
                String::operator= (s);
                return (*this);
            }


            //  class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
                : String (s)
            {
            }
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
            {
                String::operator= (s);
                return (*this);
            }
        }
    }
}


#endif // _Stroika_Foundation_Characters_String_inl_
