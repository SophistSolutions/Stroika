/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
            ********************************************************************************
            ********************************* String::_IRep ********************************
            ********************************************************************************
            */
            inline  String::_IRep::_IRep ()
            {
            }
            inline  String::_IRep::~_IRep ()
            {
            }
            inline  void    String::_IRep::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars  =   GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }
            inline  void    String::_IRep::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars  =   GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }


            /*
            ********************************************************************************
            ************************************* String ***********************************
            ********************************************************************************
            */
            inline  String::String (const String& from)
                : _fRep (from._fRep)
            {
            }
            inline  String::String (const String && from)
                : _fRep (from._fRep)
            {
            }
            inline  String& String::operator= (const String& newString)
            {
                _fRep = newString._fRep;
                return *this;
            }
            inline  String::~String ()
            {
            }
            inline  shared_ptr<String::_IRep>   String::_Clone (const _IRep& rep)
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
            inline  void    String::Append (Character c)
            {
                InsertAt (c, GetLength ());
            }
            inline  void    String::Append (const String& s)
            {
                InsertAt (s, GetLength ());
            }
            inline  void    String::Append (const wchar_t* from, const wchar_t* to)
            {
                InsertAt (from, to, GetLength ());
            }
            inline  void    String::Append (const Character* from, const Character* to)
            {
                InsertAt (from, to, GetLength ());
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
#if     !qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
            template    <typename   T>
            T   String::As () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
            template    <typename   T>
            void    String::As (T* into) const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
            template    <typename   T>
            T   String::AsUTF8 () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
            template    <typename   T>
            void    String::AsUTF8 (T* into) const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
            template    <typename   T>
            T   String::AsASCII () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
            template    <typename   T>
            void    String::AsASCII (T* into) const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
#endif
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
            inline  void    String::push_back (wchar_t c)
            {
                InsertAt (c, GetLength ());
            }
            inline  void    String::push_back (Character c)
            {
                InsertAt (c, GetLength ());
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


            /*
            ********************************************************************************
            ******************************* Operators **************************************
            ********************************************************************************
            */
            inline  bool    operator== (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (true);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, CompareOptions::eWithCase) == 0;
            }
            inline  bool    operator== (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), CompareOptions::eWithCase) == 0;
            }
            inline  bool    operator== (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), CompareOptions::eWithCase) == 0;
            }
            inline  bool    operator< (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (false);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, CompareOptions::eWithCase) < 0;
            }
            inline  bool    operator< (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), CompareOptions::eWithCase) >= 0;
            }
            inline  bool    operator< (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), CompareOptions::eWithCase) < 0;
            }
            inline  bool    operator<= (const String& lhs, const String& rhs)
            {
                if (lhs._fRep == rhs._fRep) {
                    return (false);
                }
                pair<const Character*, const Character*> d   =   rhs._fRep->GetData ();
                return lhs.Compare (d.first, d.second, CompareOptions::eWithCase) <= 0;
            }
            inline  bool    operator<= (const wchar_t* lhs, const String& rhs)
            {
                RequireNotNull (lhs);
                return rhs.Compare (lhs, lhs + ::wcslen (lhs), CompareOptions::eWithCase) > 0;
            }
            inline  bool    operator<= (const String& lhs, const wchar_t* rhs)
            {
                RequireNotNull (rhs);
                return lhs.Compare (rhs, rhs + ::wcslen (rhs), CompareOptions::eWithCase) <= 0;
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


            /*
            ********************************************************************************
            ********************************** Equals **************************************
            ********************************************************************************
            */
            inline  bool Equals (const String& lhs, const String& rhs, CompareOptions co)
            {
                return lhs.Compare (rhs, co) == 0;
            }


            /*
            ********************************************************************************
            ************************** String_BufferedArray ********************************
            ********************************************************************************
            */
            inline  String_BufferedArray::String_BufferedArray (const String_BufferedArray& s)
                : String (s)
            {
            }
            inline  String_BufferedArray& String_BufferedArray::operator= (const String_BufferedArray& s)
            {
                String::operator= (s);
                return (*this);
            }


            /*
            ********************************************************************************
            ********* String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly **********
            ********************************************************************************
            */
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
                : String (s)
            {
            }
            inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
            {
                String::operator= (s);
                return (*this);
            }


            /*
            ********************************************************************************
            ********* String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite *********
            ********************************************************************************
            */
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

namespace   Stroika {
    namespace   Foundation {
		namespace Memory {
            extern  Execution::DependableModule _BlockAllocated_;
		}
        namespace   Characters {
            namespace   Private {
                struct  String_ModuleInit_ {
					String_ModuleInit_ ()
						: fBlockAllocationDependency (Memory::_BlockAllocated_.GetDependency ())
					{
					}
					Execution::DependableModule::Dependency	fBlockAllocationDependency;
                };
            }
	    }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Characters::Private::String_ModuleInit_>   _Stroika_Foundation_Characters_String_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}

#endif // _Stroika_Foundation_Characters_String_inl_
