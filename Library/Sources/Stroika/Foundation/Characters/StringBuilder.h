/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_h_
#define _Stroika_Foundation_Characters_StringBuilder_h_    1

#include    "../StroikaPreComp.h"

#include    "../Execution/ExternallySynchronizedLock.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "String.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * Description:
 *
 *
 * TODO:
 *      @todo   Add 'formatting' like wstringstream - so you can append ints, etc. But unclear how to
 *              format these, so think out carefully
 *
 *              Maybe find a way to re-use the stream inserters from iostream (vector to them?)
 *
 *      @todo   Add Append() and InsertAt() methods - like from String class (before I deprecate them).
 *
 *      @todo   Use ExternallySynchronizedLock when copying from src.
 *
 *      @todo   Consider adding operator==, and or other String methods - esp so can compare as a value
 *              with String. Or maybe add As<> method, and force compare As<String> ()?
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *
             *  SUPER ROUGH DRAFT
             *
             *  \note - THIS IS NOT THREADSAFE!!!!!(DETAILS) - BY DESIGN
             *
             *  \note   Uses Execution::ExternallySynchronizedLock - so you must externally assure this isn't updated by
             *          one thread while being read or updated on another.
             *
             *  @see String
             *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
             *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
             */
            class   StringBuilder {
            public:
                StringBuilder ();
                explicit StringBuilder (const String& initialValue);

            public:
                /**
                 *  add overloads
                 */
                nonvirtual  void    Append (const wchar_t* s, const wchar_t* e);
                nonvirtual  void    Append (const wchar_t* s);
                nonvirtual  void    Append (const wstring& s);
                nonvirtual  void    Append (const String& s);

            public:
                /**
                 *  Alias for Append
                 */
                nonvirtual  StringBuilder&  operator+= (const wchar_t* s);
                nonvirtual  StringBuilder&  operator+= (const wstring& s);
                nonvirtual  StringBuilder&  operator+= (const String& s);

            public:
                /**
                 *  Alias for Append
                 */
                nonvirtual  StringBuilder& operator<< (const String& s);
                nonvirtual  StringBuilder& operator<< (const wstring& s);
                nonvirtual  StringBuilder& operator<< (const wchar_t* s);

            public:
                /**
                 */
                nonvirtual  void    push_back (Character c);

            public:
                /**
                 * Convert String losslessly into a
                 *      o   String or
                 *      o   wstring
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                template    <typename   T>
                nonvirtual  void    As (T* into) const;

            public:
                /*
                 *  explicit operator T () provides an alterntive syntax to As<> - depending on user
                 *  preference or context. Note - its important that this is explicit - to avoid
                 *  creating overload problems
                 *
                 *      EXPERIMENTAL AS OF 2014-02-11 (v2.0a21)
                 */
                nonvirtual  explicit operator String () const;
                nonvirtual  explicit operator wstring () const;

            public:
                /**
                 */
                nonvirtual  const wchar_t* c_str () const;

            public:
                /**
                 *  mimic wstringstream method
                 */
                nonvirtual  String str () const;

            private:
                mutable Memory::SmallStackBuffer<wchar_t>       fData_;     // maybe nul-terminated
                size_t                                          fLength_;   // seperate from SmallStackBuffer<>::GetLength ()
                mutable Execution::ExternallySynchronizedLock   fLock_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StringBuilder.inl"

#endif  /*_Stroika_Foundation_Characters_StringBuilder_h_*/
