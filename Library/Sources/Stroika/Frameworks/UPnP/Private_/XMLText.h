/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Private_XMLText_h_
#define _Stroika_Frameworks_UPnP_Private_XMLText_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"

/**
 *  \file   For UPnP's XML read as text - not parsed - which needs no XML parser in the build
 */

namespace Stroika::Frameworks::UPnP::Private_ {

    using Foundation::Characters::String;

    /**
     *  XML text's character and entity references, as the five predefined entities and numbers - the text an element's
     *  <name>...</name> holds, as it means
     */
    inline String XMLText (const String& s)
    {
        using namespace Foundation::Characters;
        if (not s.Contains ('&')) {
            return s;
        }
        StringBuilder sb;
        for (size_t i = 0; i < s.size ();) {
            optional<size_t> semi = s[i] == '&' ? s.Find (';', i) : nullopt;
            if (not semi) {
                sb << s[i++];
                continue;
            }
            const String entity = s.SubString (i + 1, *semi);
            if (entity == "lt"sv) {
                sb << "<"sv;
            }
            else if (entity == "gt"sv) {
                sb << ">"sv;
            }
            else if (entity == "amp"sv) {
                sb << "&"sv;
            }
            else if (entity == "quot"sv) {
                sb << "\""sv;
            }
            else if (entity == "apos"sv) {
                sb << "'"sv;
            }
            else if (entity.StartsWith ("#x"sv) or entity.StartsWith ("#X"sv)) {
                sb << Character{static_cast<char32_t> (HexString2Int (entity.SubString (2)))};
            }
            else if (entity.StartsWith ("#"sv)) {
                sb << Character{static_cast<char32_t> (String2Int<uint32_t> (entity.SubString (1)))};
            }
            else {
                sb << s.SubString (i, *semi + 1); // not one we know: as it is
            }
            i = *semi + 1;
        }
        return sb;
    }

}

#endif /*_Stroika_Frameworks_UPnP_Private_XMLText_h_*/
