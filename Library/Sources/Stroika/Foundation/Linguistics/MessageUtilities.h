/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Linguistics_MessageUtilities_h_
#define _Stroika_Foundation_Linguistics_MessageUtilities_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Configuration/Common.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::Linguistics {

    using Characters::String;

    struct MessageUtilities {
        virtual bool                           AppliesToThisLocale (const String& localeName) const        = 0;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const = 0;
    };
    struct MessageUtilities_en : MessageUtilities {
        virtual bool                           AppliesToThisLocale (const String& localeName) const override;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const override;
    };

    /**
     *  \brief These locale-specific functions all perform the given defined functions, except for those
     *         in the Configure sub-namespace;
     *
     *  \note These (CurrentLocaleMessageUtilities) functions CAN be called before / after main, but the value of 
     *        the locale will be unreliable ??? (need to decide), but they may use an arbitrary formatting implementation.
     */
    namespace CurrentLocaleMessageUtilities {
        class Configuration {
        private:
            Configuration ();
            Configuration (const Configuration&) = delete;
            ~Configuration ();

        public:
            static Configuration sThe;

        public:
            void AddHandler (const shared_ptr<MessageUtilities>& handler);
        };
        /**
         *  Uses a MessageUtilities based on the current thread's locale, if called between the start and end of main, and
         *  otherwise uses an arbitrary one.
         */
        pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MessageUtilities.inl"

#endif /*_Stroika_Foundation_Linguistics_MessageUtilities_h_*/
