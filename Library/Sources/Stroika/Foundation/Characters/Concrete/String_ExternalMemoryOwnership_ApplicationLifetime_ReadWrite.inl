/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_inl_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_inl_

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
                ********* String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite *********
                ********************************************************************************
                */
                inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
                    : String (s)
                {
                }
                inline  String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s)
                {
                    String::operator= (s);
                    return (*this);
                }


            }
        }
    }
}

#endif // _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_inl_
