/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_inl_
#define _Stroika_Foundation_DataExchange_OptionsFile_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Characters/Format.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ************************** DataExchange::OptionsFile ***************************
             ********************************************************************************
             */
            template    <typename T>
            Optional<T>   OptionsFile::Read ()
            {
                Optional<VariantValue>  tmp = Read<VariantValue> ();
                if (tmp.IsMissing ()) {
                    return Optional<T> ();
                }
                return fMapper_.ToObject<T> (*tmp);
            }
            template    <typename T>
            T   OptionsFile::Read (const T& defaultObj)
            {
                Optional<T> result = Read<T> ();
                if (result.IsPresent ()) {
                    return *result;
                }
                else {
                    try {
                        Write (defaultObj);
                    }
                    catch (...) {
                        fLogWarning_ (Characters::Format (L"Failed to write default values to file: %s", fModuleNameToFileNameMapper_(fModuleName_).c_str ()));
                    }
                    return defaultObj;
                }
            }
            template    <typename T>
            void    OptionsFile::Write (const T& optionsObject)
            {
                Write<VariantValue> (fMapper_.FromObject<T> (optionsObject));
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_OptionsFile_inl_*/
