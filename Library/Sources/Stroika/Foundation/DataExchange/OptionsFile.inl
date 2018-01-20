/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_inl_
#define _Stroika_Foundation_DataExchange_OptionsFile_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Characters/Format.h"
#include "../Streams/MemoryStream.h"

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {

            /*
             ********************************************************************************
             ************************** DataExchange::OptionsFile ***************************
             ********************************************************************************
             */
            template <typename T>
            Optional<T> OptionsFile::Read ()
            {
                Optional<VariantValue> tmp = Read<VariantValue> ();
                if (tmp.IsMissing ()) {
                    return Optional<T> ();
                }
                try {
                    return fMapper_.ToObject<T> (*tmp);
                }
                catch (const BadFormatException& /*bf*/) {
                    fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToParseReadFileBadFormat, GetReadFilePath_ ()));
                    return Optional<T> ();
                }
                catch (...) {
                    // if this fails, its probably because somehow the data in the config file was bad.
                    // So at least log that, and continue without reading anything (as if empty file)
                    fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToParseReadFile, GetReadFilePath_ ()));
                    return Optional<T> ();
                }
            }
            template <typename T>
            T OptionsFile::Read (const T& defaultObj, ReadFlags readFlags)
            {
                Optional<T> eltRead = Read<T> ();
                Optional<T> elt2Write; // only if needed

                LoggerMessage::Msg msgAugment = LoggerMessage::Msg::eWritingConfigFile_SoDefaultsEditable;
                if (eltRead.IsMissing ()) {
                    if (readFlags == ReadFlags::eWriteIfChanged) {
                        elt2Write  = defaultObj;
                        msgAugment = LoggerMessage::Msg::eWritingConfigFile_SoDefaultsEditable;
                    }
                }
                else {
                    if (readFlags == ReadFlags::eWriteIfChanged) {
                        if (elt2Write.IsMissing ()) {
                            // if filename differs - upgrading
                            if (GetReadFilePath_ () != GetWriteFilePath_ ()) {
                                elt2Write  = eltRead;
                                msgAugment = LoggerMessage::Msg::eWritingConfigFile_BecauseUpgraded;
                            }
                        }
                        if (elt2Write.IsMissing ()) {
                            try {
                                // See if re-persisting the item would change it.
                                // This is useful if your data model adds or removes fields. It updates the file contents written to the
                                // upgraded/latest form.
                                Memory::BLOB oldData = ReadRaw (); // @todo could have saved from previous Read<T>
                                Memory::BLOB newData;
                                {
                                    Streams::MemoryStream<Byte>::Ptr outStream = Streams::MemoryStream<Byte>::New ();
                                    fWriter_.Write (fMapper_.FromObject (*eltRead), outStream);
                                    // not sure needed? outStream.Flush();
                                    newData = outStream.As<Memory::BLOB> ();
                                }
                                if (oldData != newData) {
                                    elt2Write  = eltRead;
                                    msgAugment = LoggerMessage::Msg::eWritingConfigFile_BecauseSomethingChanged;
                                }
                            }
                            catch (...) {
                                fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToCompareReadFile, GetReadFilePath_ ()));
                            }
                        }
                    }
                }
                if (elt2Write.IsPresent ()) {
                    fLogger_ (LoggerMessage (msgAugment, GetWriteFilePath_ ()));
                    try {
                        Write (*elt2Write);
                    }
                    catch (...) {
                        fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToWriteInUseValues, GetWriteFilePath_ ()));
                    }
                    return *elt2Write;
                }
                else if (eltRead.IsPresent ()) {
                    return *eltRead;
                }
                else {
                    return defaultObj;
                }
            }
            template <typename T>
            void OptionsFile::Write (const T& optionsObject)
            {
                Write<VariantValue> (fMapper_.FromObject<T> (optionsObject));
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_OptionsFile_inl_*/
