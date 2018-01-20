/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Streams/OutputStream.h"

#include "../../VariantValue.h"

#include "../Writer.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace CharacterDelimitedLines {

                    /**
                     *  NOT YET IMPLEMENTED
                     */
                    class Writer : public Variant::Writer {
                    private:
                        using inherited = Variant::Writer;

                    private:
                        class Rep_;

                    public:
                        Writer ();

                    private:
                        nonvirtual shared_ptr<Rep_> GetRep_ () const;
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

#endif /*_Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_h_*/
