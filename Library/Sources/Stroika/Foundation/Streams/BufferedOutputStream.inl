/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_inl_
#define _Stroika_Foundation_Streams_BufferedOutputStream_inl_ 1


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
             *************************** Streams::BufferedOutputStream **********************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            class   BufferedOutputStream<ELEMENT_TYPE>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep {
                static  const   size_t  kMinBufSize_    =   1 * 1024;
                static  const   size_t  kDefaultBufSize =   16 * 1024;
            public:
                Rep_ (const OutputStream<ELEMENT_TYPE>& realOut)
                    : OutputStream<ELEMENT_TYPE>::_IRep ()
                    , fCriticalSection_ ()
                    , fBuffer_ ()
                    , fRealOut_ (realOut)
                    , fAborted_ (false)
                {
                    fBuffer_.reserve (kDefaultBufSize);
                }
                ~Rep_ ()
                {
                    IgnoreExceptionsForCall (Flush ());
                    Ensure (fBuffer_.size () == 0); // advisory - not quite right - could happen if a flush exception was eaten (@todo clean this up)
                }

            public:
                nonvirtual  size_t  GetBufferSize () const
                {
                    using   Execution::make_unique_lock;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    return fBuffer_.capacity ();
                }
                nonvirtual  void    SetBufferSize (size_t bufSize)
                {
                    using   Execution::make_unique_lock;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    bufSize = max (bufSize, kMinBufSize_);
                    if (bufSize < fBuffer_.size ()) {
                        Flush ();
                    }
                    fBuffer_.reserve (bufSize);
                }

            public:
                // Throws away all data about to be written (buffered). Once this is called, its illegal to call Flush or another write
                nonvirtual  void    Abort ()
                {
                    using   Execution::make_unique_lock;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    fAborted_ = true;   // for debug sake track this
                    fBuffer_.clear ();
                }

                //
                virtual bool    IsSeekable () const override
                {
                    return false;      // @todo - COULD be seekable if underlying fRealOut_ was!!!
                }
                virtual SeekOffsetType  GetWriteOffset () const override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual SeekOffsetType  SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual  void    Flush () override
                {
                    using   Execution::make_unique_lock;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    if (fAborted_) {
                        fBuffer_.clear ();
                    }
                    else {
                        if (not fBuffer_.empty ()) {
                            fRealOut_.Write (Containers::Start (fBuffer_), Containers::End (fBuffer_));
                            fBuffer_.clear ();
                        }
                        fRealOut_.Flush ();
                    }
                    Ensure (fBuffer_.empty ());
                }
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                virtual void            Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    Require (start < end);  // for OutputStream<Byte> - this funciton requires non-empty write
                    Require (not fAborted_);
                    using   Execution::make_unique_lock;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                    auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                    /*
                     * Minimize the number of writes at the possible cost of extra copying.
                     *
                     * See if there is room in the buffer, and use it up. Only when no more room do we flush.
                     */
                    size_t  bufSpaceRemaining   =   fBuffer_.capacity () - fBuffer_.size ();
                    size_t  size2WriteRemaining =   end - start;

                    size_t  copy2Buffer =   min (bufSpaceRemaining, size2WriteRemaining);
#if     qDebug
                    size_t  oldCap  =   fBuffer_.capacity ();
#endif
                    fBuffer_.insert (fBuffer_.end (), start, start + copy2Buffer);
                    Assert (oldCap == fBuffer_.capacity ());

                    Assert (size2WriteRemaining >= copy2Buffer);
                    size2WriteRemaining -= copy2Buffer;

                    /*
                     * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
                     */
                    if (fBuffer_.capacity () == fBuffer_.size ()) {
                        Flush ();
                        Assert (fBuffer_.empty ());
                    }
                    Assert (oldCap == fBuffer_.capacity ());

                    // If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
                    // And no point - even if equal to buffer size - since it won't save any writes...
                    if (size2WriteRemaining == 0) {
                        // DONE
                    }
                    else if (size2WriteRemaining < fBuffer_.capacity ()) {
                        fBuffer_.insert (fBuffer_.end (), start + copy2Buffer, end);
                    }
                    else {
                        fRealOut_.Write (start + copy2Buffer, end);
                    }
                }

            private:
                mutable recursive_mutex     fCriticalSection_;
                vector<ELEMENT_TYPE>        fBuffer_;
                OutputStream<ELEMENT_TYPE>  fRealOut_;
                bool                        fAborted_;
            };


            /*
             ********************************************************************************
             *************************** Streams::BufferedOutputStream **********************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            BufferedOutputStream<ELEMENT_TYPE>::BufferedOutputStream (const OutputStream<ELEMENT_TYPE>& realOut)
                : OutputStream<ELEMENT_TYPE> (make_shared<Rep_> (realOut))
            {
            }
            template    <typename ELEMENT_TYPE>
            void    BufferedOutputStream<ELEMENT_TYPE>::Abort ()
            {
                auto rep = this->_GetRep ();
                Rep_* r = dynamic_cast<Rep_*> (rep.get ());
                AssertNotNull (r);
                r->Abort ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BufferedOutputStream_inl_*/
