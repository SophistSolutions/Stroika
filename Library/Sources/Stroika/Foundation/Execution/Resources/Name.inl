/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Name_inl_
#define _Stroika_Foundation_Execution_Resources_Name_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {

                //  class Name
                inline Name::Name (const String& name, ResourceType type)
                    : fName_ (name)
                    , fType_ (type)
                {
                }
                inline String       Name::GetName () const
                {
                    return fName_;
                }
                inline ResourceType    Name::GetType () const
                {
                    return fType_;
                }

            }
        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Resources_Name_inl_*/
