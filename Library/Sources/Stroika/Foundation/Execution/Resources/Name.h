/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Name_h_
#define _Stroika_Foundation_Execution_Resources_Name_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Memory/Optional.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {
            namespace Resources {

                using namespace Stroika::Foundation::Characters;
                using namespace Stroika::Foundation::Configuration;

/**
                 */
#if qPlatform_Windows
                // This can be any Windows RT-type, such as RT_CURSOR, or RT_DIALOG
                using ResourceType = LPCTSTR;
#else
                using ResourceType = int;
#endif

                namespace PredefinedTypes {
                    extern const ResourceType kRES; // 'RES' - or default type
                }

                /**
                */
                class Name {
                public:
                    Name (const String& name, ResourceType type = PredefinedTypes::kRES);
#if qPlatform_Windows
                    Name (const int intResName, ResourceType type = PredefinedTypes::kRES);
#endif

                public:
                    /**
                     *  Just for viewing purposes, and will not necessarily be usable for low level resource calls.
                     */
                    nonvirtual String GetPrintName () const;

                public:
                    /**
                     *  NOTE - be careful with this - as its not threadsafe, and the lifetime of the SDKString exists
                     *  only until the next non-const call to (including destruction) of this Name object.
                     */
                    const SDKChar* GetSDKString () const;

                public:
                    nonvirtual ResourceType GetType () const;

                private:
                    SDKString fName_;
#if qPlatform_Windows
                    Memory::Optional<int> fIntName_;
#endif
                    ResourceType fType_;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Name.inl"

#endif /*_Stroika_Foundation_Execution_Resources_Name_h_*/
