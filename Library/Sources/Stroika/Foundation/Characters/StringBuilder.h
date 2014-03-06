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
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *
             *  SUPER ROUGH DRAFT
             *
             *  \note - THIS IS NOT THREADSAFE!!!!!(DETAILS)
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
                nonvirtual  StringBuilder&  operator+= (const wchar_t* s);
                nonvirtual  StringBuilder&  operator+= (const String& s);

            public:
                /**
                 *  Alias for operator+=
                 */
                nonvirtual  StringBuilder& operator<< (const String& s);
                nonvirtual  StringBuilder& operator<< (const wchar_t* s);

            public:
                /**
                 */
                nonvirtual  operator String () const;

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
                mutable Memory::SmallStackBuffer<wchar_t>       fData_;     // keep nul-terminated
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
