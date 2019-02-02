/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_
#define _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Characters/String_Constant.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/StringException.h"

namespace Stroika::Foundation::Streams::iostream {

    /*
     ********************************************************************************
     *************** OutputStreamFromStdOStream<ELEMENT_TYPE>::Rep_ *****************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename TRAITS>
    class OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
    private:
        using OStreamType = typename TRAITS::OStreamType;

    private:
        bool fOpen_{true};

    public:
        Rep_ (OStreamType& originalStream)
            : fOriginalStream_ (originalStream)
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            return true;
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            fOpen_ = false;
            Ensure (not IsOpenWrite ());
        }
        virtual bool IsOpenWrite () const override
        {
            return fOpen_;
        }
        virtual SeekOffsetType GetWriteOffset () const override
        {
            // instead of tellg () - avoids issue with EOF where fail bit set???
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpenWrite ());
            return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
        }
        virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpenWrite ());
            switch (whence) {
                case Whence::eFromStart:
                    fOriginalStream_.seekp (offset, ios::beg);
                    break;
                case Whence::eFromCurrent:
                    fOriginalStream_.seekp (offset, ios::cur);
                    break;
                case Whence::eFromEnd:
                    fOriginalStream_.seekp (offset, ios::end);
                    break;
            }
            return fOriginalStream_.tellp ();
        }
        virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
        {
            Require (start != nullptr or start == end);
            Require (end != nullptr or start == end);
            Require (IsOpenWrite ());

            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};

            using StreamElementType = typename OStreamType::char_type;
            fOriginalStream_.write (reinterpret_cast<const StreamElementType*> (start), end - start);
            if (fOriginalStream_.fail ())
                [[UNLIKELY_ATTR]]
                {
                    using namespace Characters;
                    Execution::Throw (Execution::Exception (L"Failed to write from ostream"sv));
                }
        }
        virtual void Flush () override
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpenWrite ());
            fOriginalStream_.flush ();
            if (fOriginalStream_.fail ())
                [[UNLIKELY_ATTR]]
                {
                    using namespace Characters;
                    Execution::Throw (Execution::Exception (L"Failed to flush ostream"sv));
                }
        }

    private:
        OStreamType& fOriginalStream_;
    };

    /*
     ********************************************************************************
     ********************* OutputStreamFromStdOStream<ELEMENT_TYPE> *****************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename TRAITS>
    inline auto OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::New (OStreamType& originalStream) -> Ptr
    {
        return make_shared<Rep_> (originalStream);
    }
    template <typename ELEMENT_TYPE, typename TRAITS>
    inline auto OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::New (Execution::InternallySynchronized internallySynchronized, OStreamType& originalStream) -> Ptr
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternalSyncRep_::New (originalStream);
            case Execution::eNotKnownInternallySynchronized:
                return New (originalStream);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

    /*
     ********************************************************************************
     *********** OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Ptr **************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename TRAITS>
    inline OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Ptr::Ptr (const shared_ptr<Rep_>& from)
        : inherited (from)
    {
    }

}

#endif /*_Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_*/
