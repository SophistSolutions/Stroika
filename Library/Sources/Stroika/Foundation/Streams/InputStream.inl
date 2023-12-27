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

namespace Stroika::Foundation::Streams::InputStream {

    /*
     ********************************************************************************
     *********************** InputStream::IRep<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    optional<size_t> InputStream::IRep<ELEMENT_TYPE>::_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (ElementType* intoStart,
                                                                                                                       ElementType* intoEnd,
                                                                                                                       size_t elementsRemaining)
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        if (intoStart == nullptr) {
            return elementsRemaining;
        }
        else {
            return elementsRemaining == 0 ? 0 : Read (span{intoStart, intoEnd}).size (); // safe to call beacuse this cannot block - there are elements available
        }
    }

    /*
     ********************************************************************************
     *********************** InputStream::Ptr<ELEMENT_TYPE> *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline InputStream::Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep)
        : inherited{rep}
    {
    }
    template <typename ELEMENT_TYPE>
    inline InputStream::Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline void InputStream::Ptr<ELEMENT_TYPE>::Close () const
    {
        Require (IsOpen ());
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        GetRepRWRef ().CloseRead ();
    }
    template <typename ELEMENT_TYPE>
    inline void InputStream::Ptr<ELEMENT_TYPE>::Close (bool reset)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        GetRepRWRef ().CloseRead ();
        if (reset) {
            this->reset ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline bool InputStream::Ptr<ELEMENT_TYPE>::IsOpen () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return GetRepConstRef ().IsOpenRead ();
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::GetSharedRep () const -> shared_ptr<IRep<ELEMENT_TYPE>>
    {
        return Debug::UncheckedDynamicPointerCast<IRep<ELEMENT_TYPE>> (inherited::GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::GetRepConstRef () const -> const IRep<ELEMENT_TYPE>&
    {
        return Debug::UncheckedDynamicCast<const IRep<ELEMENT_TYPE>&> (inherited::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::GetRepRWRef () const -> IRep<ELEMENT_TYPE>&
    {
        return Debug::UncheckedDynamicCast<IRep<ELEMENT_TYPE>&> (inherited::GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream::Ptr<ELEMENT_TYPE>::GetOffset () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        return GetRepConstRef ().GetReadOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream::Ptr<ELEMENT_TYPE>::Seek (SeekOffsetType offset) const
    {
        Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        Require (this->IsSeekable ());
        return GetRepRWRef ().SeekRead (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType InputStream::Ptr<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        Require (this->IsSeekable ());
        return GetRepRWRef ().SeekRead (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::Read () const -> optional<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        ElementType b{};
        return (GetRepRWRef ().Read (span{&b, 1}).size () == 0) ? optional<ElementType>{} : b;
    }
    template <typename ELEMENT_TYPE>
    inline size_t InputStream::Ptr<ELEMENT_TYPE>::Read (ElementType* intoStart, ElementType* intoEnd) const
    {
        return Read (span{intoStart, intoEnd}).size ();
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::Read (span<ElementType> intoBuffer) const -> span<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        Require (not intoBuffer.empty ());
        return GetRepRWRef ().Read (intoBuffer);
    }
    template <typename ELEMENT_TYPE>
    auto InputStream::Ptr<ELEMENT_TYPE>::Peek () const -> optional<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read ();
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    size_t InputStream::Ptr<ELEMENT_TYPE>::Peek (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read (intoStart, intoEnd);
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    inline bool InputStream::Ptr<ELEMENT_TYPE>::IsAtEOF () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        return not Peek ().has_value ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        return GetRepRWRef ().ReadNonBlocking (nullptr, nullptr);
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (intoStart);
        Require ((intoEnd - intoStart) >= 1);
        Require (IsOpen ());
        return GetRepRWRef ().ReadNonBlocking (intoStart, intoEnd);
    }
    template <typename ELEMENT_TYPE>
    inline Characters::Character InputStream::Ptr<ELEMENT_TYPE>::ReadCharacter () const
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
    POD_TYPE InputStream::Ptr<ELEMENT_TYPE>::ReadRaw () const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
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
    inline void InputStream::Ptr<ELEMENT_TYPE>::ReadRaw (POD_TYPE* start, POD_TYPE* end) const
        requires (is_same_v<ELEMENT_TYPE, byte>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        static_assert (is_pod_v<POD_TYPE>);
        size_t n{ReadAll (reinterpret_cast<byte*> (start), reinterpret_cast<byte*> (end))};
        if (n != sizeof (POD_TYPE) * (end - start)) {
            Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
        }
    }
    template <typename ELEMENT_TYPE>
    Characters::String InputStream::Ptr<ELEMENT_TYPE>::ReadLine () const
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
    Traversal::Iterable<Characters::String> InputStream::Ptr<ELEMENT_TYPE>::ReadLines () const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        using namespace Characters;
        using namespace Traversal;
        InputStream::Ptr<Character> copyOfStream = *this;
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
            auto readLine = [] (InputStream::Ptr<Character> s, optional<Character> firstChar) -> tuple<String, optional<Character>> {
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
    Characters::String InputStream::Ptr<ELEMENT_TYPE>::ReadAll (size_t upTo) const
        requires (is_same_v<ELEMENT_TYPE, Characters::Character>)
    {
        using namespace Characters;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"InputStream::Ptr<Character>::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo)};
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
    size_t InputStream::Ptr<ELEMENT_TYPE>::ReadAll (ElementType* intoStart, ElementType* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
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

}

#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
