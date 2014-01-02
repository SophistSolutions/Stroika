/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_inl_
#define _Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_inl_

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
                     ***************************** ReadWriteRep::_Rep *******************************
                     ********************************************************************************
                     */
                    inline ReadWriteRep::_Rep:: _Rep (wchar_t* start, wchar_t* end)
                        : inherited (start, end)
                    {
                    }
                    inline  wchar_t*    ReadWriteRep::_Rep::_PeekStart ()
                    {
                        return const_cast<wchar_t*> (_fStart);
                    }


                }
            }
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_inl_
