/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_h_
#define _Stroika_Foundation_Streams_TextReader_h_ 1

#include "../StroikaPreComp.h"

#include "InputStream.h"
#include "InternallySyncrhonizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-588 - TextReader should be seekable (maybe just optionally)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-487 - TextReader should have CTOR taking String as argument (so can be seekable)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-274 - TextWriter (and TextReader) should take optional codepage param and maybe overload taking locale
 *              Started - See Added TextReader CTOR overload calling LookupCodeConverter
 *
 *      @todo   DOCUMENT why we put ReadLine etc in InputStreamPtr, instead of here. Gist of why - though more
 *              logical here - requires no state - and so more flexible there. May reconsider.
 *              -- LGP 2015-07-06
 */

namespace Stroika {
    namespace Foundation {
        namespace Memory {
            class BLOB;
        }
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            using Characters::Character;

            /**
             *  \note   This was called TextInputStreamBinaryAdapter
             *
             *  \note   This is similar to the .net TextReader (https://msdn.microsoft.com/en-us/library/system.io.textreader(v=vs.110).aspx) except that
             *          much of the 'reading' API is baked into InputStream<Character>::Ptr.
             *
             *  \par Example Usage
             *      \code
             *      for (String line : TextReader::New (FileInputStream::New (L"/tmp/foo")).ReadLines ()) {
             *      }
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *      Assert (TextReader { String (L"hello world") }.ReadAll () == L"hello world");
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
             */
            class TextReader : public InputStream<Character> {
            public:
                TextReader ()                  = delete;
                TextReader (const TextReader&) = delete;

            public:
                class Ptr;

            public:
                /**
                 * IF handed 'bytes' - the TextReader interprets the bytes (@todo add support for code page spec, and autodetect
                 * etc - https://stroika.atlassian.net/browse/STK-274). 
                 * If handled an InputStream<Character>::Ptr - it just passes through characters.
                 *
                 *  Seekable defaults to true (for Stream and soon everything) since needed for ReadLines () and ReadLine, which is commonly used.
                 *  For the constructor taking const InputStream<Character>::Ptr& src, the seekability mimics that of the original source.
                 *  For the other constructors, they are seekable.
                 *
                 *  But when you specify it expliticly, the given value will be used
                 *
                 *  \note Depending on the underlying source (e.g. binary stream) - maintaining seekability may be expensive in terms
                 *        of memory usage.
                 */
                static Ptr New (const Memory::BLOB& src, const Memory::Optional<Characters::String>& charset = {});
                static Ptr New (const InputStream<Memory::Byte>::Ptr& src, bool seekable = true);
                static Ptr New (const InputStream<Memory::Byte>::Ptr& src, const Memory::Optional<Characters::String>& charset, bool seekable = true);
                static Ptr New (const InputStream<Memory::Byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable = true);
                static Ptr New (const InputStream<Character>::Ptr& src);
                static Ptr New (const Traversal::Iterable<Character>& src);

            private:
                class FromBinaryStreamBaseRep_;
                class UnseekableBinaryStreamRep_;
                class BaseSeekingBinaryStreamRep_;
                class CachingSeekableBinaryStreamRep_;
                class IterableAdapterStreamRep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a ExternallyOwnedMemoryInputStream.
             */
            class TextReader::Ptr : public InputStream<Character>::Ptr {
            private:
                using inherited = typename InputStream<Character>::Ptr;

            public:
                /**
                &&&&&
                *  \par Example Usage
                *      \code
                *          InputStream<Byte>::Ptr in = ExternallyOwnedMemoryInputStream<Byte> (begin (buf), begin (buf) + nBytesRead);
                *      \endcode
                *
                *  \par Example Usage
                *      \code
                *          CallExpectingBinaryInputStreamPtr (ExternallyOwnedMemoryInputStream<Byte> (begin (buf), begin (buf) + nBytesRead))
                *      \endcode
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;
                Ptr (const InputStream<Character>::Ptr& from);

            protected:
                Ptr (const shared_ptr<InputStream<Character>::_IRep>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;

            private:
                friend class TextReader;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextReader.inl"

#endif /*_Stroika_Foundation_Streams_TextReader_h_*/
