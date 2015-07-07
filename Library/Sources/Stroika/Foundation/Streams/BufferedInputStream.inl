/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_inl_
#define _Stroika_Foundation_Streams_BufferedInputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ******************** Streams::BufferedInputStream::Rep_ ************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            class   BufferedInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep {
            public:
                Rep_ (const InputStream<ELEMENT_TYPE>& realIn)
                    : InputStream<ELEMENT_TYPE>::_IRep ()
                    , fCriticalSection_ ()
                    , fRealIn_ (realIn)
                {
                }
                virtual bool    IsSeekable () const override
                {
                    return false;      // @todo - COULD be seekable if underlying fRealIn_ was!!!
                }
                virtual SeekOffsetType  GetReadOffset () const override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual size_t  Read (SeekOffsetType* offset, ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    return fRealIn_.Read (intoStart, intoEnd);
                }

            private:
                mutable mutex               fCriticalSection_;
                InputStream<ELEMENT_TYPE>   fRealIn_;
            };


            /*
             ********************************************************************************
             ************************ Streams::BinaryInputStream ****************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            BufferedInputStream<ELEMENT_TYPE>::BufferedInputStream (const InputStream<ELEMENT_TYPE>& realIn)
                : InputStream<ELEMENT_TYPE> (make_shared<Rep_> (realIn))
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BufferedInputStream_inl_*/
