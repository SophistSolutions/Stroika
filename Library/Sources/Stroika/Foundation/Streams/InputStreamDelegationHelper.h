/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStreamDelegationHelper_h_
#define _Stroika_Foundation_Streams_InputStreamDelegationHelper_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Streams {

    /**
     *  This does nothing useful, in and of itself: it just takes an input stream and wraps it into another input stream.
     *  But it provides a useful helper to reduce typing when you want to 'wrap'/'hook' an existing input stream and just react
     *  to a few things (like progress/reads).
     */
    template <typename ELEMENT_TYPE>
    struct InputStreamDelegationHelper : public InputStream::IRep<ELEMENT_TYPE> {
        InputStreamDelegationHelper (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
            : fRealIn{realIn}
        {
            RequireNotNull (realIn); // no point in a wrapping a null pointer!
        }
        // Stream::IRep
        virtual bool IsSeekable () const override
        {
            return fRealIn.IsSeekable ();
        }
        // InputStream::IRep
        virtual void CloseRead () override
        {
            fRealIn.Close ();
        }
        virtual bool IsOpenRead () const override
        {
            return fRealIn.IsOpen ();
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            return fRealIn.GetOffset ();
        }
        virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
        {
            return fRealIn.Seek (whence, offset);
        }
        virtual optional<size_t> AvailableToRead () override
        {
            return fRealIn.AvailableToRead ();
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return fRealIn.RemainingLength ();
        }
        virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            return fRealIn.Read (intoBuffer, blockFlag);
        }
        typename InputStream::Ptr<ELEMENT_TYPE> fRealIn;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputStreamDelegationHelper.inl"

#endif /*_Stroika_Foundation_Streams_InputStreamDelegationHelper_h_*/
