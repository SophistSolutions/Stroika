/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_inl_
#define _Stroika_Foundation_Streams_InputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Characters/StringBuilder.h"
#include "../Debug/Assertions.h"
#include "../Debug/Cast.h"
#include "EOFException.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     *********************** InputStream<ELEMENT_TYPE>::_IRep ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    optional<size_t> InputStream<ELEMENT_TYPE>::_IRep::_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (ElementType* intoStart,
                                                                                                                        ElementType* intoEnd,
                                                                                                                        size_t elementsRemaining)
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        if (intoStart == nullptr) {
            return elementsRemaining;
        }
        else {
            return elementsRemaining == 0 ? 0 : Read (intoStart, intoEnd); // safe to call beacuse this cannot block - there are elements available
        }
    }

    /*
     ********************************************************************************
     *********************** InputStream<ELEMENT_TYPE>::Ptr *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline InputStream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep)
        : inherited{rep}
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline InputStream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline void InputStream<ELEMENT_TYPE>::Ptr::Close () const
    {
        Require (IsOpen ());
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _GetRepRWRef ().CloseRead ();
    }
    template <typename ELEMENT_TYPE>
    inline void InputStream<ELEMENT_TYPE>::Ptr::Close (bool reset)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        _GetRepRWRef ().CloseRead ();
        if (reset) {
            this->reset ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline bool InputStream<ELEMENT_TYPE>::Ptr::IsOpen () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _GetRepConstRef ().IsOpenRead ();
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
    {
        return Debug::UncheckedDynamicPointerCast<_IRep> (inherited::_GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
    {
        EnsureMember (&inherited::_GetRepConstRef (), _IRep);
        // reinterpret_cast faster than dynamic_cast - check equivilent
        Assert (dynamic_cast<const _IRep*> (&inherited::_GetRepConstRef ()) == reinterpret_cast<const _IRep*> (&inherited::_GetRepConstRef ()));
        return *reinterpret_cast<const _IRep*> (&inherited::_GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
    {
        EnsureMember (&inherited::_GetRepRWRef (), _IRep);
        // reinterpret_cast faster than dynamic_cast - check equivilent
        Assert (dynamic_cast<_IRep*> (&inherited::_GetRepRWRef ()) == reinterpret_cast<_IRep*> (&inherited::_GetRepRWRef ()));
        return *reinterpret_cast<_IRep*> (&inherited::_GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffset () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        return _GetRepConstRef ().GetReadOffset ();
    }
    template <typename ELEMENT_TYPE>
    SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::GetOffsetToEndOfStream () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        SeekOffsetType savedReadFrom = GetOffset ();
        SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
        Seek (Whence::eFromStart, savedReadFrom);
        Assert (size >= savedReadFrom);
        size -= savedReadFrom;
        return size;
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::Seek (SeekOffsetType offset) const
    {
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Require (this->IsSeekable ());
        return _GetRepRWRef ().SeekRead (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream<ELEMENT_TYPE>::Ptr::Seek (Whence whence, SignedSeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        Require (this->IsSeekable ());
        return _GetRepRWRef ().SeekRead (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream<ELEMENT_TYPE>::Ptr::Read () const -> optional<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        ElementType b{};
        return (_GetRepRWRef ().Read (&b, &b + 1) == 0) ? optional<ElementType>{} : b;
    }
    template <typename ELEMENT_TYPE>
    inline size_t InputStream<ELEMENT_TYPE>::Ptr::Read (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        RequireNotNull (intoStart);
        Require ((intoEnd - intoStart) >= 1);
        return _GetRepRWRef ().Read (intoStart, intoEnd);
    }
    template <typename ELEMENT_TYPE>
    auto InputStream<ELEMENT_TYPE>::Ptr::Peek () const -> optional<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read ();
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    size_t InputStream<ELEMENT_TYPE>::Ptr::Peek (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read (intoStart, intoEnd);
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    inline bool InputStream<ELEMENT_TYPE>::Ptr::IsAtEOF () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        return not Peek ().has_value ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        return _GetRepRWRef ().ReadNonBlocking (nullptr, nullptr);
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream<ELEMENT_TYPE>::Ptr::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (intoStart);
        Require ((intoEnd - intoStart) >= 1);
        Require (IsOpen ());
        return _GetRepRWRef ().ReadNonBlocking (intoStart, intoEnd);
    }
    template <typename ELEMENT_TYPE>
    inline Characters::Character InputStream<ELEMENT_TYPE>::Ptr::ReadCharacter () const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        Characters::Character c;
        if (Read (&c, &c + 1) == 1) {
            return c;
        }
        return '\0';
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    POD_TYPE InputStream<ELEMENT_TYPE>::Ptr::ReadRaw () const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        static_assert (is_pod_v<POD_TYPE>);
        POD_TYPE tmp; // intentionally don't zero-initialize
        size_t   n{ReadAll (reinterpret_cast<byte*> (&tmp), reinterpret_cast<byte*> (&tmp + 1))};
        if (n == sizeof (tmp)) [[likely]] {
            return tmp;
        }
        else {
            Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
        }
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline void InputStream<ELEMENT_TYPE>::Ptr::ReadRaw (POD_TYPE* start, POD_TYPE* end) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (IsOpen ());
        static_assert (is_pod_v<POD_TYPE>);
        size_t n{ReadAll (reinterpret_cast<byte*> (start), reinterpret_cast<byte*> (end))};
        if (n != sizeof (POD_TYPE) * (end - start)) {
            Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
        }
    }
    template <typename ELEMENT_TYPE>
    Characters::String InputStream<ELEMENT_TYPE>::Ptr::ReadLine () const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        using namespace Characters;
        Require (this->IsSeekable ());
        StringBuilder result;
        while (true) {
            Character c = ReadCharacter ();
            if (c.GetCharacterCode () == '\0') {
                // EOF
                return result.str ();
            }
            result.push_back (c);
            if (c == '\n') {
                return result.str ();
            }
            else if (c == '\r') {
                c = this->ReadCharacter ();
                // if CR is follwed by LF, append that to result too before returning. Otherwise, put the character back
                if (c == '\n') {
                    result.push_back (c);
                }
                else {
                    this->Seek (Whence::eFromCurrent, -1);
                }
                return result.str ();
            }
        }
    }
    template <typename ELEMENT_TYPE>
    Traversal::Iterable<Characters::String> InputStream<ELEMENT_TYPE>::Ptr::ReadLines () const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
 using namespace Characters;
        using namespace Traversal;
        InputStream<Character>::Ptr copyOfStream = *this;
        if (this->IsSeekable ()) [[likely]] {
            return CreateGenerator<String> ([copyOfStream] () -> optional<String> {
                String line = copyOfStream.ReadLine ();
                if (line.empty ()) {
                    return nullopt;
                }
                else {
                    return line;
                }
            });
        }
        else {
            // We may need to 'read ahead' on an unseekable stream, so keep a little extra context to make it happen
            auto readLine = [] (InputStream<Character>::Ptr s, optional<Character> firstChar) -> tuple<String, optional<Character>> {
                StringBuilder result;
                while (true) {
                    Character c = firstChar.value_or (s.ReadCharacter ());
                    firstChar   = nullopt;
                    if (c.GetCharacterCode () == '\0') {
                        return make_tuple (result.str (), nullopt); // EOF
                    }
                    result.push_back (c);
                    if (c == '\n') {
                        return make_tuple (result.str (), nullopt);
                    }
                    else if (c == '\r') {
                        c = s.ReadCharacter ();
                        // if CR is follwed by LF, append that to result too before returning. Otherwise, put the character back
                        if (c == '\n') {
                            result.push_back (c);
                            return make_tuple (result.str (), nullopt);
                        }
                        else {
                            return make_tuple (result.str (), c);
                        }
                    }
                };
            };
            optional<Character> prefixLineChar; // magic so optional<Character> in mutable lambda
            return CreateGenerator<String> ([readLine, copyOfStream, prefixLineChar] () mutable -> optional<String> {
                tuple<String, optional<Character>> lineEtc = readLine (copyOfStream, prefixLineChar);
                if (get<String> (lineEtc).empty ()) {
                    return nullopt;
                }
                else {
                    prefixLineChar = get<optional<Character>> (lineEtc);
                    return get<String> (lineEtc);
                }
            });
        }
    }
    DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
    template <typename ELEMENT_TYPE>
    Characters::String InputStream<ELEMENT_TYPE>::Ptr::ReadAll (size_t upTo) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        using namespace Characters;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"InputStream<Character>::Ptr::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo)};
#endif
        Require (upTo >= 1);
        StringBuilder result;
        size_t        nEltsLeft = upTo;
        while (nEltsLeft > 0) {
            Character  buf[16 * 1024];
            Character* s = std::begin (buf);
            Character* e = std::end (buf);
            if (nEltsLeft < Memory::NEltsOf (buf)) {
                e = s + nEltsLeft;
            }
            size_t n = Read (s, e);
            Assert (0 <= n and n <= nEltsLeft);
            Assert (0 <= n and n <= Memory::NEltsOf (buf));
            if (n == 0) {
                break;
            }
            else {
                Assert (n <= nEltsLeft);
                nEltsLeft -= n;
                result.Append (span{s, n});
            }
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Returning %llu characters", static_cast<unsigned long long> (result.size ()));
#endif
        return result.str ();
    }
    DISABLE_COMPILER_MSC_WARNING_END (6262)
    template <typename ELEMENT_TYPE>
    size_t InputStream<ELEMENT_TYPE>::Ptr::ReadAll (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (intoStart);
        Require ((intoEnd - intoStart) >= 1);
        Require (IsOpen ());
        size_t elementsRead{};
        for (ElementType* readCursor = intoStart; readCursor < intoEnd;) {
            size_t eltsReadThisTime = Read (readCursor, intoEnd);
            Assert (eltsReadThisTime <= static_cast<size_t> (intoEnd - readCursor));
            if (eltsReadThisTime == 0) {
                // irrevocable EOF
                break;
            }
            elementsRead += eltsReadThisTime;
            readCursor += eltsReadThisTime;
        }
        return elementsRead;
    }

    /*
     ********************************************************************************
     *************************** InputStream<ELEMENT_TYPE> **************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto InputStream<ELEMENT_TYPE>::_mkPtr (const _SharedIRep& s) -> Ptr
    {
        return Ptr{s};
    }

}

#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
