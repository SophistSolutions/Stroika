/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Concrete_CompiledResourceManager_h_
#define _Stroika_Foundation_Execution_Resources_Concrete_CompiledResourceManager_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"

#include "../Manager.h"

/**
 *  \file
 *
 * TODO:
 *
 *      @todo   Do portable implementation which uses a set of pointers as initialized via CTOR. Not very
 *              usable CTOR API, but used by a tool
 *
 *      @todo   Add new TOOL – which generates initialized C++ structures that fit perfectly with the
 *              above portable implementation.
 *
 *      @todo   Tool takes args which are DIRS (if dir, recursively include), and explicit files. If explicit
 *              file list, no problem if they overlap in dirs.. That way – no need for –exclude param –
 *              you can always generate full list you wish to include (with find for example),
 *              and pass the full list to our app.
 *
 *      @todo   Define API (and impl from codegen) to use Stroika string etc classes –
 *              so can use ‘readonly’ strings.
 *
 *      @todo   Do implementation - NOTHING done.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {
            namespace Resources {
                namespace Concrete {

                    /**
                     * NYI, but this class will be paired with a compiler which takes an arbitrary file and converts it to
                     * an initialized c array, to be bound by the linker. The trikiest - TBD part of this - is figuring out
                     * how to compile a directory, and integrate the various resources into a single mapped manaager.
                     */
                    class CompiledResourceManager : public Manager {
                    public:
                        CompiledResourceManager ();
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
#include "CompiledResourceManager.inl"

#endif /*_Stroika_Foundation_Execution_Resources_Concrete_CompiledResourceManager_h_*/
