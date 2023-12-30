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
#include "../Execution/Finally.h"

#include "EOFException.h"
#include "EWouldBlock.h"

namespace Stroika::Foundation::Streams::InputStream {

    /*
     ********************************************************************************
     *********************** InputStream::IRep<ELEMENT_TYPE> ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    optional<size_t> InputStream::IRep<ELEMENT_TYPE>::AvailableToRead ()
    {
        Require (this->IsSeekable ()); // subclassers must override if not seekable
                                       // not sure why not compiling
        SeekOffsetType offset = GetReadOffset ();
        // note this impl assumes seek won't fail - perhaps should catch internally rather than std::terminate?
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&, this] () noexcept { this->SeekRead (Whence::eFromStart, offset); });
        ElementType elts[1]; // typically not useful to know if more than one available, and typically more costly to read extras we will toss out
        try {
            optional<span<ELEMENT_TYPE>> o = this->Read (span{elts}, NoDataAvailableHandling::eThrowIfWouldBlock);
            return o ? o->size () : optional<size_t>{};
        }
        catch (const EWouldBlock&) {
            return nullopt;
        }
        catch (...) {
            Execution::ReThrow ();
        }
    }
#if 0
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
            //tmphack - code going away
            return elementsRemaining == 0 ? 0 : Read (span{intoStart, intoEnd}, NoDataAvailableHandling::eDefault)->size (); // safe to call beacuse this cannot block - there are elements available
        }
    }
#endif

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
    inline auto InputStream::Ptr<ELEMENT_TYPE>::AvailableToRead () const -> optional<size_t>
    {
        return GetRepRWRef ().AvailableToRead ();
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::Read (NoDataAvailableHandling blockFlag) const -> optional<ElementType>
    {
        ELEMENT_TYPE b; // intentionally uninitialized in case POD-type, filled in by Read or not used
        return this->Read (span{&b, 1}, blockFlag).size () == 0 ? optional<ElementType>{} : b;
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) const -> span<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        Require (not intoBuffer.empty ());
        if (auto o = GetRepRWRef ().Read (intoBuffer, blockFlag)) [[likely]] {
            return *o;
        }
        else {
            Execution::Throw (EWouldBlock::kThe);
        }
    }
    template <typename ELEMENT_TYPE>
    inline auto InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking (span<ElementType> intoBuffer) const -> optional<span<ElementType>>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ()); // note - its OK for Write() side of input stream to be closed
        Require (not intoBuffer.empty ());
        return GetRepRWRef ().Read (intoBuffer, NoDataAvailableHandling::eThrowIfWouldBlock);
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
    auto InputStream::Ptr<ELEMENT_TYPE>::Peek (span<ElementType> intoBuffer) const -> span<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read (intoBuffer);
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
        return AvailableToRead ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
    {
        try {
            return this->Read (span{intoStart, intoEnd}, NoDataAvailableHandling::eThrowIfWouldBlock);
        }
        catch (const EWouldBlock& e) {
            return nullopt;
        }
        catch (...) {
            Execution::ReThrow ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline Characters::Character InputStream::Ptr<ELEMENT_TYPE>::ReadCharacter () const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
    {
        Characters::Character c;
        if (Read (span{&c, 1}).size () == 1) {
            return c;
        }
        return '\0';
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    POD_TYPE InputStream::Ptr<ELEMENT_TYPE>::ReadRaw () const
        requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
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
    inline void InputStream::Ptr<ELEMENT_TYPE>::ReadRaw (span<POD_TYPE> intoBuffer) const
        requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        size_t n{ReadAll (reinterpret_cast<byte*> (intoBuffer.data ()), reinterpret_cast<byte*> (intoBuffer.data () + intoBuffer.size ()))};
        if (n != intoBuffer.size_bytes ()) {
            Execution::Throw ((n == 0) ? EOFException::kThe : EOFException{true});
        }
    }
    template <typename ELEMENT_TYPE>
    Characters::String InputStream::Ptr<ELEMENT_TYPE>::ReadLine () const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
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
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
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
                        // if CR is followed by LF, append that to result too before returning. Otherwise, put the character back
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
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
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
            size_t n = Read (span{s, e}).size ();
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
    template <typename ELEMENT_TYPE2, size_t EXTENT2_T>
    auto InputStream::Ptr<ELEMENT_TYPE>::ReadAll (span<ELEMENT_TYPE2, EXTENT2_T> intoBuffer) const -> span<ElementType>
        requires (same_as<ELEMENT_TYPE, ELEMENT_TYPE2>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (not intoBuffer.empty ());
        Require (IsOpen ());
        size_t       elementsRead{};
        ElementType* intoEnd = intoBuffer.data () + intoBuffer.size ();
        for (ElementType* readCursor = intoBuffer.data (); readCursor < intoEnd;) {
            size_t eltsReadThisTime = Read (span{readCursor, intoEnd}).size ();
            if (eltsReadThisTime == 0) {
                // irrevocable EOF
                break;
            }
            elementsRead += eltsReadThisTime;
            readCursor += eltsReadThisTime;
        }
        return intoBuffer.subspan (0, elementsRead);
    }

}

#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
