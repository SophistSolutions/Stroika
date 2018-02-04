/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_
#define _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Characters/String_Constant.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Debug/Sanitizer.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/StringException.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                /*
                 ********************************************************************************
                 **************** InputStreamFromStdIStream<ELEMENT_TYPE>::Rep_ *****************
                 ********************************************************************************
                 */
                template <typename ELEMENT_TYPE, typename TRAITS>
#if qCompiler_SanitizerVPtrTemplateTypeEraseureBug
				Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
					class
                        InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep,
                                                                                private Debug::AssertExternallySynchronizedLock {
                private:
                    using IStreamType = typename TRAITS::IStreamType;

                private:
                    static SeekableFlag DefaultSeekable_ (IStreamType& originalStream)
                    {
                        // SB something like lseek(fd, CURRENT, 0) not an error, but that doesn't work wtih seekg() on
                        // MSVC2k13. Not sure of a good portable, and yet non-destructive way...
                        return eSeekable;
                    }

                private:
                    bool fOpen_{true};

                public:
                    Rep_ (IStreamType& originalStream)
                        : Rep_ (originalStream, DefaultSeekable_ (originalStream))
                    {
                    }
                    Rep_ (IStreamType& originalStream, SeekableFlag seekable)
                        : fOriginalStream_ (originalStream)
                        , fSeekable_ (seekable)
                    {
                    }

                protected:
                    virtual bool IsSeekable () const override
                    {
                        return fSeekable_ == eSeekable;
                    }
                    virtual void CloseRead () override
                    {
                        Require (IsOpenRead ());
                        fOpen_ = false;
                        Ensure (not IsOpenRead ());
                    }
                    virtual bool IsOpenRead () const override
                    {
                        return fOpen_;
                    }
                    virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        RequireNotNull (intoStart);
                        RequireNotNull (intoEnd);
                        Require (intoStart < intoEnd);

                        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                        Require (IsOpenRead ());
                        if (fOriginalStream_.eof ()) {
                            return 0;
                        }
                        size_t maxToRead        = intoEnd - intoStart;
                        using StreamElementType = typename IStreamType::char_type;
                        fOriginalStream_.read (reinterpret_cast<StreamElementType*> (intoStart), maxToRead);
                        size_t n = static_cast<size_t> (fOriginalStream_.gcount ()); // cast safe cuz amount asked to read was also size_t

                        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
                        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
                            Execution::Throw (Execution::StringException (Characters::String_Constant{L"Failed to read from istream"}));
                        }
                        return n;
                    }
                    virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        Require (IsOpenRead ());
                        std::streamsize sz = fOriginalStream_.rdbuf ()->in_avail ();
                        // http://en.cppreference.com/w/cpp/io/basic_streambuf/in_avail
                        if (sz == 0) {
                            return {};
                        }
                        else if (sz == -1) {
                            sz = 0;
                        }
                        return this->_ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, static_cast<size_t> (sz));
                    }
                    virtual SeekOffsetType GetReadOffset () const override
                    {
                        // instead of tellg () - avoids issue with EOF where fail bit set???
                        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                        Require (IsOpenRead ());
                        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
                    }
                    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                        Require (IsOpenRead ());
                        switch (whence) {
                            case Whence::eFromStart:
                                fOriginalStream_.seekg (offset, ios::beg);
                                break;
                            case Whence::eFromCurrent:
                                fOriginalStream_.seekg (offset, ios::cur);
                                break;
                            case Whence::eFromEnd:
                                fOriginalStream_.seekg (offset, ios::end);
                                break;
                        }
                        // instead of tellg () - avoids issue with EOF where fail bit set???
                        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
                    }

                private:
                    IStreamType& fOriginalStream_;
                    SeekableFlag fSeekable_;
                };

                /*
                 ********************************************************************************
                 ********************* InputStreamFromStdIStream<ELEMENT_TYPE> ******************
                 ********************************************************************************
                 */
                template <typename ELEMENT_TYPE, typename TRAITS>
                inline auto InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::New (IStreamType& originalStream) -> Ptr
                {
                    return make_shared<Rep_> (originalStream);
                }
                template <typename ELEMENT_TYPE, typename TRAITS>
                inline auto InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::New (IStreamType& originalStream, SeekableFlag seekable) -> Ptr
                {
                    return make_shared<Rep_> (originalStream, seekable);
                }
                template <typename ELEMENT_TYPE, typename TRAITS>
                inline auto InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::New (Execution::InternallySyncrhonized internallySyncrhonized, IStreamType& originalStream) -> Ptr
                {
                    switch (internallySyncrhonized) {
                        case Execution::eInternallySynchronized:
                            return InternalSyncRep_::New (originalStream);
                        case Execution::eNotKnwonInternallySynchronized:
                            return New (originalStream);
                        default:
                            RequireNotReached ();
                            return nullptr;
                    }
                }
                template <typename ELEMENT_TYPE, typename TRAITS>
                inline auto InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::New (Execution::InternallySyncrhonized internallySyncrhonized, IStreamType& originalStream, SeekableFlag seekable) -> Ptr
                {
                    switch (internallySyncrhonized) {
                        case Execution::eInternallySynchronized:
                            return InternalSyncRep_::New (originalStream, seekable);
                        case Execution::eNotKnwonInternallySynchronized:
                            return New (originalStream, seekable);
                        default:
                            RequireNotReached ();
                            return nullptr;
                    }
                }

                /*
                 ********************************************************************************
                 *********** InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Ptr ***************
                 ********************************************************************************
                 */
                template <typename ELEMENT_TYPE, typename TRAITS>
                inline InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                    : inherited (from)
                {
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_*/
