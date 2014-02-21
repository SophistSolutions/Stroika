/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedPtr_h_
#define _Stroika_Foundation_Memory_SharedPtr_h_ 1

#include    "../StroikaPreComp.h"

#include	<atomic>

#include    "../Configuration/Common.h"







/*
 *      TODO:
 *
 *      @todo   CLEANUP AND CONSIDER RE-INCLUSION INTO STROIKA!!!
 *
 *              Test if performs better.
 *              (because of blcok allocation of count and not supporitng weak_ptr)
 *
 *              See if I can transparently add (optional traits) locker, to make it threadsafe.
 *              (at least copying envelope safe)
 *
 *          (o)     CLEAR DOCS!!! - once we have stuff stable...
 *
 *          (o)      supported && move operations!
 *
 *          (o)     CAREFULLY writeup differences between this class and shared_ptr<>
 *                  +   I DONT BELIEVE weak_ptr<T> makes sense, and seems likely to generate bugs in multithreaded
 *                      applications. Maybe I'm missing something. Ask around a bit...
 *                      FOR THE MOST PART.
 *
 *                      There are specific (rare) cases where weak_ptr IS important, and I wnat to find (TODO)
 *                      SOME way to implemnet athat (e.g. PHRDB:: shared DB stuff).
 *
 *          (o)     BETTER DOCUMENT - USE ShaerdPtrBase stuff in other module
 *
 *          (o)     And document the issue wtih Also - Thread::Cretae() issue - but htat has a workaround...
 *
 *
 */








/*
 *  TODO:
 *          (o)     Cleanup documentation, especially about the purpose/point, and how to use.
 *
 */



/*

<<<OBSOLETE>>>

 *          It is principally to solve THIS problem - that SharedPtrBase/UsesSharedPtrBase still exists for. There is as common base class
 *          (so you can always find the shared reference count).
 *
 *          For example:
 *
                void    PHRDBBaseRep::DoSave (ProgressStatusCallback* progressCallback)
                {
                    wstring soapBody;
                    {
                        BLOBs::BLOB phrBLOB;
                        {
                            stringstream    tmpOut;
                            ImportExport::Export (PHRDB::DB (SharedPtr<IDBRep> (SharedPtr<IDBRep>::eUsesSharedPtrBase, this)), MimeTypes::Predefined::HealthFramePHR2_CT (), ImportExport::ExportOptions (), tmpOut, ProgressSubTask (progressCallback, 0.0f, .5f));
                            phrBLOB = StreamUtils::ReadStreamAsBLOB (tmpOut);
                        }
                        CreateSOAPEnvelope_PUTPHR (&soapBody, NarrowSDKStringToWide (Cryptography::EncodeBase64 (phrBLOB)), fUserName, fPassword, fOverridePHRName);
                    }
                    const wstring   kSoapAction =    wstring (kHealthFrameWorksURLNamespaceURI_V10) + L"UploadPHR";
                    (void)HTTPSupport::SimpleSOAPSendReceive (L"POST", fUploadURL, kSoapAction, soapBody, CalcSendBestTimeout2Use (soapBody), sOurUserAgent, ProgressSubTask (progressCallback, 0.5f, 1.0f));
                }

 *
 *          ALTERNATIVES:
 *
 *          (A)     DELETER argument to SharedPtr<>. Basically - we would have a TEMPLATD ARG to SharedPtr - the default of
 *                  which would take up zero size (calling regular operaotr delete on T arg to SharedPtr).
 *
 *                  That has the advantage that if someone delcared SharedPtr<T,T::SPECIALDELETER> and tried to assign something of
 *                  type SharedPtr<T> - that would not compile (in contrast with existing practice where SharedPtr<IDBRep> (this) would compile
 *                  and just crash badly).
 *
 *          (B)     DELETER function pointer - in EVERY SharedPtr<> - passed in for orignal CTOR call. Stores extra pointer with every underlying SharedPtrREP.
 *                  But then have specail one that does NOTHING. And then
 *
 *          (C)     ScopedNODELETER
 *                  This would be a specail class (or parameter to SharedPtr<>) which initializes the sharedptr with a specail count
 *                  with - perhaps - the value 1, and then asserts at the end - when the envoleope is desroyed, that hte value is still 1,
 *                  and THEN delete the COUNTER (cuz 1 was magically like zero - means all refernces went away).
 *
 *                  It would be a BUG if the originating ScopeNoDeleter/SharedPtr didn't left behind any references. All the cases I have currently
 *                  that would be fine for - and it works pretty well - but could be create buggy code if called code with this special pointer
 *                  stored those poitners. Really just doesnt work for those cases. Maybe not too bad.
 *
 *          (D)     Partial specialization with SharedPtrBase.
 *                  If I had faith in this always working - it would probably be the best choice. I guess I should try it...
 *                  Basically - if you have a class you want to pull this trick on - then inheirt from a specail base class, and the rest
 *                  happens fully automatically.
 *                  [attractive enuf - IF I can make the partial-specailation stuff work well enuf - then its perhaps the best way to go]
 *
 *                  This MAYBE as simple as template specialize of
 *                                  template    <typename T>
                                        inline  SharedPtr<T>::~SharedPtr ()
                            and
                                op=
                            (or bette ryet - but the delete part in its own template and template partial specialize that

 *
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            template    <typename   T>
            class   enable_shared_from_this;


            namespace   Private {
                namespace   SharedPtr_Default_Traits_Helpers_ {
                    /*
                     * Note - though we COULD use a smaller reference count type (e.g. uint32_t - for 64bit machines) -
                     * if we use one smaller than sizeof(void*) we cannot use BlockAllocation<> code -
                     * which currently requires sizeof (T) >= sizeof (void*)
                     */
                    typedef atomic<size_t>  ReferenceCountType_;
                    // This is used to wrap/combine the shared pointer with the counter.
                    template    <typename   T>
                    class   Envelope_;
                }
                namespace   enable_shared_from_this_Traits_Helpers_ {
                    // 32 bits of counter should be enough for any reasonable application
                    typedef uint32_t    ReferenceCountType_;
                    // This is used to wrap/combine the shared pointer with the counter.
                    template    <typename   T>
                    class   Envelope_;
                }
            }




            /*
             * Default 'TRAITS' object controlling how SharedPtr<T,T_TRAITS> works. This typically will
             * not be used directly, but just part of using @SharedPtr<T>
             */
            template    <typename   T>
            struct  SharedPtr_Default_Traits {
                typedef Private::SharedPtr_Default_Traits_Helpers_::ReferenceCountType_             ReferenceCountType;
                typedef Private::SharedPtr_Default_Traits_Helpers_::Envelope_<T>                    Envelope;
            };



            /*
             * SharedPtrFromThis_Traits is the TRAITS object to use with SharedPtr, and T must already
             * inherit from SharedPtrBase.
             *
             *  Example usage:
             *
             *      class   VeryFancyObj : SharedPtrBase {
             *      };
             *
             *      typedef SharedPtr<VeryFancyObj,SharedPtrFromThis_TraitsVeryFancyObj>>   VeryFancySmartPointer;
             *
             *      THEN - VeryFancySmartPointer will work like a regular smart pointer - EXCEPT THAT IN ADDITION,
             *      you can ALWAYS safely create a VeryFancySmartPointer from an already existing VeryFancyObj -
             *      just by wrapping/constructing the smart pointer (because the reference count is already in
             *      the base wrapped data type).
             */
            template    <typename   T>
            struct  SharedPtrFromThis_Traits {
                typedef Private::enable_shared_from_this_Traits_Helpers_::ReferenceCountType_   ReferenceCountType;
                typedef Private::enable_shared_from_this_Traits_Helpers_::Envelope_<T>          Envelope;
            };




            /*
            @CLASS:         SharedPtr<T,T_TRAITS>
            @DESCRIPTION:
                    <p>This class is for keeping track of a data structure with reference counts,
                and disposing of that structure when the reference count drops to zero.
                Copying one of these Shared<T> just increments the referce count,
                and destroying/overwriting one decrements it.
                </p>

                    <p>You can have a ptr having a nullptr value, and it can be copied.
                (Implementation detail - the reference count itself is NEVER nil except upon
                failure of alloction of memory in ctor and then only valid op on class is
                destruction). You can access the value with GetPointer () but this is not
                advised - only if it may be legitimately nullptr do you want to do this.
                Generaly just use ptr-> to access the data, and this will do the
                RequireNotNull (POINTER) for you.
                </p>

                    <p>This class can be enourmously useful in implementing letter/envelope -
                type data structures - see String, or Shapes, for examples.
                </p>

                    <p>Example Usage</p>

                <code>
                    {
                        SharedPtr<int>  p (new int ());
                        *p = 3;
                        // 'when 'p' goes out of scope - the int will be automatically deleted
                    }
                </code>

                    <p>SharedPtr<T> is a simple utility class - very much akin to the C++11 class
                std::shared_ptr<T>. SharedPtr<T> contains the following basic differences:

                    <li>There is no std::weak_ptr - or at least if there is - we must document it clearly
                        how/why via extra sharedPTR tmeplate arg(to be worked out)</li>
                    <li>There is an extra template T_TRAITS that allows for solving special problems that
                        come up with shared_ptr<> - namely recovering the
                        'shared' version of 'T' when only given a plain copy of 'T'
                    </li>

                    Otherwise, the intention is that they should operate very similarly, and SharedPtr<T>
                    should work with most classes that expect shared_ptr<T> (so long
                    as they are templated, and not looking for the particular type name 'shared_ptr').

                    <p>TODO: CHECK EXACT API DIFFERENCES WITH shared_ptr - BUT - they should be reasonably small -
                    neglecting the weak_ptr stuff.</p>

                    <p>See also @SharedPtrBase module for how to do much FANCIER SharedPtr<> usage</p>

            */
            template    <typename   T, typename T_TRAITS = SharedPtr_Default_Traits<T>>
            class   SharedPtr {
            public:
                SharedPtr ();
                explicit SharedPtr (T* from);
                explicit SharedPtr (const typename T_TRAITS::Envelope& from);
                SharedPtr (const SharedPtr<T, T_TRAITS>& from);

                template <typename T2, typename T2_TRAITS>
                /*
                @METHOD:        SharedPtr::SharedPtr
                @DESCRIPTION:   <p>This CTOR is meant to allow for the semantics of assigning a sub-type pointer to a pointer
                            to the base class. There isn't any way to express this in template requirements, but this template
                            will fail to compile (error assigning to its fPtr_ member in the CTOR) if its ever used to
                            assign inappropriate pointer combinations.</p>
                */
                SharedPtr (const SharedPtr<T2, T2_TRAITS>& from);

            public:
                nonvirtual      SharedPtr<T, T_TRAITS>& operator= (const SharedPtr<T, T_TRAITS>& rhs);

            public:
                ~SharedPtr ();

            public:
                nonvirtual  bool        IsNull () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::GetRep
                @DESCRIPTION:   <p>Requires that the pointer is non-nullptr. You can call SharedPtr<T,T_TRAITS>::get ()
                    which whill return null without asserting if the pointer is allowed to be null.</p>
                */
                nonvirtual  T&          GetRep () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::operator->
                @DESCRIPTION:   <p>Note - this CAN NOT return nullptr (because -> semantics are typically invalid for a logically null pointer)</p>
                */
                nonvirtual  T* operator-> () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::operator*
                @DESCRIPTION:   <p></p>
                */
                nonvirtual  T& operator* () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::operator T*
                @DESCRIPTION:   <p>Note - this CAN return nullptr</p>
                */
                nonvirtual  operator T* () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::get
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
                            asserts about it being non-null.</p>
                */
                nonvirtual  T*      get () const;
            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::release
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer nullptr, but first return the
                            pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
                            <br>
                            NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
                            If you want the pointer before release, explicitly call get () first!!!
                            </p>
                */
                nonvirtual  void    release ();

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::clear
                @DESCRIPTION:   <p>Synonymn for SharedPtr<T,T_TRAITS>::release ()
                            </p>
                */
                nonvirtual  void    clear ();

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::reset
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
                            pre-existing pointer value. Unreference any previous value. Note - if there were more than one references
                            to the underlying object, its not destroyed.</p>
                */
                nonvirtual  void    reset (T* p = nullptr);

            public:
                template <typename T2>
                /*
                @METHOD:        SharedPtr::Dynamic_Cast
                @DESCRIPTION:   <p>Similar to SharedPtr<T2> () CTOR - which does base type. NB couldn't call this dynamic_cast -
                            thats a reserved word.</p>
                */
                nonvirtual  SharedPtr<T2> Dynamic_Cast ();

            public:
                // Returns true iff reference count of owned pointer is 1 (false if 0 or > 1)
                nonvirtual  bool    IsUnique () const;

            public:
                // Alias for IsUnique()
                nonvirtual  bool    unique () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::CurrentRefCount
                @DESCRIPTION:   <p>I used to keep this available only for debugging, but I've found a few
                cases where its handy outside the debugging context so not its awlays available (it has
                no cost to keep available).</p>
                */
                typename T_TRAITS::ReferenceCountType   CurrentRefCount () const;

            public:
                // Alias for CurrentRefCount()
                typename T_TRAITS::ReferenceCountType   use_count () const;

            public:
                nonvirtual  bool    operator< (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator<= (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator> (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator>= (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator== (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const SharedPtr<T, T_TRAITS>& rhs) const;


            public:
                nonvirtual  const typename T_TRAITS::Envelope& PeekAtEnvelope () const;

            private:
                typename    T_TRAITS::Envelope  fEnvelope_;

            private:
                template    <typename T2, typename T2_TRAITS>
                friend  class   SharedPtr;
            };











///DOCS OUT OF DATE - UPDATE -
            // An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
            // from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
            // This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
            // a layer of code, and then re-constitute the SharedPtr<> part later.

            /*
             * To enable the shared_from_this () functionality - and allow recovery of the SharedPtr<T> from the T* itself, its necessary to
             * combine the T type with the SharedPtr<T> infrastructure.
             *
             * To use, just inherit your type from enable_shared_from_this<>:
             *
             *      struct  TTT : Memory::enable_shared_from_this<TTT> {
             *          string x;
             *      };
             *      typedef SharedPtr<TTT,SharedPtrFromThis_Traits<TTT>>    TTT_SP;
             *
             *
             */
            template    <typename   T>
            class   enable_shared_from_this {
            private:
                Private::enable_shared_from_this_Traits_Helpers_::ReferenceCountType_   fCount_;

            public:
                enable_shared_from_this ();
                virtual ~enable_shared_from_this ();

            public:
#if     qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions
                template    <typename   RESULT_TRAITS = SharedPtrFromThis_Traits<T>>
                SharedPtr<T, RESULT_TRAITS> shared_from_this ();
#else
                template    <typename   RESULT_TRAITS>
                SharedPtr<T, RESULT_TRAITS> shared_from_this ();
                SharedPtr<T, SharedPtrFromThis_Traits<T>> shared_from_this ();
#endif

            private:
                friend  class   Private::enable_shared_from_this_Traits_Helpers_::Envelope_<T>;
            };




            /*
             * SO FAR FAILED - attempts at getting partial specialization to work. Reason todo that is so that
             *
             * then SharedPtr<T> will be the same as SharedPtr<T,SharedPtrFromThis_Traits<T>>
             */
#if     0
            template    <typename   T>
            struct  SharedPtr_Default_Traits<enable_shared_from_this<T>> :  SharedPtrFromThis_Traits<T> {
            };
#endif



        }




        namespace   Execution {
            template    <typename   T>
            void    ThrowIfNull (const Memory::SharedPtr<T>& p);
            template    <typename   T, typename T_TRAITS>
            void    ThrowIfNull (const Memory::SharedPtr<T, T_TRAITS>& p);
        }

    }
}
#endif  /*_Stroika_Foundation_Memory_SharedPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SharedPtr.inl"
