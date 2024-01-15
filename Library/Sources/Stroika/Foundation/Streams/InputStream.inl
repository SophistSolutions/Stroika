/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
        SeekOffsetType offset = GetReadOffset ();
        // note this impl assumes seek won't fail - perhaps should catch internally rather than std::terminate?
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&, this] () noexcept { this->SeekRead (Whence::eFromStart, offset); });
        ElementType elts[1]; // typically not useful to know if more than one available, and typically more costly to read extras we will toss out
        try {
            optional<span<ELEMENT_TYPE>> o = this->Read (span{elts}, NoDataAvailableHandling::eDontBlock);
            return o ? o->size () : optional<size_t>{};
        }
        catch (const EWouldBlock&) {
            return nullopt;
        }
        catch (...) {
            Execution::ReThrow ();
        }
    }
    template <typename ELEMENT_TYPE>
    optional<SeekOffsetType> InputStream::IRep<ELEMENT_TYPE>::RemainingLength ()
    {
        return nullopt;
    }
    template <typename ELEMENT_TYPE>
    auto InputStream::IRep<ELEMENT_TYPE>::SeekRead ([[maybe_unused]] Whence whence, [[maybe_unused]] SignedSeekOffsetType offset) -> SeekOffsetType
    {
        AssertNotImplemented (); // never call if not seekable and must override if IsSeekable
        return 0;
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
    template <typename ASSTREAMABLE>
    inline InputStream::Ptr<ELEMENT_TYPE>::Ptr (ASSTREAMABLE&& src)
        requires
#if qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
        same_as<byte, ELEMENT_TYPE> and
#endif
        requires (ASSTREAMABLE) { src.template As<Ptr<ELEMENT_TYPE>> (); }
        : inherited{src.template As<Ptr<ELEMENT_TYPE>> ()}
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
    inline auto InputStream::Ptr<ELEMENT_TYPE>::RemainingLength () const -> optional<SeekOffsetType>
    {
        return GetRepRWRef ().RemainingLength ();
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
        return GetRepRWRef ().Read (intoBuffer, NoDataAvailableHandling::eDontBlock);
    }
    template <typename ELEMENT_TYPE>
    auto InputStream::Ptr<ELEMENT_TYPE>::Peek (NoDataAvailableHandling blockFlag) const -> optional<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read (blockFlag);
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    auto InputStream::Ptr<ELEMENT_TYPE>::Peek (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) const -> span<ElementType>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (this->IsSeekable ());
        Require (IsOpen ());
        SeekOffsetType saved  = GetOffset ();
        auto           result = this->Read (intoBuffer, blockFlag);
        this->Seek (saved);
        return result;
    }
    template <typename ELEMENT_TYPE>
    bool InputStream::Ptr<ELEMENT_TYPE>::IsAtEOF () const
    {
        return not Peek (NoDataAvailableHandling::eBlockIfNoDataAvailable).has_value ();
    }
    template <typename ELEMENT_TYPE>
    optional<bool> InputStream::Ptr<ELEMENT_TYPE>::IsAtEOF (NoDataAvailableHandling blockFlag) const
    {
        if (blockFlag == eBlockIfNoDataAvailable) {
            return IsAtEOF ();
        }
        ELEMENT_TYPE ignored{};
        Require (this->IsSeekable ());
        SeekOffsetType saved = GetOffset ();
        auto           o     = this->ReadNonBlocking (span{&ignored, 1});
        this->Seek (saved);
        return not o.has_value ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking () const
    {
        // DEPRECATED
        return AvailableToRead ();
    }
    template <typename ELEMENT_TYPE>
    inline optional<size_t> InputStream::Ptr<ELEMENT_TYPE>::ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) const
    {
        // DEPRECATED
        try {
            return this->Read (span{intoStart, intoEnd}, NoDataAvailableHandling::eDontBlock).size ();
        }
        catch (const EWouldBlock& e) {
            return nullopt;
        }
        catch (...) {
            Execution::ReThrow ();
        }
    }
    template <typename ELEMENT_TYPE>
    inline Characters::Character InputStream::Ptr<ELEMENT_TYPE>::ReadCharacter (NoDataAvailableHandling blockFlag) const
        requires (same_as<ELEMENT_TYPE, Characters::Character>)
    {
        Characters::Character c;
        if (Read (span{&c, 1}, blockFlag).size () == 1) [[likely]] {
            return c;
        }
        return '\0';
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    inline POD_TYPE InputStream::Ptr<ELEMENT_TYPE>::ReadRaw () const
        requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        POD_TYPE tmp; // intentionally don't zero-initialize
        return this->ReadRaw (span{&tmp, 1})[0];
    }
    template <typename ELEMENT_TYPE>
    template <typename POD_TYPE>
    span<POD_TYPE> InputStream::Ptr<ELEMENT_TYPE>::ReadRaw (span<POD_TYPE> intoBuffer) const
        requires (same_as<ELEMENT_TYPE, byte> and is_standard_layout_v<POD_TYPE>)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        Require (IsOpen ());
        Require (intoBuffer.size () == 1 or this->IsSeekable ());
        auto   byteSpan = as_writable_bytes (intoBuffer);
        size_t n{Read (byteSpan)};
        if (n < sizeof (POD_TYPE)) {
            // must keep (blocking) reading til we have one POD_TYPE, and if we come up short, must throw EOF
            while (n < sizeof (POD_TYPE)) {
                auto ni = this->Read (byteSpan.subspan (n, sizeof (POD_TYPE) - n)).size ();
                if (ni == 0) {
                    Execution::Throw (EOFException{true});
                }
                n += ni;
            }
            Assert (n == sizeof (POD_TYPE));
            return intoBuffer.subspan (0, 1);
        }
        else {
            // we win, just return the right span size
            if (n % sizeof (POD_TYPE) != 0) {
                // Read even number of objects and adjust seek pointer
                this->Seek (Whence::eFromCurrent, -static_cast<SignedSeekOffsetType> (n % sizeof (POD_TYPE)));
            }
            // we win, just return the right span size
            size_t nObjectsRead = n / sizeof (POD_TYPE);
            Assert (1 <= nObjectsRead and nObjectsRead <= intoBuffer.size ());
            return intoBuffer.subspan (0, nObjectsRead);
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
        /*
         *  Conceptually very simple - the same API as READ - so we can just forward. BUT - its not quite the same.
         *  Read is ALLOWED to return less than requested, without prejudice about whether more is available.
         *  This API is NOT.
         *
         *  So keep reading will we know we got everything.
         * 
         *  Note - its because of this, and to avoid potentiallly needing to unread, that this API doesn't support non-blocking ReadAll (could do with seek).
         */
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
