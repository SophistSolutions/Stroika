/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


                    /*
                     ********************************************************************************
                     ***************************** BufferedStringRep::_Rep ********************************
                     ********************************************************************************
                     */

                    inline   BufferedStringRep::_Rep::_Rep (const wchar_t* start, const wchar_t* end)
                        : inherited (nullptr, nullptr)
                        , fCapacity_ (0)
                    {
                        size_t  len     =   end - start;
                        ReserveAtLeast_ (len);
                        if (len != 0) {
                            AssertNotNull (_PeekStart ());
                            (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                            _fEnd = _fStart + len;
                        }
                    }
                    inline     BufferedStringRep::_Rep:: _Rep (const wchar_t* start, const wchar_t* end, size_t reserve)
                        : inherited (nullptr, nullptr)
                        , fCapacity_ (0)
                    {
                        size_t  len     =   end - start;
                        ReserveAtLeast_ (max (len, reserve));
                        if (len != 0) {
                            AssertNotNull (_PeekStart ());
                            (void)::memcpy (_PeekStart (), start, len * sizeof (wchar_t));
                            _fEnd = _fStart + len;
                        }
                    }
                    //theory not tested- but inlined because its important this be fast, it it sb instantiated only in each of the 3 or 4 final DTORs
                    inline  BufferedStringRep::_Rep::~_Rep ()
                    {
#if     qUseBlockAllocatedForSmallBufStrings
                        if (fCapacity_ == kBlockAllocMinSizeInwchars) {
                            Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (_PeekStart ());
                            return;
                        }
#endif
                        delete[] _PeekStart ();
                    }

                }
            }
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_inl_
