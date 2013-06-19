/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_inl_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {


                /*
                ********************************************************************************
                ********* String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly **********
                ********************************************************************************
                */
                inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
                    : inherited (s)
                {
                }
                inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s)
                {
                    inherited::operator= (s);
                    return *this;
                }


            }
        }
    }
}
#endif // _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_inl_
