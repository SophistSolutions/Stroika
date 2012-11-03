/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Name_h_
#define _Stroika_Foundation_Execution_Resources_Name_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {

                using   namespace   Stroika::Foundation::Characters;
                using   namespace   Stroika::Foundation::Configuration;

#if     qPlatform_Windows
                // This can be any Windows RT-type, such as RT_CURSOR, or RT_DIALOG
                typedef LPCTSTR ResourceType;
#else
                typedef int     ResourceType;
#endif

                namespace PredefinedTypes {
                    extern const ResourceType   kRES;       // 'RES' - or default type
                }

                class   Name {
                public:
                    Name (const String& name, ResourceType type = PredefinedTypes::kRES);

                public:
                    nonvirtual  String          GetName () const;
                    nonvirtual  ResourceType    GetType () const;

                private:
                    String          fName_;
                    ResourceType    fType_;
                };

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Resources_Name_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Name.inl"
