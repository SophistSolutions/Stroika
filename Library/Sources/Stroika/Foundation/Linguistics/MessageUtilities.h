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
 *      @todo   Very VERY primitive linguistic support, but this could easily evolve over time. Led has a bunch of
 *              lingusitic code I could move here, and I'm sure I could dig up more...
 */

namespace Stroika::Foundation::Linguistics {

    using Characters::String;

    struct MessageUtilities {
        virtual bool                           AppliesToThisLocale (const String& localeName) const                              = 0;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const                       = 0;
        virtual String                         PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const = 0;
        virtual String                         MakeNounSingular (const String& s) const                                          = 0;
    };
    struct MessageUtilities_en : MessageUtilities {
        virtual bool                           AppliesToThisLocale (const String& localeName) const override;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const override;
        virtual String                         PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const override;
        virtual String                         MakeNounSingular (const String& s) const override;
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

        /**
         *  Implement current-ui-language-specific noun-pluralization logic for the given noun string (assuming the count of that noun
         *  is given (english rules - if count != 1 - append s, but we don't wnat that logic to proliferate through the app, so
         *  its easier to localize.
         *
         *  The variation with two strings - the second one is the explicit plural - just plugged in if the count is non-zero
         */
        String PluralizeNoun (const String& s, int count = 1000);
        String PluralizeNoun (const String& s, const String& sPlural, int count);

        /**
         * Take argument string (assumed noun) munge it so its singular (if it happened to have been plural).
         */
        String MakeNounSingular (const String& s);

    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MessageUtilities.inl"

#endif /*_Stroika_Foundation_Linguistics_MessageUtilities_h_*/
