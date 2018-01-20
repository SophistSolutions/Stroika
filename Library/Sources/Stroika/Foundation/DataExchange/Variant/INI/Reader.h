/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_INI_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_INI_Reader_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Containers/Collection.h"

#include "../../VariantValue.h"

#include "../Reader.h"

#include "Profile.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Add DOCS
 *
 *      @todo   Quoted characters (see http://en.wikipedia.org/wiki/INI_file)
 *
 *      @todo   VERY PRIMITIVE IMPL (no error checking/validation) - or at least little
 *
 *      @todo   Add these referiences to docs:
 *              http://en.wikipedia.org/wiki/INI_file
 *              http://stackoverflow.com/questions/190629/what-is-the-easiest-way-to-parse-an-ini-file-in-java
 *
 *      @todo   Config params should take choice about quoting (always, never)???
 *
 *      @todo   Characterset / BOM
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace INI {

                    /**
                     *  See example usage in Regregssion test(@todo fixup)
                     */
                    class Reader : public Variant::Reader {
                    private:
                        using inherited = Variant::Reader;

                    private:
                        class Rep_;

                    public:
                        Reader ();

                    public:
                        /**
                        */
                        nonvirtual Profile ReadProfile (const Streams::InputStream<Memory::Byte>::Ptr& in);
                        nonvirtual Profile ReadProfile (const Streams::InputStream<Characters::Character>::Ptr& in);
                        nonvirtual Profile ReadProfile (istream& in);
                        nonvirtual Profile ReadProfile (wistream& in);
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
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_INI_Reader_h_*/
