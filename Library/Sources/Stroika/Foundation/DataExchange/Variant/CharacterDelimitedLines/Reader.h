/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Containers/Sequence.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/InputStream.h"

#include "../../VariantValue.h"

#include "../Reader.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace CharacterDelimitedLines {

                    using Characters::Character;
                    using Characters::String;
                    using Containers::Sequence;
                    using Containers::Set;
                    using Traversal::Iterable;

                    /**
                     *  @todo add example usage (and docs).
                     *
                     *  This COULD be easily used to read CSV files, for example.
                     */
                    class Reader : public Variant::Reader {
                    private:
                        using inherited = Variant::Reader;

                    private:
                        class Rep_;

                    public:
                        Reader (const Set<Character>& columnDelimiters);

                    public:
                        nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream<Memory::Byte>::Ptr& in) const;
                        nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream<Character>::Ptr& in) const;
                    };
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_*/
