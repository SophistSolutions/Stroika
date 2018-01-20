/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_h_
#define _Stroika_Foundation_Characters_StringBuilder_h_ 1

#include "../StroikaPreComp.h"

#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/SmallStackBuffer.h"

#include "String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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
 *      @todo   Add InsertAt() methods - like from String class (before I deprecate them).
 *
 *      @todo   Use ExternallySynchronizedLock when copying from src.
 *
 *      @todo   Consider adding operator==, and or other String methods - esp so can compare as a value
 *              with String. Or maybe add As<> method, and force compare As<String> ()?
 *
 *      @todo   Think about how to add support for STL manipulator/inserters like endl;
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /**
             *
             *  \note   Uses Execution::ExternallySynchronizedLock - so you must externally assure this isn't updated by
             *          one thread while being read or updated on another.
             *
             *  @see String
             *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
             *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
             */
            class StringBuilder : private Debug::AssertExternallySynchronizedLock {
            public:
                StringBuilder ();
                StringBuilder (const StringBuilder&) = default;
                explicit StringBuilder (const String& initialValue);

            public:
                nonvirtual StringBuilder& operator= (const StringBuilder& rhs) = default;

            public:
                /**
                 *  add overloads
                 */
                nonvirtual void Append (const Character* s, const Character* e);
                nonvirtual void Append (const char16_t* s, const char16_t* e);
                nonvirtual void Append (const char16_t* s);
                nonvirtual void Append (const char32_t* s, const char32_t* e);
                nonvirtual void Append (const char32_t* s);
                nonvirtual void Append (const wchar_t* s, const wchar_t* e);
                nonvirtual void Append (const wchar_t* s);
                nonvirtual void Append (const wstring& s);
                nonvirtual void Append (const u16string& s);
                nonvirtual void Append (const u32string& s);
                nonvirtual void Append (const String& s);
                nonvirtual void Append (wchar_t c);
                nonvirtual void Append (Character c);

            public:
                /**
                 *  Alias for Append
                 */
                nonvirtual StringBuilder& operator+= (const char16_t* s);
                nonvirtual StringBuilder& operator+= (const char32_t* s);
                nonvirtual StringBuilder& operator+= (const wchar_t* s);
                nonvirtual StringBuilder& operator+= (const wstring& s);
                nonvirtual StringBuilder& operator+= (const u16string& s);
                nonvirtual StringBuilder& operator+= (const u32string& s);
                nonvirtual StringBuilder& operator+= (const String& s);
                nonvirtual StringBuilder& operator+= (const Character& c);

            public:
                /**
                 *  Alias for Append
                 */
                nonvirtual StringBuilder& operator<< (const String& s);
                nonvirtual StringBuilder& operator<< (const wstring& s);
                nonvirtual StringBuilder& operator<< (const wchar_t* s);
                nonvirtual StringBuilder& operator<< (const Character& c);

            public:
                /**
                 */
                nonvirtual void push_back (Character c);

            public:
                nonvirtual size_t GetLength () const;

            public:
                /**
                 *  Returns true if this is an empty string (aka iff GetLength () == 0);
                 */
                nonvirtual bool empty () const;

            public:
                nonvirtual Character GetAt (size_t index) const;

            public:
                nonvirtual void SetAt (Character item, size_t index);

            public:
                /**
                 * Convert String losslessly into a
                 *      o   String or
                 *      o   wstring
                 */
                template <typename T>
                nonvirtual T As () const;
                template <typename T>
                nonvirtual void As (T* into) const;

            public:
                /*
                 *  explicit operator T () provides an alterntive syntax to As<> - depending on user
                 *  preference or context. Note - its important that this is explicit - to avoid
                 *  creating overload problems
                 *
                 *      EXPERIMENTAL AS OF 2014-02-11 (v2.0a21)
                 */
                nonvirtual explicit operator String () const;
                nonvirtual explicit operator wstring () const;

            public:
                /**
                 *  This ensures nul-character termination. However, it returns an internal pointer only valid
                 *  until the next non-const call to this object.
                 */
                nonvirtual const wchar_t* c_str () const;

            public:
                /**
                 */
                nonvirtual void clear ();

            public:
                /**
                 *  mimic wstringstream method
                 */
                nonvirtual String str () const;

            public:
                /**
                 *  STL-ish alias for GetLength ()
                 */
                nonvirtual size_t length () const;

            public:
                /**
                 *  STL-ish alias for GetLength ()
                 */
                nonvirtual size_t size () const;

            public:
                /**
                 */
                nonvirtual const wchar_t* begin ();

            public:
                /**
                 */
                nonvirtual const wchar_t* end ();

            public:
                /**
                 *  Returns the amount of space reserved - before memory allocation will be needed to grow. 
                 *
                 *  @see reserve
                 */
                nonvirtual size_t capacity () const;

            public:
                /**
                 *  Provide a hint as to how much (contiguous) space to reserve. There is no need to call
                 *  this but when the total size is known in advance, it can improve performance.
                 *
                 *  @see capacity
                 */
                nonvirtual void reserve (size_t newCapacity);

            private:
                mutable Memory::SmallStackBuffer<wchar_t> fData_;   // maybe nul-terminated
                size_t                                    fLength_; // seperate from SmallStackBuffer<>::GetLength ()
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringBuilder.inl"

#endif /*_Stroika_Foundation_Characters_StringBuilder_h_*/
